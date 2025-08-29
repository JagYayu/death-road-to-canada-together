local CWorldTime = require("dr2c.client.world.WorldTime")
local CWorldTick = require("dr2c.client.world.WorldTick")
local CInput = require("dr2c.client.system.Input")
local CPlayerInputBuffers = require("dr2c.client.world.PlayerInputBuffers")
local CClient = require("dr2c.client.network.Client")
local GPlayerInput = require("dr2c.shared.world.PlayerInput")

--- @class dr2c.CPlayerInput
local CPlayerInput = {}

local previousWorldTick = 0

-- local function handleLocalPlayersMovement()
-- 	--
-- end

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
	local currentWorldTick = CWorldTick.getCurrentTick()
	if previousWorldTick < currentWorldTick then
		previousWorldTick = currentWorldTick

		local playerID = CClient.getClientID()

		if log.canTrace() then
			log.trace("Add input")
		end
		CPlayerInputBuffers.addInput(
			playerID,
			previousWorldTick,
			GPlayerInput.ID.Move,
			CPlayerInput.getPlayerMoveArg(playerID)
		)
		-- send
	end
end, "HandleLocalPlayersInputs", "Inputs")

return CPlayerInput
