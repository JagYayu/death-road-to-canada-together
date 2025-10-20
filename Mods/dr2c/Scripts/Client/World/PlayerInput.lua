--[[
-- @module dr2c.Client.World.PlayerInput
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Math = require("TE.Math")

local CSystemInput = require("dr2c.Client.System.Input")
local CNetworkPlayers = require("dr2c.Client.Network.Players")
local CNetworkClient = require("dr2c.Client.Network.Client")
local CWorldTick = require("dr2c.Client.World.Tick")
local CWorldPlayerInputBuffers = require("dr2c.Client.World.PlayerInputBuffers")
local CWorldSession = require("dr2c.Client.World.Session")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local GWorldPlayerInput = require("dr2c.Shared.World.PlayerInput")
local GWorldTick = require("dr2c.Shared.World.Tick")

--- @class dr2c.CPlayerInput
local CPlayerInput = {}

local previousInputTick = 0

previousInputTick = persist("previousInputTick", function()
	return previousInputTick
end)

-- local function handleLocalPlayersMovement()
-- 	--
-- end

--- @param playerID dr2c.PlayerID
--- @return any?
function CPlayerInput.getPlayerMoveArg(playerID)
	local dx = 0
	local dy = 0

	if CSystemInput.isScanCodeHeld(EScanCode.A) then
		dx = dx - 1
	end
	if CSystemInput.isScanCodeHeld(EScanCode.D) then
		dx = dx + 1
	end
	if CSystemInput.isScanCodeHeld(EScanCode.W) then
		dy = dy - 1
	end
	if CSystemInput.isScanCodeHeld(EScanCode.S) then
		dy = dy + 1
	end

	if dx > 0 then
		if dy > 0 then
			return 2
		elseif dy < 0 then
			return 8
		else
			return 1
		end
	elseif dx < 0 then
		if dy > 0 then
			return 4
		elseif dy < 0 then
			return 6
		else
			return 5
		end
	else
		if dy > 0 then
			return 3
		elseif dy < 0 then
			return 7
		else
			return 0
		end
	end
	-- return (dx ~= 0 or dy ~= 0) and math.atan2(dy, dx) or nil
end

--- @param e dr2c.E.CUpdate
TE.events:add(N_("CUpdate"), function(e)
	local nextWorldTick = CWorldTick.getLatestTick() + 1
	if nextWorldTick <= 0 or CWorldSession.isInactive() then
		return
	end

	local clientID = CNetworkClient.getClientID()
	if not clientID then
		return
	end

	local messageContent = {}

	for _, playerID in ipairs(CNetworkPlayers.getPlayers(clientID)) do
		local inputID = GWorldPlayerInput.ID.Move
		local inputArg = CPlayerInput.getPlayerMoveArg(playerID)

		messageContent[#messageContent + 1] = {
			playerID = playerID,
			playerInputID = inputID,
			playerInputArg = inputArg,
			worldTick = nextWorldTick,
		}

		CWorldPlayerInputBuffers.addInput(playerID, nextWorldTick, inputID, inputArg)
	end
end, "HandlePlayersContinuousInputs", "Inputs")

TE.events:add(N_("CUpdate"), function(e)
	if CWorldSession.isInactive() then
		return
	end

	local clientID = CNetworkClient.getClientID()
	if not clientID then
		return
	end

	local currentWorldTick = CWorldTick.getLatestTick()

	if previousInputTick > currentWorldTick then
		previousInputTick = currentWorldTick
	elseif previousInputTick < currentWorldTick - GWorldTick.getTPS() then
		previousInputTick = currentWorldTick - GWorldTick.getTPS()
	end

	if currentWorldTick - previousInputTick > 1e5 and log.canWarn() then
		log.warn("Too many ticks pending to execute, might cause infinite loop?")
	end

	local canTrace = log.canTrace()

	while previousInputTick < currentWorldTick do
		local worldTick = previousInputTick + 1

		for _, playerID in ipairs(CNetworkPlayers.getPlayers(clientID)) do
			local fields = GNetworkMessageFields.PlayerInputs
			local messageContent = {
				[fields.worldTick] = worldTick,
				[fields.playerID] = playerID,
				[fields.playerInputs] = CWorldPlayerInputBuffers.getInputs(playerID, worldTick),
			}

			CNetworkClient.sendReliable(
				GNetworkMessage.Type.PlayerInputs,
				messageContent,
				nil,
				GNetworkMessageFields.PlayerInputs
			)

			if canTrace then
				log.trace(("Send player %d input"):format(playerID), messageContent)
			end
		end

		previousInputTick = worldTick
	end
end, "SendPlayersInputs", "Inputs")

--- @param e dr2c.E.CWorldSessionStart
TE.events:add(N_("CWorldSessionStart"), function(e) end)

return CPlayerInput
