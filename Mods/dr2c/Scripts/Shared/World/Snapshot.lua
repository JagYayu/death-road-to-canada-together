--[[
-- @module dr2c.Shared.World.Snapshot
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local String = require("TE.String")
local Utility = require("TE.Utility")

local GNetworkMessage = require("dr2c.Shared.Network.Message")

local hasServer = pcall(require, "dr2c.Server.World.Snapshot")

local floor = math.floor

--- @class dr2c.SnapshotID : integer

--- @class dr2c.GWorldSnapshot
local GWorldSnapshot = {}

--- @type table<string, dr2c.WorldSnapshot>
local worldSnapshotModules = setmetatable({}, { __mode = "v" })
worldSnapshotModules = persist("worldSnapshotModules", function()
	return worldSnapshotModules
end)

function GWorldSnapshot.getAll()
	return worldSnapshotModules
end

function GWorldSnapshot.new()
	local scriptName = TE.scriptLoader:getLoadingScriptName()
	if scriptName ~= "" and worldSnapshotModules[scriptName] then
		return worldSnapshotModules[scriptName]
	end

	--- @class dr2c.WorldSnapshot
	local WorldSnapshot = {}

	--- @class dr2c.WorldSnapshotTable
	--- @field first dr2c.SnapshotID?
	local snapshotTable = {
		first = nil,
	}

	worldSnapshotModules[scriptName] = WorldSnapshot

	function WorldSnapshot.clear()
		snapshotTable = {
			first = nil,
		}
	end

	--- @param snapshotID integer
	function WorldSnapshot.set(snapshotID, data)
		local firstID = snapshotTable.first

		if snapshotID <= 0 then
			error("snapshotID must be greater than 0", 2)
		elseif firstID and not snapshotTable[snapshotID - 1] then
			error("Snapshot list cannot have holes", 2)
		end

		snapshotTable[snapshotID] = data

		if not firstID then
			snapshotTable.first = snapshotID
		end
	end

	--- @param snapshotID integer
	--- @return string?
	function WorldSnapshot.get(snapshotID)
		if snapshotID ~= floor(snapshotID) then
			return
		end

		local firstID = snapshotTable.first
		if not firstID then
			return
		elseif snapshotID < firstID then
			return
		elseif snapshotID >= firstID + #snapshotTable then
			return
		end

		return snapshotTable[snapshotID]
	end

	--- Remove current and subsequent snapshots.
	--- @return integer count
	function WorldSnapshot.dropBackward(snapshotID)
		local firstID = snapshotTable.first
		if not firstID then
			return 0
		end

		local len = #snapshotTable
		for i = len, snapshotID - firstID + 1, -1 do
			snapshotTable[i] = nil
		end

		return len - snapshotID + firstID
	end

	--- Remove current and previous snapshots.
	--- @return integer count
	function WorldSnapshot.dropForward(snapshotID)
		local firstID = snapshotTable.first
		if not firstID then
			return 0
		end

		local len = #snapshotTable
		local diff = snapshotID - firstID

		for i = len - diff, 1, -1 do
			local j = i + diff
			snapshotTable[i] = snapshotTable[j]
			snapshotTable[j] = nil
		end

		for i = diff, len - diff + 1, -1 do
			snapshotTable[i] = nil
		end

		return diff + 1
	end

	return WorldSnapshot
end

TE.events:add("DebugSnapshot", function(e)
	e.worldSnapshotModules = worldSnapshotModules
end, scriptName, nil, scriptName)

return GWorldSnapshot
