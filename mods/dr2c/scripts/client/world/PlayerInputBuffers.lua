local Table = require("tudov.Table")

local GMessage = require("dr2c.shared.network.Message")
local GPlayerInput = require("dr2c.shared.world.PlayerInput")
local GPlayerInputBuffers = require("dr2c.shared.world.PlayerInputBuffers")

--- @class dr2c.CPlayerInputBuffers : dr2c.PlayerInputBuffers
local CPlayerInputBuffers = GPlayerInputBuffers.new()

--- @param e dr2c.E.ClientMessage
events:add(N_("CMessage"), function(e)
	local playerID = e.content.playerID
	local inputID = e.content.playerInputID
	local inputArg = e.content.playerInputArg
	local worldTick = e.content.worldTick

	CPlayerInputBuffers.addInput(playerID, worldTick, inputID, inputArg)
end, "ReceivePlayerInput", "Receive", GMessage.Type.PlayerInput)

return CPlayerInputBuffers
