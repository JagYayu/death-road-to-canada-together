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

--- @class dr2c.GPlayerInputBuffers
local GPlayerInputBuffers = {}

--- @type table<string, dr2c.PlayerInputBuffers>
local modules = {}

modules = persist("modules", function()
	return modules
end)

--- @return dr2c.PlayerInputBuffers
function GPlayerInputBuffers.new()
	local GPlayerInput = require("dr2c.Shared.World.PlayerInput")
	local GWorldTick = require("dr2c.Shared.World.Tick")

	--- @class dr2c.PlayerTickInputs
	--- @field [1]? table<dr2c.PlayerInputID, Serializable> @A map of inputs, where store continuous inputs.
	--- @field [2]? { [1]: dr2c.PlayerInputID, [2]: Serializable }[] @A list of inputs.

	--- @class dr2c.PlayerTicksInputs
	--- @field [dr2c.WorldTick] dr2c.PlayerTickInputs
	--- @field beginTick? dr2c.WorldTick
	--- @field endTick? dr2c.WorldTick

	--- @class dr2c.PlayersTicksInputs
	--- @field [dr2c.PlayerID] dr2c.PlayerTicksInputs

	--- @class dr2c.PlayerInputCollectedEntry
	--- @field deterministic boolean
	--- @field map table<dr2c.PlayerInputID, Serializable>
	--- @field list { [1]: dr2c.PlayerInputID, [2]: Serializable }[]

	--- @class dr2c.PlayerInputBuffers
	local PlayerInputBuffers = {}

	--- @type dr2c.PlayersTicksInputs
	local playersTicksInputsData = {}
	--- @type dr2c.WorldTick
	local playersTicksInputsArchivedTick = 0

	local scriptName = TE.scriptLoader:getLoadingScriptName()
	if scriptName ~= "" then
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

	--- @return dr2c.WorldTick archivedTick
	--- @nodiscard
	function PlayerInputBuffers.getArchivedTick()
		return playersTicksInputsArchivedTick
	end

	function PlayerInputBuffers.reset()
		playersTicksInputsData = {}
		playersTicksInputsArchivedTick = 0
	end

	--- @param playerID dr2c.PlayerID
	--- @return boolean
	--- @nodiscard
	function PlayerInputBuffers.hasPlayer(playerID)
		return not not playersTicksInputsData[playerID]
	end

	--- @param playerID dr2c.PlayerID
	function PlayerInputBuffers.addPlayer(playerID)
		if playersTicksInputsData[playerID] then
			error(("Player %s already exists"):format(playerID), 2)
		end

		playersTicksInputsData[playerID] = {}
	end

	--- @param playerID dr2c.PlayerID
	function PlayerInputBuffers.removePlayer(playerID)
		if not playersTicksInputsData[playerID] then
			error(("Player %s does not exists"):format(playerID), 2)
		end

		playersTicksInputsData[playerID] = nil
	end

	local function checkWorldTick(worldTick)
		if type(worldTick) ~= "number" then
			error("'worldTick' number expected", 3)
		elseif worldTick <= 0 then
			error("'worldTick' should be greater than 0", 3)
		end
	end

	--- 获取玩家在某一世界刻的输入
	--- @param playerID dr2c.PlayerID
	--- @param worldTick dr2c.WorldTick
	--- @return dr2c.PlayerTickInputs? tickInputs
	function PlayerInputBuffers.getInputs(playerID, worldTick)
		local playerTicksInputs = playersTicksInputsData[playerID]
		if playerTicksInputs then
			return playerTicksInputs[worldTick]
		end
	end

	--- @param playerID dr2c.PlayerID
	--- @param worldTick dr2c.WorldTick
	--- @param playerInputID dr2c.PlayerInputID
	--- @param playerInputArg Serializable
	function PlayerInputBuffers.addInput(playerID, worldTick, playerInputID, playerInputArg)
		checkWorldTick(worldTick)

		local playerTickInputs = playersTicksInputsData[playerID]
		if not playerTickInputs then
			error(("Player %s not exist"):format(playerID), 2)
		end

		local playerInputs = playerTickInputs[worldTick]
		if not playerInputs then
			playerInputs = {}
			playerTickInputs[worldTick] = playerInputs
		end

		if GPlayerInput.isContinuous(playerInputID) then
			local map = playerInputs[1]
			if not map then
				map = {}
				playerInputs[1] = map
			end

			map[playerInputID] = playerInputArg
		else
			local list = playerInputs[2]
			if not list then
				list = {}
				playerInputs[2] = list
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

		if (playerTickInputs.endTick or 0) < worldTick then
			playerTickInputs.endTick = worldTick
		end
	end

	--- 设置某一玩家在某一世界刻的所有输入
	--- @param playerID dr2c.PlayerID
	--- @param worldTick dr2c.WorldTick
	--- @param playerInputs dr2c.PlayerTickInputs
	--- @return boolean archived
	function PlayerInputBuffers.setInputs(playerID, worldTick, playerInputs)
		checkWorldTick(worldTick)

		do
			local playerTickInputs = playersTicksInputsData[playerID]
			if not playerTickInputs then
				error(("Player %s does not exists"):format(playerID), 2)
			end

			playerTickInputs[worldTick] = playerInputs
		end

		local archived = true
		for _, playerTickInputs in pairs(playersTicksInputsData) do
			if type(playerTickInputs) == "table" and not playerTickInputs[worldTick] then
				archived = false
				break
			end
		end

		if archived then
			playersTicksInputsArchivedTick = worldTick

			return true
		else
			return false
		end
	end

	--- 移除某一玩家在某一世界刻的所有输入
	--- @param playerID dr2c.PlayerID
	--- @param worldTick dr2c.WorldTick
	function PlayerInputBuffers.removeInputs(playerID, worldTick)
		checkWorldTick(worldTick)

		local inputBuffer = playersTicksInputsData[playerID]
		if not inputBuffer then
			inputBuffer = {}
			playersTicksInputsData[playerID] = inputBuffer
		else
			inputBuffer[worldTick] = nil
		end
	end

	--- @param inputsLifetime number
	function PlayerInputBuffers.removeOldInputs(inputsLifetime)
		local ticks = playersTicksInputsArchivedTick - math.floor(inputsLifetime * GWorldTick.getTPS())

		print(ticks)

		--- @param playerID dr2c.PlayerID
		--- @param playerTicksInputs dr2c.PlayerTicksInputs
		for playerID, playerTicksInputs in pairs(playersTicksInputsData) do
			--
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
