--[[
-- @module dr2c.Client.Network.RPC
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CNetworkClient = require("dr2c.Client.Network.Client")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local GNetworkRPC = require("dr2c.Shared.Network.RPC")

--- @class dr2c.CNetworkRPC : dr2c.MNetworkRPC
local CNetworkRPC = GNetworkRPC.new()

--- @param sendFunction function
--- @param messageType dr2c.MessageType
--- @param messageContent? any
--- @param messageChannel? dr2c.GNetworkMessage.Channel
--- @param callback dr2c.NetworkRemoteRequestCallback
--- @param timeout? number
--- @return integer? requestHandle
local function sendRequestImpl(sendFunction, messageType, messageContent, messageChannel, callback, timeout, ...)
	local requestHandle = CNetworkRPC.addRequest(messageType, callback, timeout, ...)
	messageContent[GNetworkMessageFields.requestHandle] = requestHandle

	if sendFunction(messageType, messageContent, messageChannel) then
		return requestHandle
	else
		CNetworkRPC.removeRequest(requestHandle)
	end
end

--- @param messageType dr2c.MessageType
--- @param messageContent? table
--- @param messageChannel? dr2c.GNetworkMessage.Channel
--- @param callback dr2c.NetworkRemoteRequestCallback
--- @param timeout? number
--- @return integer? requestHandle
function CNetworkRPC.sendReliable(messageType, messageContent, messageChannel, callback, timeout, ...)
	if messageContent == nil then
		messageContent = {}
	elseif type(messageContent) ~= "table" then
		error("Message content must be a table", 2)
	elseif messageContent[GNetworkMessageFields.requestHandle] ~= nil then
		error("Message content's field `GNetworkMessageFields.requestHandle` must be a nil", 2)
	end

	return sendRequestImpl(
		CNetworkClient.sendReliable,
		messageType,
		messageContent or {},
		messageChannel,
		callback,
		timeout,
		...
	)
end

--- @param messageType dr2c.MessageType
--- @param messageContent? any
--- @param messageChannel? dr2c.GNetworkMessage.Channel
--- @param callback dr2c.NetworkRemoteRequestCallback
--- @param timeout? number
--- @param ... unknown
--- @return integer? requestHandle
function CNetworkRPC.sendUnreliable(messageType, messageContent, messageChannel, callback, timeout, ...)
	if messageContent == nil then
		messageContent = {}
	elseif type(messageContent) ~= "table" then
		error("Message content must be a table", 2)
	elseif messageContent[GNetworkMessageFields.requestHandle] ~= nil then
		error("Message content's field `GNetworkMessageFields.requestHandle` must be a nil", 2)
	end

	return sendRequestImpl(
		CNetworkClient.sendUnreliable,
		messageType,
		messageContent,
		messageChannel,
		callback,
		timeout,
		...
	)
end

function CNetworkRPC.registerResponse()
	-- TODO
end

TE.events:add(N_("CUpdate"), CNetworkRPC.cleanupExpiredRequests, "ClearExpiredMessageRequests", "Cleanup")

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	--- @class dr2c.E.CMessage
	--- @field suppressRequest boolean?

	if e.suppressRequest then
		return
	end

	if type(e.content) ~= "table" then
		return
	end

	CNetworkRPC.receiveMessage(e.type, e.content)
end, "HandleMessageRequests", "Request")

return CNetworkRPC
