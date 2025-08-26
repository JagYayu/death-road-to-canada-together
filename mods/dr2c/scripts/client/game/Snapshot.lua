local String = require("tudov.String")

--- Do not inject metatables/functions/threads... that are not serializable by LJBuffer into snapshot table.
--- Especially circular referenced table and
--- @class dr2c.CSnapshot
local CSnapshot = {}

local snapshotLatestID = 0
local snapshotRegistrationTable = {}
local snapshots = {}

function CSnapshot.getAll()
	return snapshots
end

--- @param name string
--- @param default any?
--- @return integer snapshotID
function CSnapshot.register(name, default)
	local buffer = String.bufferEncode(default)

	snapshotLatestID = snapshotLatestID + 1
	local id = snapshotLatestID

	local entry = {
		id,
		name,
		default,
		buffer,
	}
	snapshotRegistrationTable[name] = entry
	snapshots[id] = entry

	return id
end

function CSnapshot.get(snapshotID) end

local snapshotLJBuffer = String.bufferNew(1024)

function CSnapshot.serialize()
	local pcall = pcall
	local encode = snapshotLJBuffer.encode

	for _, entry in ipairs(snapshots) do
		local success, result = pcall(encode, snapshotLJBuffer, entry[3])
		if success then
		elseif log.canError() then
			log.error(result)
		end
	end

	local data = snapshotLJBuffer:tostring()
	snapshotLJBuffer:reset()
	return data
end

return CSnapshot
