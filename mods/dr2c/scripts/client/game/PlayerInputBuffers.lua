local Table = require("tudov.Table")

--- @class dr2c.PlayerInputEntry
--- @field [1] boolean isPrediction
--- @field [2] dr2c.PlayerInput value

--- @class dr2c.CPlayerInputBuffer
local CPlayerInputBuffers = {}

local playerInputBuffers = {}

function CPlayerInputBuffers.hasPlayer(playerID)
	return not not playerInputBuffers[playerID]
end

function CPlayerInputBuffers.addPlayer(playerID)
	playerInputBuffers[playerID] = {
		size = 0,
	}
end

function CPlayerInputBuffers.removePlayer(playerID)
	playerInputBuffers[playerID] = nil
end

function CPlayerInputBuffers.addInput(playerID, frameID, inputID)
	local inputs = playerInputBuffers[playerID]
	if not inputs then
		error("player does not exist", 2)
	end

	inputs[frameID] = { false, inputID }
	inputs.latestFrame = frameID
end

function CPlayerInputBuffers.getInputs(playerID, beginFrame, endFrame)
	local inputs = playerInputBuffers[playerID]
	if not inputs then
		error("player does not exist", 2)
	end

	beginFrame = tonumber(beginFrame) or 1
	endFrame = tonumber(endFrame) or inputs.latestFrame

	local result = Table.new(endFrame - beginFrame + 1, 0)
	for frame = beginFrame, endFrame do
		local entry = inputs[frame]
		if not entry then
			break
		end

		result[frame] = entry[2]
	end

	return result
end

return CPlayerInputBuffers
