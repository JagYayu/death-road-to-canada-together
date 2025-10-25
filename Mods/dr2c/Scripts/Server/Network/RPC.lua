--[[
-- @module dr2c.Server.Network.RPC
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local GNetworkRPC = require("dr2c.Shared.Network.RPC")
local SNetworkServer = require("dr2c.Server.Network.Server")

--- @class dr2c.SNetworkRPC : dr2c.MNetworkRPC
local SNetworkRPC = GNetworkRPC.new()

--- @param sendFunction function
--- @param clientID TE.Network.ClientID
--- @param messageType dr2c.MessageType
--- @param messageContent? any
--- @param messageChannel? dr2c.GNetworkMessage.Channel
--- @param callback dr2c.NetworkRemoteRequestCallback
--- @param timeout? number
--- @return dr2c.NetworkRemoteRequestHandle? requestHandle
local function sendRequestImpl(
	sendFunction,
	clientID,
	messageType,
	messageContent,
	messageChannel,
	callback,
	timeout,
	...
)
	local requestHandle = SNetworkRPC.addRequest(messageType, callback, timeout, ...)
	messageContent[GNetworkMessageFields.requestHandle] = requestHandle

	if sendFunction(messageType, messageContent, messageChannel) then
		return requestHandle
	else
		SNetworkRPC.removeRequest(requestHandle)
	end
end

--- @param clientID TE.Network.ClientID
--- @param messageType dr2c.MessageType
--- @param messageContent? any
--- @param messageChannel? dr2c.GNetworkMessage.Channel
--- @param callback dr2c.NetworkRemoteRequestCallback
--- @param timeout? number
--- @param ... unknown
--- @return integer? requestHandle
function SNetworkRPC.sendReliable(clientID, messageType, messageContent, messageChannel, callback, timeout, ...)
	if type(messageContent) ~= "table" then
		error("Message content must be a table", 2)
	elseif messageContent[GNetworkMessageFields.requestHandle] ~= nil then
		error("Message content's field `GNetworkMessageFields.requestHandle` must be a nil", 2)
	elseif GNetworkRPC.hasClientResponse(messageType) then
		error(("Client response is unregistered for message type %s"):format(messageType), 2)
	end

	return sendRequestImpl(
		SNetworkServer.sendReliable,
		clientID,
		messageType,
		messageContent,
		messageChannel,
		callback,
		timeout,
		...
	)
end

--- @param clientID TE.Network.ClientID
--- @param messageType dr2c.MessageType
--- @param messageContent? any
--- @param messageChannel? dr2c.GNetworkMessage.Channel
--- @param callback dr2c.NetworkRemoteRequestCallback
--- @param timeout? number
--- @param ... unknown
--- @return integer? requestHandle
function SNetworkRPC.sendUnreliable(clientID, messageType, messageContent, messageChannel, callback, timeout, ...)
	if type(messageContent) ~= "table" then
		error("Message content must be a table", 2)
	elseif messageContent[GNetworkMessageFields.requestHandle] ~= nil then
		error("Message content's field `GNetworkMessageFields.requestHandle` must be a nil", 2)
	elseif GNetworkRPC.hasClientResponse(messageType) then
		error(("Client response is unregistered for message type %s"):format(messageType), 2)
	end

	return sendRequestImpl(
		SNetworkServer.sendUnreliable,
		clientID,
		messageType,
		messageContent,
		messageChannel,
		callback,
		timeout,
		...
	)
end

--- @param messageType dr2c.MessageType
function SNetworkRPC.registerResponse(messageType)
	--- @param e dr2c.E.SMessage
	TE.events:add(N_("SMessage"), function(e)
		-- e.
	end, "Response" .. messageType, "Response", messageType)
end

TE.events:add(N_("SUpdate"), SNetworkRPC.cleanupExpiredRequests, "ClearExpiredMessageRequests", "Cleanup")

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	--- @class dr2c.E.SMessage
	--- @field suppressRequest boolean?

	if e.suppressRequest then
		return
	end

	if type(e.content) ~= "table" then
		return
	end

	SNetworkRPC.receiveMessage(e.type, e.content)
end, "HandleMessageRequests", "Request")

return SNetworkRPC
