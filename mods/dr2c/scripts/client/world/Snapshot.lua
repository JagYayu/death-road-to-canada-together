local String = require("tudov.String")
local Utility = require("tudov.Utility")

--- Do not inject metatables/functions/threads... that are not serializable by LJBuffer into snapshot table.
--- Especially circular referenced table and
--- @class dr2c.CSnapshot
local CSnapshot = {}

local snapshotRegistrationLatestID = 0
local snapshotRegistrationTable = {}

local savedSnapshots = {}

local eventClientSnapshotCollect = events:new(N_("CSnapshotCollect"), {
	"ECS",
})

local eventClientSnapshotDispense = events:new(N_("CSnapshotDispense"), {
	"ECS",
})

function CSnapshot.getAll()
	return savedSnapshots
end

--- @param name string
--- @param default any?
--- @return integer snapshotID
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
		savedSnapshots[id] = entry
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

--- @param snapshotID integer
function CSnapshot.save(snapshotID)
	savedSnapshots[snapshotID] = CSnapshot.serialize()
end

--- @param snapshotID integer
function CSnapshot.delete(snapshotID)
	savedSnapshots[snapshotID] = nil
end

--- @param snapshotID integer
--- @return boolean
function CSnapshot.load(snapshotID)
	local savedData = savedSnapshots[snapshotID]
	if savedData then
		CSnapshot.deserialize(savedData)

		return true
	else
		return false
	end
end

return CSnapshot
