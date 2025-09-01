local Table = require("tudov.Table")

local GPlayerInput = require("dr2c.shared.world.PlayerInput")

--- @class dr2c.PlayerInputCollectedEntry
--- @field [1] boolean isPrediction
--- @field [2] dr2c.PlayerInput value

--- @class dr2c.GPlayerInputBuffers
local GPlayerInputBuffers = {}

--- @type table<table, table<dr2c.PlayerID, dr2c.PlayerInputBuffer>>
local playerInputBuffersMap = setmetatable({}, {
	__mode = "k",
})

playerInputBuffersMap = persist("playerInputBuffers", function()
	return playerInputBuffersMap
end)

--- @return dr2c.PlayerInputBuffers
function GPlayerInputBuffers.new()
	--- @class dr2c.PlayerInputBuffers
	local PlayerInputBuffers = {}

	playerInputBuffersMap[PlayerInputBuffers] = {}
	local playerInputBuffers = playerInputBuffersMap[PlayerInputBuffers]

	--- @class dr2c.PlayerInputValue
	--- @field [true] table<dr2c.PlayerInputID, Serializable>
	--- @field [false] { [1]: dr2c.PlayerInputID, [2]: Serializable }[]

	--- @class dr2c.PlayerInputBuffer
	--- @field [integer] dr2c.PlayerInputValue
	--- @field beginTick integer?
	--- @field endTick integer?

	--- @param playerID any
	--- @param worldTick any
	--- @return dr2c.PlayerInputValue?
	function PlayerInputBuffers.getPlayerInputEntry(playerID, worldTick)
		local inputBuffer = playerInputBuffers[playerID]
		if inputBuffer then
			return inputBuffer[worldTick]
		end
	end

	--- @warn Do not use this function unless you know what you're doing.
	function PlayerInputBuffers.getRawTable()
		return playerInputBuffers
	end

	function PlayerInputBuffers.hasPlayer(playerID)
		return not not playerInputBuffers[playerID]
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

		local inputBuffer = playerInputBuffers[playerID]
		if not inputBuffer then
			inputBuffer = {}
			playerInputBuffers[playerID] = inputBuffer
		end

		local tickInput = inputBuffer[worldTick]
		if not tickInput then
			tickInput = {}
			inputBuffer[worldTick] = tickInput
		end

		if GPlayerInput.isContinuous(playerInputID) then
			tickInput[true] = tickInput[true] or {}
			tickInput[true][playerInputID] = playerInputArg
		else
			tickInput[false] = tickInput[false] or {}
			local queue = tickInput[false]

			if #queue < GPlayerInput.InputQueueMaxSize then
				queue[#queue + 1] = {
					playerInputID,
					playerInputArg,
				}
			elseif log.canWarn() then
				log.warn("Input queue exceeded in one tick!")
			end
		end

		if (inputBuffer.endTick or 0) < worldTick then
			inputBuffer.endTick = worldTick
		end
	end

	--- @param playerID dr2c.PlayerID
	--- @param worldTick integer
	--- @param inputEntry dr2c.PlayerInputValue
	function PlayerInputBuffers.setInput(playerID, worldTick, inputEntry)
		checkWorldTick(worldTick)

		local inputBuffer = playerInputBuffers[playerID]
		if not inputBuffer then
			inputBuffer = {}
			playerInputBuffers[playerID] = inputBuffer
		end

		local tickInput = inputBuffer[worldTick]
		if not tickInput then
			tickInput = {}
			inputBuffer[worldTick] = tickInput
		end

		Table.deepEquals(l, r)

		if GPlayerInput.isContinuous(playerInputID) then
			tickInput[true] = tickInput[true] or {}
			tickInput[true][playerInputID] = playerInputArg
		else
			tickInput[false] = tickInput[false] or {}
			local queue = tickInput[false]

			if #queue < GPlayerInput.InputQueueMaxSize then
				queue[#queue + 1] = {
					playerInputID,
					playerInputArg,
				}
			elseif log.canWarn() then
				log.warn("Input queue exceeded in one tick!")
			end
		end

		if (inputBuffer.endTick or 0) < worldTick then
			inputBuffer.endTick = worldTick
		end
	end

	--- @param playerID dr2c.PlayerID
	--- @param worldTick integer
	function PlayerInputBuffers.removeInput(playerID, worldTick)
		checkWorldTick(worldTick)

		local inputBuffer = playerInputBuffers[playerID]
		if not inputBuffer then
			inputBuffer = {}
			playerInputBuffers[playerID] = inputBuffer
		else
			inputBuffer[worldTick] = nil
		end
	end

	--- @param playerInputBuffer dr2c.PlayerInputBuffer
	--- @param beginWorldTick integer
	--- @param endWorldTick integer
	--- @return table<integer, { deterministic: boolean, entry: dr2c.PlayerInputValue }>
	local function collectInputsInRangeImpl(playerInputBuffer, beginWorldTick, endWorldTick)
		local inputs = Table.new(endWorldTick - beginWorldTick + 1, 0)

		--- @type dr2c.PlayerInputValue
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
	--- @return table<integer, { deterministic: boolean, entry: dr2c.PlayerInputValue }>
	function PlayerInputBuffers.collectInputsInRange(playerID, beginWorldTick, endWorldTick)
		local inputBuffer = playerInputBuffers[playerID]
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

		for playerID, inputBuffer in pairs(playerInputBuffers) do
			playersInputs[playerID] = collectInputsInRangeImpl(inputBuffer, beginWorldTick, endWorldTick)
		end

		return playersInputs
	end

	return PlayerInputBuffers
end

return GPlayerInputBuffers
