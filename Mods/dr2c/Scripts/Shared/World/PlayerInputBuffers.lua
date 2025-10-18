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

local GPlayerInput = require("dr2c.Shared.World.PlayerInput")

--- @class dr2c.GPlayerInputBuffers
local GPlayerInputBuffers = {}

--- @type table<table, dr2c.PlayerInputBuffers>
local playerInputBuffersModules = setmetatable({}, {
	__mode = "k",
})

playerInputBuffersModules = persist("playerInputBuffersModules", function()
	return playerInputBuffersModules
end)

--- @return dr2c.PlayerInputBuffers
function GPlayerInputBuffers.new()
	local scriptName = TE.scriptLoader:getLoadingScriptName()
	if scriptName ~= "" and playerInputBuffersModules[scriptName] then
		return playerInputBuffersModules[scriptName]
	end

	--- @class dr2c.PlayerInputBuffers
	local PlayerInputBuffers = {}

	--- @class dr2c.PlayerInputs
	--- @field [1]? table<dr2c.PlayerInputID, Serializable> @A map of inputs, where store continuous inputs.
	--- @field [2]? { [1]: dr2c.PlayerInputID, [2]: Serializable }[] @A list of inputs, where store continuous inputs.

	--- @class dr2c.PlayerTickInputs
	--- @field [dr2c.WorldTick] dr2c.PlayerInputs
	--- @field beginTick? dr2c.WorldTick
	--- @field endTick? dr2c.WorldTick

	--- @class dr2c.PlayerInputCollectedEntry
	--- @field deterministic boolean
	--- @field map table<dr2c.PlayerInputID, Serializable>
	--- @field list { [1]: dr2c.PlayerInputID, [2]: Serializable }[]

	playerInputBuffersModules[scriptName] = PlayerInputBuffers

	--- @class dr2c.PlayersTickInputsData
	local playersTicksInputsData = {}
	--- @type dr2c.WorldTick
	local playersTicksInputsAchievedTick = 0

	--- @warn Do not use this function unless you know what you're doing.
	--- @return dr2c.PlayersTickInputsData data
	function PlayerInputBuffers.getData()
		return playersTicksInputsData
	end

	--- @return dr2c.WorldTick achievedTick
	function PlayerInputBuffers.getAchievedTick()
		return playersTicksInputsAchievedTick
	end

	function PlayerInputBuffers.reset()
		playersTicksInputsData = {}
		playersTicksInputsAchievedTick = 0
	end

	--- @param playerID dr2c.PlayerID
	--- @param worldTick dr2c.WorldTick
	--- @return dr2c.PlayerInputs?
	function PlayerInputBuffers.getPlayerInputs(playerID, worldTick)
		local playerTickInputsBuffer = playersTicksInputsData[playerID]
		if playerTickInputsBuffer then
			return playerTickInputsBuffer[worldTick]
		end
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

	--- @param playerID dr2c.PlayerID
	--- @return boolean
	function PlayerInputBuffers.hasPlayer(playerID)
		return not not playersTicksInputsData[playerID]
	end

	local function checkWorldTick(worldTick)
		if type(worldTick) ~= "number" then
			error("'worldTick' number expected", 3)
		elseif worldTick <= 0 then
			error("'worldTick' should be greater than 0", 3)
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
			local set = playerInputs[1]
			if not set then
				set = {}
				playerInputs[1] = set
			end

			set[playerInputID] = playerInputArg
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

	--- @param playerID dr2c.PlayerID
	--- @param worldTick dr2c.WorldTick
	--- @param playerInputs dr2c.PlayerInputs
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
			playersTicksInputsData.archivedTick = worldTick

			return true
		else
			return false
		end
	end

	--- @param playerID dr2c.PlayerID
	--- @param worldTick dr2c.WorldTick
	function PlayerInputBuffers.removeInput(playerID, worldTick)
		checkWorldTick(worldTick)

		local inputBuffer = playersTicksInputsData[playerID]
		if not inputBuffer then
			inputBuffer = {}
			playersTicksInputsData[playerID] = inputBuffer
		else
			inputBuffer[worldTick] = nil
		end
	end

	--- @param worldTick dr2c.WorldTick
	--- @return boolean
	function PlayerInputBuffers.isArchived(worldTick)
		return worldTick <= playersTicksInputsData.archivedTick
	end

	--- @param ticksInputsData dr2c.PlayerTickInputs
	--- @param beginWorldTick dr2c.WorldTick
	--- @param endWorldTick dr2c.WorldTick
	--- @return table<dr2c.WorldTick, dr2c.PlayerInputCollectedEntry>
	local function collectInputsInRangeImpl(ticksInputsData, beginWorldTick, endWorldTick)
		local inputs = Table.new(endWorldTick - beginWorldTick + 1, 0)

		--- @type dr2c.PlayerInputs
		local prevInputsData = {}

		for worldTick = beginWorldTick, endWorldTick do
			local inputsData = ticksInputsData[worldTick]
			if inputsData then
				prevInputsData = inputsData

				inputs[worldTick] = {
					deterministic = true,
					map = inputsData[1] or Table.empty,
					list = inputsData[2] or Table.empty,
				}
			else -- Predict continuous inputs
				inputsData = {}
				local prevContinuousSet = prevInputsData[GPlayerInput.Type.Continuous]

				if prevContinuousSet then
					local set = {}
					inputsData[GPlayerInput.Type.Continuous] = set

					for key, value in pairs(prevContinuousSet) do
						set[key] = value
					end
				end

				inputs[worldTick] = {
					deterministic = false,
					map = inputsData[1] or Table.empty,
					list = inputsData[2] or Table.empty,
				}
			end
		end

		return inputs
	end

	--- 收集一个玩家在两个时刻范围内的输入
	--- @param playerID dr2c.PlayerID
	--- @param beginWorldTick dr2c.WorldTick
	--- @param endWorldTick dr2c.WorldTick
	--- @return table<dr2c.WorldTick, dr2c.PlayerInputCollectedEntry>
	--- @nodiscard
	function PlayerInputBuffers.collectPlayerInputsInRange(playerID, beginWorldTick, endWorldTick)
		local ticksInputsData = playersTicksInputsData[playerID]
		if ticksInputsData then
			return Table.empty
		end

		return collectInputsInRangeImpl(ticksInputsData, beginWorldTick, endWorldTick)
	end

	--- 收集所有玩家在两个时刻范围内的输入
	--- @param beginWorldTick dr2c.WorldTick
	--- @param endWorldTick dr2c.WorldTick
	--- @return table<dr2c.PlayerID, table<dr2c.WorldTick, dr2c.PlayerInputCollectedEntry>>
	--- @nodiscard
	function PlayerInputBuffers.collectPlayersInputsInRange(beginWorldTick, endWorldTick)
		local playersInputs = {}

		for playerID, playerTickInputs in pairs(playersTicksInputsData) do
			if type(playerTickInputs) == "table" then
				playersInputs[playerID] = collectInputsInRangeImpl(playerTickInputs, beginWorldTick, endWorldTick)
			end
		end

		return playersInputs
	end

	return PlayerInputBuffers
end

return GPlayerInputBuffers
