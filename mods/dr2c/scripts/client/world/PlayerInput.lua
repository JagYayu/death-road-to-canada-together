local CWorldTime = require("dr2c.client.world.WorldTime")
local CWorldTick = require("dr2c.client.world.WorldTick")
local CInput = require("dr2c.client.system.Input")
local CPlayerInputBuffers = require("dr2c.client.world.PlayerInputBuffers")
local CPlayers = require("dr2c.client.network.Players")
local CClient = require("dr2c.client.network.Client")
local GMessage = require("dr2c.shared.network.Message")
local GPlayerInput = require("dr2c.shared.world.PlayerInput")

--- @class dr2c.CPlayerInput
local CPlayerInput = {}

local previousWorldTick = 0

-- local function handleLocalPlayersMovement()
-- 	--
-- end

--- @param playerID dr2c.PlayerID
--- @return any?
function CPlayerInput.getPlayerMoveArg(playerID)
	local dx = 0
	local dy = 0

	if CInput.isScanCodeHeld(EScanCode.A) then
		dx = dx - 1
	end
	if CInput.isScanCodeHeld(EScanCode.D) then
		dx = dx + 1
	end
	if CInput.isScanCodeHeld(EScanCode.W) then
		dy = dy - 1
	end
	if CInput.isScanCodeHeld(EScanCode.S) then
		dy = dy + 1
	end

	return math.atan2(dy, dx)
end

--- @param e dr2c.E.ClientUpdate
events:add(N_("CUpdate"), function(e)
	if previousWorldTick <= 0 then
		return
	end

	local clientID = CClient.getClientID()
	if not clientID then
		return
	end

	if log.canTrace() then
		log.trace("Adding players input")
	end

	local messageContent = {}

	for _, playerID in ipairs(CPlayers.getPlayers(clientID)) do
		local inputID = GPlayerInput.ID.Move
		local inputArg = CPlayerInput.getPlayerMoveArg(playerID)

		messageContent[#messageContent + 1] = {
			worldTick = previousWorldTick,
			playerID = playerID,
			playerInputID = inputID,
			playerInputArg = inputArg,
		}

		CPlayerInputBuffers.addInput(playerID, previousWorldTick, inputID, inputArg)
	end
end, "HandlePlayersContinuousInputs", "Inputs")

events:add(N_("CUpdate"), function(e)
	local clientID = CClient.getClientID()
	if not clientID then
		return
	end

	local currentWorldTick = CWorldTick.getCurrentTick()

	while previousWorldTick < currentWorldTick do
		local worldTick = previousWorldTick + 1
		previousWorldTick = worldTick

		print("Send players inputs")

		for _, playerID in ipairs(CPlayers.getPlayers(clientID)) do
			local messageContent = {
				worldTick = worldTick,
				input = CPlayerInputBuffers.getPlayerInputEntry(playerID, worldTick),
			}

			CClient.sendReliable(GMessage.Type.PlayerInputs, messageContent)
		end
	end
end, "SendPlayersInputs", "Inputs")

return CPlayerInput
