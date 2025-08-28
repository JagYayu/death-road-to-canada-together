local Table = require("tudov.Table")

local GPlayerInput = require("dr2c.shared.world.PlayerInput")

--- @class dr2c.PlayerInputEntry
--- @field [1] boolean isPrediction
--- @field [2] dr2c.PlayerInput value

--- @class dr2c.CPlayerInputBuffer
local CPlayerInputBuffers = {}

--- @class dr2c.PlayerInputBufferEntry
--- @field [dr2c.PlayerInputType] { [1]: dr2c.PlayerInputID, [2]: Serializable }[] | table<dr2c.PlayerInputID, Serializable>

--- @class dr2c.PlayerInputBuffer
--- @field [integer] dr2c.PlayerInputBufferEntry
--- @field beginTick integer?
--- @field endTick integer?

--- @type table<dr2c.PlayerID, dr2c.PlayerInputBuffer>
local playerInputBuffers = {}

playerInputBuffers = persist("playerInputBuffers", function()
	return playerInputBuffers
end)

--- @warn Do not use this function unless you know what you're doing.
function CPlayerInputBuffers.getRawTable()
	return playerInputBuffers
end

function CPlayerInputBuffers.hasPlayer(playerID)
	return not not playerInputBuffers[playerID]
end

--- @param playerID dr2c.PlayerID
--- @param worldTickID integer
--- @param playerInputID dr2c.PlayerInputID
--- @param playerInputArg Serializable
function CPlayerInputBuffers.addInput(playerID, worldTickID, playerInputID, playerInputArg)
	if type(worldTickID) ~= "number" then
		error("'worldTickID' number expected", 2)
	elseif worldTickID <= 0 then
		error("'worldTickID' should be greater than 0", 2)
	end

	local inputBuffer = playerInputBuffers[playerID]
	if not inputBuffer then
		inputBuffer = {}
		playerInputBuffers[playerID] = inputBuffer
	end

	local tickInput = inputBuffer[worldTickID]
	if not tickInput then
		tickInput = {}
		inputBuffer[worldTickID] = tickInput
	end

	local playerInputType = GPlayerInput.getType(playerInputID)
	tickInput[playerInputType] = tickInput[playerInputType] or {}

	if playerInputType == GPlayerInput.Type.Discrete then
		local queue = tickInput[playerInputType]
		if #queue < GPlayerInput.InputQueueMaxSize then
			queue[#queue + 1] = {
				playerInputID,
				playerInputArg,
			}
		elseif log.canWarn() then
			log.warn("Input queue exceeded in one tick!")
		end
	elseif playerInputType == GPlayerInput.Type.Continuous or playerInputType == GPlayerInput.Type.Intent then
		local set = tickInput[playerInputType]
		set[playerInputID] = playerInputArg
	else
		error("Unknown player input type", 2)
	end

	if (inputBuffer.endTick or 0) < worldTickID then
		inputBuffer.endTick = worldTickID
	end
end

--- @param playerID dr2c.PlayerID
--- @param worldTickID integer
function CPlayerInputBuffers.removeInput(playerID, worldTickID) end

--- @param playerInputBuffer dr2c.PlayerInputBuffer
--- @param beginWorldTick integer
--- @param endWorldTick integer
--- @return table<integer, { deterministic: boolean, entry: dr2c.PlayerInputBufferEntry }>
local function collectInputsInRangeImpl(playerInputBuffer, beginWorldTick, endWorldTick)
	local inputs = Table.new(endWorldTick - beginWorldTick + 1, 0)

	--- @type dr2c.PlayerInputBufferEntry
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
--- @return table<integer, { deterministic: boolean, entry: dr2c.PlayerInputBufferEntry }>
function CPlayerInputBuffers.collectInputsInRange(playerID, beginWorldTick, endWorldTick)
	local inputBuffer = playerInputBuffers[playerID]
	if not inputBuffer then
		return {}
	end

	return collectInputsInRangeImpl(inputBuffer, beginWorldTick, endWorldTick)
end

--- @param beginWorldTick integer
--- @param endWorldTick integer
--- @return table<integer, { deterministic: boolean, entry: dr2c.PlayerInputBufferEntry }>
function CPlayerInputBuffers.collectPlayersInputsInRange(beginWorldTick, endWorldTick)
	local playersInputs = {}

	for playerID, inputBuffer in pairs(playerInputBuffers) do
		playersInputs[playerID] = collectInputsInRangeImpl(inputBuffer, beginWorldTick, endWorldTick)
	end

	return playersInputs
end

return CPlayerInputBuffers
