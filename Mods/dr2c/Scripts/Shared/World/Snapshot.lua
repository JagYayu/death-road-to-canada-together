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

local Table = require("TE.Table")

local math_floor = math.floor

--- @class dr2c.SnapshotID : integer

--- @class dr2c.GWorldSnapshot
local GWorldSnapshot = {}

--- @type table<string, dr2c.WorldSnapshot>
local modules = setmetatable({}, { __mode = "v" })

local ticksPerID = 1
local idsPerTick = 1 / ticksPerID

modules = persist("modulesData", function()
	return modules
end)

function GWorldSnapshot.getAll()
	return modules
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
	local GWorldTick = require("dr2c.Shared.World.Tick")

	--- @class dr2c.WorldSnapshot
	local WorldSnapshot = {}

	--- 快照数据列表
	--- 快照ID都是连续的，中间不允许出现ID空洞或跳跃，因此只能连续批量地移除。
	--- @type string[]
	local snapshotDataList = {}
	--- 第一个快照的ID
	--- @type dr2c.SnapshotID?
	local firstSnapshotID = nil

	local scriptName = TE.scriptLoader:getLoadingScriptName()
	if scriptName ~= "" then
		if modules[scriptName] then
			snapshotDataList = modules[scriptName][1]
			firstSnapshotID = modules[scriptName][2]
		else
			modules[scriptName] = {
				snapshotDataList,
				firstSnapshotID,
			}
		end
	end

	function WorldSnapshot.clear()
		snapshotDataList = {}
		firstSnapshotID = nil
	end

	--- @return string[]
	function WorldSnapshot.getSnapshotDataList()
		return snapshotDataList
	end

	function WorldSnapshot.setSnapshotDataList(snapshotDataList_)
		snapshotDataList = snapshotDataList_
		firstSnapshotID = snapshotDataList[1]
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

	--- @param snapshotID dr2c.SnapshotID
	--- @return string?
	function WorldSnapshot.getSnapshotData(snapshotID)
		if not firstSnapshotID then
			return
		elseif snapshotID < firstSnapshotID then
			return
		elseif snapshotID >= firstSnapshotID + #snapshotDataList then
			return
		end

		return snapshotDataList[snapshotID - firstSnapshotID + 1]
	end

	--- @param snapshotID integer
	--- @param snapshotData string
	function WorldSnapshot.setSnapshotData(snapshotID, snapshotData)
		if snapshotID <= 0 then
			error("SnapshotID must be greater than 0", 2)
		end

		WorldSnapshot.dropBackward(snapshotID)

		-- print(#snapshotDataList, firstSnapshotID and (snapshotID - firstSnapshotID), snapshotID, firstSnapshotID)
		if firstSnapshotID then
			local index = snapshotID - firstSnapshotID
			if not snapshotDataList[index] then
				print(snapshotDataList)
				error("Snapshot list cannot have holes", 2)
			end

			snapshotDataList[index + 1] = snapshotData
		else
			snapshotDataList[1] = snapshotData
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
		local index = snapshotID - firstSnapshotID + 1

		if index <= 1 then
			Table.clear(snapshotDataList)
			firstSnapshotID = nil

			return len
		else
			for i = len, index, -1 do
				snapshotDataList[i] = nil
			end

			return len - snapshotID + firstSnapshotID
		end
	end

	--- Remove current and previous snapshots.
	--- 移除当前及先前的快照。
	--- @return integer count
	function WorldSnapshot.dropForward(snapshotID)
		if not firstSnapshotID then
			return 0
		end

		local len = #snapshotDataList
		local count = snapshotID - firstSnapshotID + 1

		if count >= len then
			Table.clear(snapshotDataList)
			firstSnapshotID = nil

			return len
		else
			for i = count + 1, len do
				local j = i - count
				snapshotDataList[j] = snapshotDataList[i]
				snapshotDataList[i] = nil
			end

			for i = len, len - count + 1, -1 do
				snapshotDataList[i] = nil
			end

			firstSnapshotID = firstSnapshotID + count
			return count
		end
	end

	--- @param snapshotLifetime number
	--- @return integer?
	function WorldSnapshot.dropOldSnapshots(snapshotLifetime, factor)
		factor = factor or 2
		local maxCount = math_floor(snapshotLifetime * GWorldTick.getTPS() * idsPerTick) * factor -- 3 * CWorldTick.getTPS()

		if #snapshotDataList > maxCount then
			local snapshotID = WorldSnapshot.getLastSnapshotID() - math.floor(maxCount / factor)

			local droppedNumber = WorldSnapshot.dropForward(snapshotID)

			return droppedNumber
		end
	end

	return WorldSnapshot
end

TE.events:add("DebugSnapshot", function(e)
	e.worldSnapshotModules = modules
end, scriptName, nil, scriptName)

return GWorldSnapshot
