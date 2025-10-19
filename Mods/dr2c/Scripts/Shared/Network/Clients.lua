--[[
-- @module dr2c.Shared.Network.Clients
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local EnumFlag = require("TE.EnumFlag")

local GNetworkClient = require("dr2c.Shared.Network.Client")
local GNetworkDisconnection = require("dr2c.Shared.Network.Disconnection")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local SNetworkServer = require("dr2c.Server.Network.Server")

--- 只提供一些工具函数给`dr2c.CNetworkClients`, `dr2c.SNetworkClients`使用
--- @class dr2c.GNetworkClients
local GNetworkClients = {}

--- @param clientList TE.Network.ClientID[]
--- @param clientsPublicAttributes table<TE.Network.ClientID, table<dr2c.NetworkClientPublicAttribute, any>>
--- @return TE.Network.ClientID[]
function GNetworkClients.getAuthoritativeClients(clientList, clientsPublicAttributes)
	local authoritativeClients = {}

	for _, clientID in ipairs(clientList) do
		local permissions = clientsPublicAttributes[clientID][GNetworkClient.PublicAttribute.Permissions]
		if permissions and EnumFlag.hasAll(permissions, GNetworkClient.Permission.Authority) then
			authoritativeClients[#authoritativeClients + 1] = clientID
		end
	end

	return authoritativeClients
end

return GNetworkClients
