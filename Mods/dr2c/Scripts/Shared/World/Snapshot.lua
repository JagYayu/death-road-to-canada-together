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

local math_floor = math.floor

--- @class dr2c.SnapshotID : integer

--- @class dr2c.GWorldSnapshot
local GWorldSnapshot = {}

--- @type table<string, dr2c.WorldSnapshot>
local worldSnapshotModules = setmetatable({}, { __mode = "v" })

local ticksPerID = 1
local idsPerTick = 1 / ticksPerID

worldSnapshotModules = persist("worldSnapshotModules", function()
	return worldSnapshotModules
end)

function GWorldSnapshot.getAll()
	return worldSnapshotModules
end

--- @param snapshotID dr2c.SnapshotID
--- @return dr2c.WorldTick
--- @nodiscard
function GWorldSnapshot.id2tick(snapshotID)
	return snapshotID * ticksPerID
end

--- @param worldTick dr2c.WorldTick
--- @return dr2c.SnapshotID
--- @nodiscard
function GWorldSnapshot.tick2id(worldTick)
	return math_floor(worldTick * idsPerTick)
end

--- @return dr2c.WorldSnapshot
--- @nodiscard
function GWorldSnapshot.new()
	local scriptName = TE.scriptLoader:getLoadingScriptName()
	if scriptName ~= "" and worldSnapshotModules[scriptName] then
		return worldSnapshotModules[scriptName]
	end

	--- @class dr2c.WorldSnapshot
	local WorldSnapshot = {}

	worldSnapshotModules[scriptName] = WorldSnapshot

	--- 快照数据列表
	--- 快照ID都是连续的，中间不允许出现ID空洞或跳跃，因此只能连续批量地移除。
	--- @type string[]
	local snapshotDataList = {}
	--- 第一个快照的ID
	--- @type dr2c.SnapshotID?
	local firstSnapshotID = nil

	function WorldSnapshot.clear()
		snapshotDataList = {}
		firstSnapshotID = nil
	end

	--- @return string[]
	function WorldSnapshot.getSnapshotDataList()
		return snapshotDataList
	end

	--- @return integer
	function WorldSnapshot.getSnapshotCount()
		return #snapshotDataList
	end

	--- @return dr2c.SnapshotID?
	function WorldSnapshot.getFirstSnapshotID()
		return firstSnapshotID
	end

	--- @return dr2c.SnapshotID?
	function WorldSnapshot.getLastSnapshotID()
		return firstSnapshotID and (firstSnapshotID + #snapshotDataList - 1) or nil
	end

	--- @param snapshotID integer
	--- @return string?
	function WorldSnapshot.getSnapshotData(snapshotID)
		if snapshotID ~= math_floor(snapshotID) then
			return
		end

		if not firstSnapshotID then
			return
		elseif snapshotID < firstSnapshotID then
			return
		elseif snapshotID >= firstSnapshotID + #snapshotDataList then
			return
		end

		return snapshotDataList[snapshotID]
	end

	--- @param snapshotID integer
	--- @param snapshotData string
	function WorldSnapshot.setSnapshotData(snapshotID, snapshotData)
		if snapshotID <= 0 then
			error("snapshotID must be greater than 0", 2)
		elseif firstSnapshotID and not snapshotDataList[snapshotID - 1] then
			error("Snapshot list cannot have holes", 2)
		end

		snapshotDataList[snapshotID] = snapshotData

		if not firstSnapshotID then
			firstSnapshotID = snapshotID
		end
	end

	--- Remove current and subsequent snapshots.
	--- 移除当前及之后的快照。
	--- @return integer count
	function WorldSnapshot.dropBackward(snapshotID)
		if not firstSnapshotID then
			return 0
		end

		local len = #snapshotDataList
		for i = len, snapshotID - firstSnapshotID + 1, -1 do
			snapshotDataList[i] = nil
		end

		return len - snapshotID + firstSnapshotID
	end

	--- Remove current and previous snapshots.
	--- 移除当前及先前的快照。
	--- @return integer count
	function WorldSnapshot.dropForward(snapshotID)
		if not firstSnapshotID then
			return 0
		end

		local len = #snapshotDataList
		local diff = snapshotID - firstSnapshotID

		for i = len - diff, 1, -1 do
			local j = i + diff
			snapshotDataList[i] = snapshotDataList[j]
			snapshotDataList[j] = nil
		end

		for i = diff, len - diff + 1, -1 do
			snapshotDataList[i] = nil
		end

		return diff + 1
	end

	return WorldSnapshot
end

TE.events:add("DebugSnapshot", function(e)
	e.worldSnapshotModules = worldSnapshotModules
end, scriptName, nil, scriptName)

return GWorldSnapshot
