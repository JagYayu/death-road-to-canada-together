--[[
-- @module dr2c.Shared.world.PlayerInputBuffers
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

--- @class dr2c.PlayerTickInputsData
--- @field archivedTick dr2c.WorldTick
--- @field [dr2c.PlayerID] dr2c.PlayerTickInputs

--- @class dr2c.PlayerInputCollectedEntry
--- @field [1] boolean isPrediction
--- @field [2] dr2c.PlayerInput value

--- @class dr2c.GPlayerInputBuffers
local GPlayerInputBuffers = {}

--- @type table<table, dr2c.PlayerTickInputsData>
local playersTickInputsDataModules = setmetatable({}, {
	__mode = "k",
})

playersTickInputsDataModules = persist("playerTickInputsMapCollection", function()
	return playersTickInputsDataModules
end)

--- @return dr2c.PlayerInputBuffers
function GPlayerInputBuffers.new()
	local scriptName = TE.scriptLoader:getLoadingScriptName()
	if scriptName ~= "" and playersTickInputsDataModules[scriptName] then
		return playersTickInputsDataModules[scriptName]
	end

	--- @class dr2c.PlayerInputBuffers
	local PlayerInputBuffers = {}

	playersTickInputsDataModules[scriptName] = PlayerInputBuffers

	--- @class dr2c.PlayersTickInputsData
	local playersTickInputsData = {
		archivedTick = 0,
	}

	--- @class dr2c.PlayerInputs
	--- @field [true] table<dr2c.PlayerInputID, Serializable>
	--- @field [false] { [1]: dr2c.PlayerInputID, [2]: Serializable }[]

	--- @class dr2c.PlayerTickInputs
	--- @field [integer] dr2c.PlayerInputs
	--- @field beginTick integer?
	--- @field endTick integer?

	--- @warn Do not use this function unless you know what you're doing.
	function PlayerInputBuffers.getRawTable()
		return playersTickInputsData
	end

	function PlayerInputBuffers.clear()
		playersTickInputsData = {
			archivedTick = 0,
		}
	end

	--- @param playerID dr2c.PlayerID
	--- @param worldTick dr2c.WorldTick
	--- @return dr2c.PlayerInputs?
	function PlayerInputBuffers.getPlayerInputs(playerID, worldTick)
		local playerTickInputsBuffer = playersTickInputsData[playerID]
		if playerTickInputsBuffer then
			return playerTickInputsBuffer[worldTick]
		end
	end

	--- @param playerID dr2c.PlayerID
	function PlayerInputBuffers.addPlayer(playerID)
		if playersTickInputsData[playerID] then
			error(("Player %s already exists"):format(playerID), 2)
		end

		playersTickInputsData[playerID] = {}
	end

	--- @param playerID dr2c.PlayerID
	function PlayerInputBuffers.removePlayer(playerID)
		if not playersTickInputsData[playerID] then
			error(("Player %s does not exists"):format(playerID), 2)
		end

		playersTickInputsData[playerID] = nil
	end

	--- @param playerID dr2c.PlayerID
	--- @return boolean
	function PlayerInputBuffers.hasPlayer(playerID)
		return not not playersTickInputsData[playerID]
	end

	local function checkWorldTick(worldTick)
		if type(worldTick) ~= "number" then
			error("'worldTick' number expected", 2)
		elseif worldTick <= 0 then
			error("'worldTick' should be greater than 0", 2)
		end
	end

	--- @param playerID dr2c.PlayerID
	--- @param worldTick integer
	--- @param playerInputID dr2c.PlayerInputID
	--- @param playerInputArg Serializable
	function PlayerInputBuffers.addInput(playerID, worldTick, playerInputID, playerInputArg)
		checkWorldTick(worldTick)

		local playerTickInputs = playersTickInputsData[playerID]
		if not playerTickInputs then
			error(("Player %s not exist"):format(playerID), 2)
		end

		local playerInputs = playerTickInputs[worldTick]
		if not playerInputs then
			playerInputs = {}
			playerTickInputs[worldTick] = playerInputs
		end

		if GPlayerInput.isContinuous(playerInputID) then
			local set = playerInputs[true]
			if not set then
				set = {}
				playerInputs[true] = set
			end

			set[playerInputID] = playerInputArg
		else
			local list = playerInputs[false]
			if not list then
				list = {}
				playerInputs[false] = list
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
			local playerTickInputs = playersTickInputsData[playerID]
			if not playerTickInputs then
				error(("Player %s does not exists"):format(playerID), 2)
			end

			playerTickInputs[worldTick] = playerInputs
		end

		local archived = true
		for _, playerTickInputs in pairs(playersTickInputsData) do
			if type(playerTickInputs) == "table" and not playerTickInputs[worldTick] then
				archived = false
				break
			end
		end

		if archived then
			playersTickInputsData.archivedTick = worldTick

			return true
		else
			return false
		end
	end

	--- @param playerID dr2c.PlayerID
	--- @param worldTick integer
	function PlayerInputBuffers.removeInput(playerID, worldTick)
		checkWorldTick(worldTick)

		local inputBuffer = playersTickInputsData[playerID]
		if not inputBuffer then
			inputBuffer = {}
			playersTickInputsData[playerID] = inputBuffer
		else
			inputBuffer[worldTick] = nil
		end
	end

	function PlayerInputBuffers.isArchived(worldTick)
		return worldTick <= playersTickInputsData.archivedTick
	end

	--- @param playerInputBuffer dr2c.PlayerTickInputs
	--- @param beginWorldTick integer
	--- @param endWorldTick integer
	--- @return table<integer, { deterministic: boolean, entry: dr2c.PlayerInputs }>
	local function collectInputsInRangeImpl(playerInputBuffer, beginWorldTick, endWorldTick)
		local inputs = Table.new(endWorldTick - beginWorldTick + 1, 0)

		--- @type dr2c.PlayerInputs
		local prevEntry = {}

		for worldTick = beginWorldTick, endWorldTick do
			local entry = playerInputBuffer[worldTick]
			if entry then
				prevEntry = entry

				inputs[worldTick] = {
					deterministic = true,
					entry = entry,
				}
			else -- Predict continuous inputs
				entry = {}
				local prevContinuousSet = prevEntry[GPlayerInput.Type.Continuous]

				if prevContinuousSet then
					local set = {}
					entry[GPlayerInput.Type.Continuous] = set

					for key, value in pairs(prevContinuousSet) do
						set[key] = value
					end
				end

				inputs[worldTick] = {
					deterministic = false,
					entry = entry,
				}
			end
		end

		return inputs
	end

	--- @param playerID dr2c.PlayerID
	--- @param beginWorldTick integer
	--- @param endWorldTick integer
	--- @return table<integer, { deterministic: boolean, entry: dr2c.PlayerInputs }>
	function PlayerInputBuffers.collectInputsInRange(playerID, beginWorldTick, endWorldTick)
		local inputBuffer = playersTickInputsData[playerID]
		if not inputBuffer then
			return Table.empty
		end

		return collectInputsInRangeImpl(inputBuffer, beginWorldTick, endWorldTick)
	end

	--- @param beginWorldTick integer
	--- @param endWorldTick integer
	--- @return table<dr2c.PlayerID, table<integer, { deterministic: boolean, entry: dr2c.PlayerInputCollectedEntry }>>
	function PlayerInputBuffers.collectPlayersInputsInRange(beginWorldTick, endWorldTick)
		local playersInputs = {}

		for playerID, playerTickInputs in pairs(playersTickInputsData) do
			if type(playerTickInputs) == "table" then
				playersInputs[playerID] = collectInputsInRangeImpl(playerTickInputs, beginWorldTick, endWorldTick)
			end
		end

		return playersInputs
	end

	return PlayerInputBuffers
end

return GPlayerInputBuffers
