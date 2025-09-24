--[[
-- @module dr2c.client.network.ClientAuthoritativeMessage
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local EnumFlag = require("tudov.EnumFlag")

local CNetworkClient = require("dr2c.client.network.Client")
local GNetworkClient = require("dr2c.shared.network.Client")

--- @class dr2c.CNetworkClientAuthoritativeMessage
local CNetworkClientAuthoritativeMessage = {}

local clientRegisteredAuthoritativeRequestsMap = {}

clientRegisteredAuthoritativeRequestsMap = persist("clientRegisteredAuthoritativeRequestsMap", function()
	return clientRegisteredAuthoritativeRequestsMap
end)

--- @param messageType dr2c.NetworkMessageType
--- @param response fun(requestID: integer, extras: any?): any?
function CNetworkClientAuthoritativeMessage.register(messageType, response)
	if clientRegisteredAuthoritativeRequestsMap[messageType] then
		error(("Message type %s has already been registered!"):format(messageType), 2)
	elseif type(response) ~= "function" then
		error(("Bad argument to #2 'response': function expected, got %s"):format(type(response)), 2)
	end

	--- @param e dr2c.E.CMessage
	TE.events:add(N_("CMessage"), function(e)
		local permissions = CNetworkClient.getPublicAttribute(GNetworkClient.PublicAttribute.Permissions)
		if not (permissions and EnumFlag.hasAll1(permissions, GNetworkClient.Permission.Authority)) then
			return
		end

		CNetworkClient.sendReliable(messageType, {
			requestID = e.content.requestID,
			response = response(e.content.args),
		})
	end, "ReceiveAuthoritativeMessage" .. messageType, "Receive", messageType)
end

return CNetworkClientAuthoritativeMessage
