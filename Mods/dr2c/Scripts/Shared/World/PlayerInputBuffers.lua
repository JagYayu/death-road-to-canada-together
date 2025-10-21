--[[
-- @module dr2c.Shared.World.PlayerInputBuffers
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("TE.Table")

local assert = assert
local math_huge = math.huge
local math_max = math.max
local math_min = math.min

--- @class dr2c.GPlayerInputBuffers
local GPlayerInputBuffers = {}

--- @type table<string, dr2c.PlayerInputBuffers>
local modules = {}

modules = persist("modules", function()
	return modules
end)

--- @param playerID dr2c.PlayerID
local function throwPlayerNotFound(playerID)
	error(("Player %s does not exists"):format(playerID), 3)
end

--- @param worldTick dr2c.WorldTick
local function checkWorldTickValidation(worldTick)
	if type(worldTick) ~= "number" then
		error("Bad argument to 'worldTick': number expected", 3)
	elseif worldTick <= 0 then
		error("Bad argument to 'worldTick': 'worldTick' should be greater than 0", 3)
	end
end

--- @return dr2c.PlayerInputBuffers module
function GPlayerInputBuffers.new()
	local GPlayerInput = require("dr2c.Shared.World.PlayerInput")
	local GWorldTick = require("dr2c.Shared.World.Tick")

	--- @class dr2c.PlayerTickInputs
	--- @field [1]? table<dr2c.PlayerInputID, Serializable> @A map of inputs, where store continuous inputs.
	--- @field [2]? { [1]: dr2c.PlayerInputID, [2]: Serializable }[] @A list of inputs.

	--- @class dr2c.PlayerTicksInputs
	--- @field [dr2c.WorldTick] dr2c.PlayerTickInputs
	--- @field beginTick? dr2c.WorldTick
	--- @field endTick dr2c.WorldTick

	--- @class dr2c.PlayersTicksInputs
	--- @field [dr2c.PlayerID] dr2c.PlayerTicksInputs

	--- @class dr2c.PlayerInputBuffers
	local PlayerInputBuffers = {}

	--- @type dr2c.PlayersTicksInputs
	local playersTicksInputsData = {}
	--- 归档刻，也就是所有玩家的输入都被确定了最新世界刻。
	--- @type dr2c.WorldTick
	local playersTicksInputsArchivedTick = 0

	local scriptName = TE.scriptLoader:getLoadingScriptName()
	if scriptName ~= "" then -- 模块数据持久化
		if modules[scriptName] then
			playersTicksInputsData, playersTicksInputsArchivedTick = unpack(modules[scriptName])
		else
			modules[scriptName] = {
				playersTicksInputsData,
				playersTicksInputsArchivedTick,
			}
		end
	end

	--- @warn Do not use this function unless you know what you're doing.
	--- @return dr2c.PlayersTicksInputs data
	--- @nodiscard
	function PlayerInputBuffers.getData()
		return playersTicksInputsData
	end

	--- 获取归档刻
	--- @return dr2c.WorldTick archivedTick
	--- @nodiscard
	function PlayerInputBuffers.getArchivedTick()
		return playersTicksInputsArchivedTick
	end

	--- 清理所有玩家输入
	function PlayerInputBuffers.clear()
		for playerID in pairs(playersTicksInputsData) do
			playersTicksInputsData[playerID] = { endTick = 0 }
		end

		playersTicksInputsArchivedTick = 0
	end

	--- 检测玩家是否已添加
	--- @param playerID dr2c.PlayerID
	--- @return boolean
	--- @nodiscard
	function PlayerInputBuffers.hasPlayer(playerID)
		return not not playersTicksInputsData[playerID]
	end

	--- 添加某一个玩家
	--- @param playerID dr2c.PlayerID
	function PlayerInputBuffers.addPlayer(playerID)
		if playersTicksInputsData[playerID] then
			error(("Player %s already exists"):format(playerID), 2)
		end

		playersTicksInputsData[playerID] = { endTick = 0 }
	end

	--- 移除某一个玩家
	--- @param playerID dr2c.PlayerID
	function PlayerInputBuffers.removePlayer(playerID)
		if not playersTicksInputsData[playerID] then
			throwPlayerNotFound(playerID)
		end

		playersTicksInputsData[playerID] = nil
	end

	--- 获取玩家在某一世界刻的输入
	--- @param playerID dr2c.PlayerID
	--- @param worldTick dr2c.WorldTick
	--- @return dr2c.PlayerTickInputs? tickInputs
	function PlayerInputBuffers.getInputs(playerID, worldTick)
		local playerTicksInputs = playersTicksInputsData[playerID]
		if not playerTicksInputs then
			throwPlayerNotFound(playerID)
		end

		checkWorldTickValidation(worldTick)

		return playerTicksInputs[worldTick]
	end

	--- 给一个玩家在某一世界刻添加输入，该函数应该只作用于本地玩家
	--- @param playerID dr2c.PlayerID
	--- @param worldTick dr2c.WorldTick
	--- @param playerInputID dr2c.PlayerInputID
	--- @param playerInputArg Serializable
	--- @return boolean success @若该世界刻已归档，返回`false`，否则为`true`
	function PlayerInputBuffers.addInput(playerID, worldTick, playerInputID, playerInputArg)
		-- if worldTick <= playersTicksInputsArchivedTick then
		-- 	return false
		-- end

		local playerTicksInputs = playersTicksInputsData[playerID]
		if not playerTicksInputs then
			throwPlayerNotFound(playerID)
		end

		checkWorldTickValidation(worldTick)

		local tickInputs = playerTicksInputs[worldTick]
		if not tickInputs then
			tickInputs = {}
			playerTicksInputs[worldTick] = tickInputs
		end

		playerTicksInputs.beginTick = math_min(playerTicksInputs.beginTick or worldTick, worldTick)
		playerTicksInputs.endTick = math_max(playerTicksInputs.endTick, worldTick)

		if GPlayerInput.isContinuous(playerInputID) then
			local map = tickInputs[1]
			if not map then
				map = {}
				tickInputs[1] = map
			end

			map[playerInputID] = playerInputArg
		else
			local list = tickInputs[2]
			if not list then
				list = {}
				tickInputs[2] = list
			end

			if #list < GPlayerInput.InputListMaxSize then
				list[#list + 1] = {
					playerInputID,
					playerInputArg,
				}
			elseif log.canWarn() then
				log.warn("Input list exceeded in one tick!")
			end
		end

		if not playerTicksInputs.beginTick then
			playerTicksInputs.beginTick = worldTick
		end

		if playerTicksInputs.endTick < worldTick then
			playerTicksInputs.endTick = worldTick
		end

		return true
	end

	--- @param worldTick dr2c.WorldTick
	--- @return boolean
	local function checkArchived(worldTick)
		for _, playerTicksInputs in pairs(playersTicksInputsData) do
			local beginTick = math_max(playersTicksInputsArchivedTick, playerTicksInputs.beginTick or -math_huge)
			local endTick = math_max(worldTick, playerTicksInputs.endTick or -math_huge)
			assert(beginTick <= endTick)

			if endTick - beginTick > 1e5 and log.canWarn() then
				log.warn("Tick span is too large, is there a logic error and causing the infinite loop?")
			end

			for tick = beginTick, endTick do
				if not playerTicksInputs[tick] then
					return false
				end
			end
		end

		return true
	end

	--- 设置某一玩家在某一世界刻的所有输入，并记录归档帧
	--- @param playerID dr2c.PlayerID
	--- @param worldTick dr2c.WorldTick
	--- @param tickInputs dr2c.PlayerTickInputs
	--- @return boolean? archived @设置了玩家的输入后，返回值表示该刻是否成为了归档刻，若该帧已归档则返回`nil`
	function PlayerInputBuffers.setInputs(playerID, worldTick, tickInputs)
		-- if worldTick <= playersTicksInputsArchivedTick then
		-- 	return
		-- end

		local playerTicksInputs = playersTicksInputsData[playerID]
		if not playerTicksInputs then
			throwPlayerNotFound(playerID)
		end

		checkWorldTickValidation(worldTick)

		playerTicksInputs[worldTick] = tickInputs

		playerTicksInputs.beginTick = math_min(playerTicksInputs.beginTick or math_huge, worldTick)
		playerTicksInputs.endTick = math_max(playerTicksInputs.beginTick or -math_huge, worldTick)

		-- if checkArchived(worldTick) then
		-- 	playersTicksInputsArchivedTick = worldTick

		-- 	return true
		-- else
		-- 	return false
		-- end
	end

	--- 移除某一玩家在某一世界刻的所有输入
	--- @param playerID dr2c.PlayerID
	--- @param worldTick dr2c.WorldTick
	--- @return boolean
	function PlayerInputBuffers.removeInputs(playerID, worldTick)
		local playerTicksInputs = playersTicksInputsData[playerID]
		if not playerTicksInputs then
			return false
		end

		checkWorldTickValidation(worldTick)

		playerTicksInputs[worldTick] = nil

		if worldTick == playerTicksInputs.beginTick then
			playerTicksInputs.beginTick = nil

			for beginTick = playerTicksInputs.beginTick, playerTicksInputs.endTick or -math_huge do
				if playerTicksInputs[beginTick] then
					playerTicksInputs.beginTick = beginTick
					break
				end
			end
		end

		if worldTick == playerTicksInputs.endTick then
			playerTicksInputs.beginTick = nil

			for endTick = playerTicksInputs.endTick, playerTicksInputs.beginTick or math_huge, -1 do
				if playerTicksInputs[endTick] then
					playerTicksInputs.endTick = endTick
					break
				end
			end
		end

		return true
	end

	--- 尝试丢弃所有玩家在目标刻及目标刻前的所有输入
	--- @param targetTick number
	function PlayerInputBuffers.discardInputs(targetTick)
		--- @param playerID dr2c.PlayerID
		--- @param playerTicksInputs dr2c.PlayerTicksInputs
		for playerID, playerTicksInputs in pairs(playersTicksInputsData) do
			if targetTick >= playerTicksInputs.endTick then
				playersTicksInputsData[playerID] = { endTick = 0 }
			else
				for tick = playerTicksInputs.beginTick, targetTick do
					playerTicksInputs[tick] = nil
				end
				playerTicksInputs.beginTick = targetTick + 1
			end
		end
	end

	--- @param worldTick dr2c.WorldTick
	--- @return boolean
	function PlayerInputBuffers.isArchived(worldTick)
		return worldTick <= playersTicksInputsArchivedTick
	end

	return PlayerInputBuffers
end

return GPlayerInputBuffers
