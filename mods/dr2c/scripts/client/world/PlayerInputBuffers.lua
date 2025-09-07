--[[
-- @module dr2c.client.world.PlayerInputBuffers
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("tudov.Table")

local CClients = require("dr2c.client.network.Clients")
local CPlayers = require("dr2c.client.network.Players")
local CServer = require("dr2c.client.network.Server")

local GMessage = require("dr2c.shared.network.Message")
local GPlayerInput = require("dr2c.shared.world.PlayerInput")
local GPlayerInputBuffers = require("dr2c.shared.world.PlayerInputBuffers")

--- @class dr2c.CPlayerInputBuffers : dr2c.PlayerInputBuffers
local CPlayerInputBuffers = GPlayerInputBuffers.new()

--- @param e dr2c.E.CClientAdded
events:add(N_("CClientAdded"), function(e)
	CPlayerInputBuffers.addPlayer(e.clientID)
end, "AddPlayerToPlayerInputBuffers", "PlayerInputBuffer")

--- @param e dr2c.E.CMessage
events:add(N_("CMessage"), function(e)
	local playerID = e.content.playerID

	if CPlayerInputBuffers.hasPlayer(playerID) then
		local playerInputs = e.content.playerInputs
		local worldTick = e.content.worldTick

		CPlayerInputBuffers.setInputs(playerID, worldTick, playerInputs)
	end
end, "ReceivePlayerInput", "Receive", GMessage.Type.PlayerInputs)

events:add(N_("CConnect"), function(e)
	CPlayerInputBuffers.clear()
end, "ClearPlayerInputBuffers", "Initialize")

events:add(N_("CDisconnect"), CPlayerInputBuffers.clear, "ClearPlayerInputBuffers", "Reset")

return CPlayerInputBuffers
