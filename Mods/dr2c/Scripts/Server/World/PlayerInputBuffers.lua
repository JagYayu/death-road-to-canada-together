--[[
-- @module dr2c.Server.World.PlayerInputBuffers
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local GWorldPlayerInputBuffers = require("dr2c.Shared.World.PlayerInputBuffers")
local GWorldSession = require("dr2c.Shared.World.Session")
local SNetworkServer = require("dr2c.Server.Network.Server")
local SWorldSession = require("dr2c.Server.World.Session")

--- @class dr2c.SPlayerInputBuffers : dr2c.PlayerInputBuffers
local SPlayerInputBuffers = GWorldPlayerInputBuffers.new()

TE.events:add(N_("SUpdate"), function(e)
	local inputsLifetime = SWorldSession.getAttribute(GWorldSession.Attribute.DataLifetime)
	if inputsLifetime then
		SPlayerInputBuffers.removeOldInputs(inputsLifetime)
	end
end, "RemoveOldInputsFromPlayerInputBuffers", "ClearCaches")

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	local fields = GNetworkMessageFields.PlayerInputs
	SPlayerInputBuffers.setInputs(
		e.content[fields.playerID],
		e.content[fields.worldTick],
		e.content[fields.playerInputs]
	)

	SNetworkServer.broadcastReliable(e.type, e.content)
end, "ReceivePlayerInput", "Receive", GNetworkMessage.Type.PlayerInputs)

TE.events:add(N_("SConnect"), function(e)
	local clientID = e.clientID

	SPlayerInputBuffers.addPlayer(clientID)
end, "AddPlayerToPlayerInputBuffers", "PlayerInputBuffer", GNetworkMessage.Type.ClientConnect)

return SPlayerInputBuffers
