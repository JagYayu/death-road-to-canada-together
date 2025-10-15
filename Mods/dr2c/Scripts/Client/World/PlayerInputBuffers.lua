--[[
-- @module dr2c.Client.World.PlayerInputBuffers
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("TE.Table")

local CNetworkClients = require("dr2c.Client.Network.Clients")
local CNetworkPlayers = require("dr2c.Client.Network.Players")
local CNetworkServer = require("dr2c.Client.Network.Server")

local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GPlayerInput = require("dr2c.Shared.World.PlayerInput")
local GPlayerInputBuffers = require("dr2c.Shared.World.PlayerInputBuffers")

--- @class dr2c.CPlayerInputBuffers : dr2c.PlayerInputBuffers
local CPlayerInputBuffers = GPlayerInputBuffers.new()

--- @param e dr2c.E.CClientAdded
TE.events:add(N_("CClientAdded"), function(e)
	CPlayerInputBuffers.addPlayer(e.clientID)
end, "AddPlayerToPlayerInputBuffers", "PlayerInputBuffer")

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	local playerID = e.content.playerID

	if CPlayerInputBuffers.hasPlayer(playerID) then
		local playerInputs = e.content.playerInputs
		local worldTick = e.content.worldTick

		CPlayerInputBuffers.setInputs(playerID, worldTick, playerInputs)
	end
end, "ReceivePlayerInput", "Receive", GNetworkMessage.Type.PlayerInputs)

TE.events:add(N_("CConnect"), function(e)
	CPlayerInputBuffers.clear()
end, "ClearPlayerInputBuffers", "Initialize")

TE.events:add(N_("CDisconnect"), CPlayerInputBuffers.clear, "ClearPlayerInputBuffers", "Reset")

return CPlayerInputBuffers
