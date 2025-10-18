--[[
-- @module dr2c.Client.network.Players
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")
local Table = require("TE.Table")

local GPlayers = require("dr2c.Shared.Network.Players")

--- TODO
--- not implement yet, currently assume every client has a playerID which equals to clientID
--- @class dr2c.CNetworkPlayers
local CNetworkPlayers = {}

--- @type table<dr2c.PlayerID, table<dr2c.PlayerAttribute, any>>
local clientPlayersAttributes = {}

--- @param clientID TE.Network.ClientID
--- @return dr2c.PlayerID[]
function CNetworkPlayers.getPlayers(clientID)
	if clientID ~= 0 then
		return { clientID }
	else
		return Table.empty
	end
end

--- @param clientID TE.Network.ClientID
--- @return dr2c.PlayerID?
function CNetworkPlayers.getFirstPlayer(clientID)
	return clientID
end

--- @param playerID dr2c.PlayerID
function CNetworkPlayers.getClientID(playerID)
	--
end

function CNetworkPlayers.isLocalPlayer(playerID)
	--
end

function CNetworkPlayers.listPlayers()
	--
end

return CNetworkPlayers
