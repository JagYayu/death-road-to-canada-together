local String = require("tudov.String")
local Utility = require("tudov.Utility")

local CClient = require("dr2c.client.network.Client")
local CServer = require("dr2c.client.network.Server")
local GMessage = require("dr2c.shared.network.Message")

local hasServer = pcall(require, "dr2c.server.world.Snapshot")

--- @class dr2c.SnapshotID : integer

--- Do not inject metatables/functions/threads... that are not serializable by LJBuffer into snapshot table.
--- Especially notice tables that are circular referenced or with metatables.
--- @class dr2c.CSnapshot
local CSnapshot = {}

local ticksPerSnapshot = 1

local snapshotRegistrationLatestID = 0
local snapshotRegistrationTable = {}

--- @class dr2c.ClientSnapshots
--- @field first dr2c.SnapshotID?
--- @field [integer] string
local clientSnapshots = {
	first = nil,
}

local eventClientSnapshotCollect = events:new(N_("CSnapshotCollect"), {
	"ECS",
})

local eventClientSnapshotDispense = events:new(N_("CSnapshotDispense"), {
	"ECS",
})

function CSnapshot.getAll()
	return clientSnapshots
end

function CSnapshot.clearAll()
	clientSnapshots = {
		first = nil,
	}
end

--- @param snapshotID dr2c.SnapshotID
--- @return string?
function CSnapshot.getSnapshot(snapshotID)
	local firstID = clientSnapshots.first
	if firstID then
		return clientSnapshots[snapshotID - firstID + 1]
	end
end

--- @param index any
--- @return nil
function CSnapshot.getSnapshotID(index)
	local firstID = clientSnapshots.first
	return firstID and (firstID + index - 1) or nil
end

--- @return dr2c.SnapshotID?
function CSnapshot.getFirstSnapshotID()
	return clientSnapshots.first
end

--- @return dr2c.SnapshotID?
function CSnapshot.getLastSnapshotID()
	local firstID = clientSnapshots.first
	return firstID and (firstID + #clientSnapshots - 1) or nil
end

--- @param name string
--- @param default any?
--- @return dr2c.SnapshotID snapshotID
function CSnapshot.register(name, default)
	local scriptName = scriptLoader:getLoadingScriptName()
	if scriptName then
		name = ("%s_%s"):format(name, scriptName)
	end

	local buffer = String.bufferEncode(default)
	local id

	if snapshotRegistrationTable[name] then
		local entry = snapshotRegistrationTable[name]
		id = entry[1]
		entry[3] = buffer
		entry[4] = String.bufferDecode(buffer)
	else
		snapshotRegistrationLatestID = snapshotRegistrationLatestID + 1
		id = snapshotRegistrationLatestID
		local entry = {
			id,
			name,
			buffer,
			String.bufferDecode(buffer),
		}
		snapshotRegistrationTable[name] = entry
		snapshotRegistrationTable[#snapshotRegistrationTable + 1] = entry
	end

	return id
end

function CSnapshot.serialize()
	-- Utility.assertRuntime()

	local collection = {}
	events:invoke(eventClientSnapshotCollect, {
		snapshot = collection,
	})

	for _, entry in ipairs(snapshotRegistrationTable) do
		collection[entry[1]] = entry[4]
	end

	local data = String.bufferEncode(collection)
	return data
end

function CSnapshot.deserialize(data)
	-- Utility.assertRuntime()

	local success, collection = pcall(String.bufferDecode, data)
	if not success then
		error("Failed to deserialize!")
	end

	--- @cast collection table
	for _, entry in ipairs(snapshotRegistrationTable) do
		entry[4] = collection[entry[1]]
	end

	events:invoke(eventClientSnapshotDispense, {
		snapshot = collection,
	})
end

--- Request a snapshot from server.
--- @param snapshotID dr2c.SnapshotID
function CSnapshot.request(snapshotID)
	CClient.sendReliable(GMessage.Type.SnapshotRequest, {
		snapshotID = snapshotID,
	})
end

--- Deliver snapshots to server.
--- @deprecated
function CSnapshot.deliver()
	-- CClient.sendReliable(GMessage.Type.Snapshot, clientSnapshots)
end

--- @param e dr2c.E.ClientMessage
events:add(N_("CMessage"), function(e)
	if hasServer then
		CSnapshot.dropBackward(e.content.snapshotID)

		e.content.snapshotID = e.content.snapshot
	end
end, "ReceiveSnapshotResponse", "Receive", GMessage.Type.SnapshotResponse)

--- @param snapshotID integer
function CSnapshot.save(snapshotID)
	clientSnapshots[snapshotID] = CSnapshot.serialize()

	if not clientSnapshots.first then
		clientSnapshots.first = snapshotID
	end

	if hasServer then
		CClient.sendReliable(GMessage.Type.Snapshot, {
			snapshotID = snapshotID,
			snapshot = clientSnapshots[snapshotID],
		})
	end
end

--- @param snapshotID integer
--- @return boolean
function CSnapshot.load(snapshotID)
	local savedData = clientSnapshots[snapshotID]
	if savedData then
		CSnapshot.deserialize(savedData)

		return true
	else
		return false
	end
end

--- Remove current and subsequent snapshots.
--- @return integer count
function CSnapshot.dropBackward(snapshotID)
	local firstID = clientSnapshots.first
	if not firstID then
		return 0
	end

	local len = #clientSnapshots
	for i = len, snapshotID - firstID + 1, -1 do
		clientSnapshots[i] = nil
	end

	return len - snapshotID + firstID
end

--- Remove current and previous snapshots.
--- @return integer count
function CSnapshot.dropForward(snapshotID)
	local firstID = clientSnapshots.first
	if not firstID then
		return 0
	end

	local len = #clientSnapshots
	local diff = snapshotID - firstID

	for i = len - diff, 1, -1 do
		local j = i + diff
		clientSnapshots[i] = clientSnapshots[j]
		clientSnapshots[j] = nil
	end

	for i = diff, len - diff + 1, -1 do
		clientSnapshots[i] = nil
	end

	return diff + 1
end

events:add(N_("CDisconnect"), CSnapshot.clearAll, N_("ResetSnapshot"), "Reset")

events:add(N_("CWorldTickProcess"), function(e)
	if true then
		CSnapshot.save(e.tick)
	end
end, N_("SaveSnapshot"), "SnapshotSave")

events:add(N_("CWorldRollback"), function(e)
	CSnapshot.load(e.tick)
	CSnapshot.dropBackward(e.tick)
end, N_("UpdateSnapshots"), "Snapshot")

return CSnapshot
