local CWorldTime = require("dr2c.client.world.WorldTime")
local CWorldTick = require("dr2c.client.world.WorldTick")
local CInput = require("dr2c.client.system.Input")

--- @class dr2c.CPlayerInput
local CPlayerInput = {}

local previousWorldTick = 0

-- local function handleLocalPlayersMovement()
-- 	--
-- end

--- @param e dr2c.E.ClientUpdate
events:add(N_("CUpdate"), function(e)
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

	local currentWorldTick = CWorldTick.getCurrentTick()
	if previousWorldTick < currentWorldTick then
		previousWorldTick = currentWorldTick

		-- send
	end
end, "HandleLocalPlayersInputs", "Inputs")

return CPlayerInput
