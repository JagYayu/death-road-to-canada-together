--[[
-- @module dr2c.Client.World.Snapshot
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local String = require("TE.String")

local CNetworkClient = require("dr2c.Client.Network.Client")

local CNetworkClientMisc = require("dr2c.Client.Network.ClientMisc")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local GWorldSnapshot = require("dr2c.Shared.World.Snapshot")

local floor = math.floor

--- @class dr2c.SnapshotID : integer

--- Do not inject metatables/functions/threads... that are not serializable by LJBuffer into snapshot table.
--- Especially notice tables that are circular referenced or with metatables.
--- @class dr2c.CWorldSnapshot : dr2c.WorldSnapshot
local CWorldSnapshot = GWorldSnapshot.new()

local latestSnapshotRegistryID = 0
local snapshotRegistryTable = {}

latestSnapshotRegistryID = persist("latestSnapshotRegistryID", function()
	return latestSnapshotRegistryID
end)
snapshotRegistryTable = persist("snapshotRegistryTable", function()
	return snapshotRegistryTable
end)

local eventClientSnapshotCollect = TE.events:new(N_("CSnapshotCollect"), {
	"ECS",
})

local eventClientSnapshotDispense = TE.events:new(N_("CSnapshotDispense"), {
	"ECS",
})

--- @param name string
--- @param default any?
--- @return dr2c.SnapshotID snapshotID
function CWorldSnapshot.registerVariable(name, default)
	local scriptName = TE.scriptLoader:getLoadingScriptName()
	if scriptName then
		name = ("%s_%s"):format(name, scriptName)
	end

	local buffer = String.bufferEncode(default)
	local id

	if snapshotRegistryTable[name] then
		-- Override already existed snapshot registration.

		local entry = snapshotRegistryTable[name]
		id = entry[1]
		entry[3] = buffer
		entry[4] = String.bufferDecode(buffer)
	else
		latestSnapshotRegistryID = latestSnapshotRegistryID + 1
		id = latestSnapshotRegistryID
		local entry = {
			id,
			name,
			buffer,
			String.bufferDecode(buffer),
		}
		snapshotRegistryTable[name] = entry
		snapshotRegistryTable[#snapshotRegistryTable + 1] = entry
	end

	return id
end

function CWorldSnapshot.registerCoroutine(name)
	--
end

--- @return string data
function CWorldSnapshot.serialize()
	-- Utility.assertRuntime()

	local collection = {}
	TE.events:invoke(eventClientSnapshotCollect, {
		snapshot = collection,
	})

	for _, entry in ipairs(snapshotRegistryTable) do
		collection[entry[1]] = entry[4]
	end

	local data = String.bufferEncode(collection)
	return data
end

--- @param data string
function CWorldSnapshot.deserialize(data)
	-- Utility.assertRuntime()

	local success, collection = pcall(String.bufferDecode, data)
	if not success then
		error("Failed to deserialize!")
	end

	--- @cast collection table
	for _, entry in ipairs(snapshotRegistryTable) do
		entry[4] = collection[entry[1]]
	end

	TE.events:invoke(eventClientSnapshotDispense, {
		snapshot = collection,
	})
end

--- Request a snapshot from server.
--- 请求服务器的快照信息。
--- @param snapshotID dr2c.SnapshotID
function CWorldSnapshot.request(snapshotID)
	return CNetworkClient.sendReliable(GNetworkMessage.Type.SnapshotRequest, snapshotID)
end

--- Upload snapshots to server, only allowed for clients with permission `GNetworkClient.Permission.Authority`.
--- 上传快照给服务器，只允许带`GNetworkClient.Permission.Authority`许可的客户端上传。
--- @return boolean?
function CWorldSnapshot.upload()
	if CNetworkClientMisc.hasPermissionAuthority() then
		return CNetworkClient.sendReliable(GNetworkMessage.Type.Snapshots, CWorldSnapshot.getSnapshotDataList())
	end
end

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	local snapshotID = e.content[1]

	CWorldSnapshot.dropBackward(snapshotID)
end, "ReceiveSnapshotResponse", "Receive", GNetworkMessage.Type.SnapshotResponse)

--- @param snapshotID integer
function CWorldSnapshot.save(snapshotID)
	local snapshotData = CWorldSnapshot.serialize()
	CWorldSnapshot.setSnapshotData(snapshotID, snapshotData)

	if CNetworkClientMisc.hasPermissionAuthority() then
		local fields = GNetworkMessageFields.Snapshot
		CNetworkClient.sendReliable(GNetworkMessage.Type.Snapshot, {
			[fields.snapshotID] = snapshotID,
			[fields.snapshotData] = snapshotData,
		})
	end
end

--- @param snapshotID integer
--- @return boolean
function CWorldSnapshot.load(snapshotID)
	local firstID = CWorldSnapshot.getFirstSnapshotID()
	local list = CWorldSnapshot.getSnapshotDataList()

	if not firstID then
		return false
	elseif snapshotID < firstID then
		return false
	elseif snapshotID >= firstID + #list then
		return false
	end

	CWorldSnapshot.deserialize(list[snapshotID])

	return true
end

local function resetSnapshots()
	CWorldSnapshot.clear()
end

TE.events:add(N_("CConnect"), resetSnapshots, N_("ResetSnapshot"), "Reset")

TE.events:add(N_("CDisconnect"), resetSnapshots, N_("ResetSnapshot"), "Reset")

TE.events:add(N_("CWorldSessionStart"), resetSnapshots, "ResetSnapshots", "Reset")

TE.events:add(N_("CWorldSessionFinish"), resetSnapshots, "ResetSnapshots", "Reset")

TE.events:add(N_("CWorldTickProcess"), function(e)
	-- Save snapshot every tick, for now :)
	CWorldSnapshot.save(GWorldSnapshot.tick2id(e.tick))

	-- Then drop faraway snapshots
	local maximumSnapshotCount = 300 -- 10s * 30tps
	if CWorldSnapshot.getSnapshotCount() > maximumSnapshotCount then
		CWorldSnapshot.dropForward(latestSnapshotRegistryID)
	end
end, "SaveSnapshot", "SnapshotSave")

TE.events:add(N_("CWorldRollback"), function(e)
	if e.suppressed then
		return
	end

	if CWorldSnapshot.load(e.tick) then
		CWorldSnapshot.dropBackward(e.tick)

		if log.canTrace() then
			log.trace(("Rollback: load snapshot to tick %d"):format(e.tick))
		end
	else
		e.suppressed = true
	end
end, N_("UpdateSnapshots"), "Snapshot")

return CWorldSnapshot
