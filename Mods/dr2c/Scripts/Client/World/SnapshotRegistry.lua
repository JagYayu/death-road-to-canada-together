--[[
-- @module dr2c.Client.World.SnapshotRegistry
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local String = require("TE.String")

--- Do not inject metatables/functions/threads... that are not serializable by LJBuffer into snapshot table.
--- Especially notice tables that are circular referenced or with metatables.
--- @class dr2c.CWorldSnapshotRegistry
local CWorldSnapshotRegistry = {}

local latestSnapshotRegistryID = 0
local snapshotRegistryTable = {}

latestSnapshotRegistryID = persist("latestSnapshotRegistryID", function()
	return latestSnapshotRegistryID
end)
snapshotRegistryTable = persist("snapshotRegistryTable", function()
	return snapshotRegistryTable
end)

--- @param name string
--- @param default any?
--- @return dr2c.SnapshotID snapshotID
function CWorldSnapshotRegistry.registerVariable(name, default)
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

function CWorldSnapshotRegistry.registerCoroutine(name)
	--
end

TE.events:add(N_("CSnapshotCollect"), function(e)
	local collection = e.snapshot

	for _, entry in ipairs(snapshotRegistryTable) do
		entry[4] = collection[entry[1]]
	end
end, "FromRegistry", "Registry")

TE.events:add(N_("CSnapshotDispense"), function(e)
	-- TODO
end, "ToRegistry", "Registry")

return CWorldSnapshotRegistry
