--[[
-- @module dr2c.server.world.PlayerInputBuffers
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local String = require("tudov.String")

local GMessage = require("dr2c.shared.network.Message")
local GPlayerInputBuffers = require("dr2c.shared.world.PlayerInputBuffers")
local SServer = require("dr2c.server.network.Server")

--- @class dr2c.SPlayerInputBuffer : dr2c.PlayerInputBuffers
local SPlayerInputBuffer = GPlayerInputBuffers.new()

local latestArchivedTick = 0

--- @param e dr2c.E.SMessage
events:add(N_("SMessage"), function(e)
	if SPlayerInputBuffer.setInputs(e.content.playerID, e.content.worldTick, e.content.playerInputs) then
		latestArchivedTick = e.content.worldTick
	end

	SServer.broadcastReliable(e.type, e.content)
end, "ReceivePlayerInput", "Receive", GMessage.Type.PlayerInputs)

events:add(N_("SConnect"), function(e)
	local clientID = e.clientID

	SPlayerInputBuffer.addPlayer(clientID)
end, "AddPlayerToPlayerInputBuffers", "PlayerInputBuffer", GMessage.Type.ClientConnect)

return SPlayerInputBuffer
