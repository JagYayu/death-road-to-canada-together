--[[
-- @module dr2c.client.network.Players
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
--- @class dr2c.CPlayers
local CPlayers = {}

--- @type table<dr2c.PlayerID, table<dr2c.PlayerAttribute, any>>
local clientPlayersAttributes = {}

--- @param clientID Network.ClientID
--- @return dr2c.PlayerID[]
function CPlayers.getPlayers(clientID)
	if clientID ~= 0 then
		return { clientID }
	else
		return Table.empty
	end
end

--- @param clientID Network.ClientID
--- @return dr2c.PlayerID?
function CPlayers.getFirstPlayer(clientID)
	return clientID
end

--- @param playerID dr2c.PlayerID
function CPlayers.getClientID(playerID)
	--
end

function CPlayers.isLocalPlayer(playerID)
	--
end

function CPlayers.listPlayers()
	--
end

return CPlayers
