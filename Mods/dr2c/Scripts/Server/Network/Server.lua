--[[
-- @module dr2c.Server.Network.Server
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local GNetworkClient = require("dr2c.Shared.Network.Client")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local GNetworkServer = require("dr2c.Shared.Network.Server")

local network = TE.network

--- @class dr2c.ServerAttributes
--- @field [dr2c.ServerAttribute] Serializable

--- @class dr2c.SNetworkServer
local SNetworkServer = {}

local sessionSlot = 0

--- @type table<dr2c.ServerAttribute, Serializable>
local serverAttributes = {}

serverAttributes = persist("serverAttributes", function()
	return serverAttributes
end)

--- @return TE.Network.Server? server
function SNetworkServer.getNetworkSession()
	return network:getServer(sessionSlot)
end

--- @return table<dr2c.GNetworkServer.Attribute, Serializable> attributes
function SNetworkServer.getAttributes()
	return serverAttributes
end

--- @param serverAttribute dr2c.ServerAttribute
--- @return Serializable? value
function SNetworkServer.getAttribute(serverAttribute)
	return serverAttributes[serverAttribute]
end

--- @param serverAttribute dr2c.ServerAttribute
--- @param attributeValue Serializable?
function SNetworkServer.setAttribute(serverAttribute, attributeValue)
	serverAttributes[serverAttribute] = attributeValue
end

TE.events:add(N_("SUpdate"), function()
	if serverAttributes then
		return
	end

	local session = SNetworkServer.getNetworkSession()
	if not session then
		return
	end

	serverAttributes = {
		[GNetworkServer.Attribute.DisplayName] = session:getTitle(),
		[GNetworkServer.Attribute.Clients] = session:getClients(),
		[GNetworkServer.Attribute.MaxClients] = session:getMaxClients(),
		[GNetworkServer.Attribute.StartupTime] = Time.getStartupTime(),
		[GNetworkServer.Attribute.Version] = TE.engine:getVersion(),
		[GNetworkServer.Attribute.Mods] = {}, -- TODO get from mod manager
		[GNetworkServer.Attribute.HasPassword] = session:getPassword() ~= "",
		[GNetworkServer.Attribute.Rooms] = {},
		[GNetworkServer.Attribute.SocketType] = session:getSocketType(),
	}
end, "InitializeServer", "Network")

--- @param clientID TE.Network.ClientID
--- @param disconnectionCode dr2c.DisconnectionCode
--- @return boolean
function SNetworkServer.disconnect(clientID, disconnectionCode)
	local session = SNetworkServer.getNetworkSession()
	if not session then
		return false
	end

	session:disconnect(clientID, disconnectionCode)

	return true
end

--- @param clientID TE.Network.ClientID
--- @param messageType dr2c.MessageType
--- @param messageContent any?
--- @param messageChannel dr2c.NetworkMessageChannel?
--- @return boolean
function SNetworkServer.sendReliable(clientID, messageType, messageContent, messageChannel)
	local session = SNetworkServer.getNetworkSession()
	if not session then
		return false
	end

	local data = GNetworkMessage.pack(messageType, messageContent)

	if log.canTrace() then
		log.trace(("Send reliable message to client %s: %s"):format(clientID, data))
	end

	session:sendReliable(clientID, data, messageChannel or GNetworkMessage.Channel.Main)

	return true
end

--- @param clientID TE.Network.ClientID
--- @param messageType dr2c.MessageType
--- @param messageContent any?
--- @param channel dr2c.NetworkMessageChannel?
--- @return boolean success
function SNetworkServer.sendUnreliable(clientID, messageType, messageContent, channel)
	local session = SNetworkServer.getNetworkSession()
	if not session then
		return false
	end

	local data = GNetworkMessage.pack(messageType, messageContent)

	if log.canTrace() then
		log.trace(("Send unreliable message to client %d: %s"):format(clientID, data))
	end

	session:sendUnreliable(clientID, data, channel or GNetworkMessage.Channel.Main)

	return true
end

--- Broadcast reliable message to all clients on the server.
--- 向服务器内所有客户端广播可靠消息
--- @param messageType dr2c.MessageType
--- @param messageContent any?
--- @param channel dr2c.NetworkMessageChannel?
--- @return boolean success
function SNetworkServer.broadcastReliable(messageType, messageContent, channel)
	local session = SNetworkServer.getNetworkSession()
	if not session then
		return false
	end

	local data = GNetworkMessage.pack(messageType, messageContent)

	if log.canTrace() then
		log.trace(("Broadcast reliable message: %s"):format(data))
	end

	session:broadcastReliable(data, channel or GNetworkMessage.Channel.Main)

	return true
end

--- Broadcast unreliable message to all clients in this server.
--- 向服务器内所有客户端广播不可靠消息
--- @param messageType dr2c.MessageType
--- @param messageContent any?
--- @param channel dr2c.NetworkMessageChannel?
--- @return boolean success
function SNetworkServer.broadcastUnreliable(messageType, messageContent, channel)
	local session = SNetworkServer.getNetworkSession()
	if session then
		local data = GNetworkMessage.pack(messageType, messageContent)

		if log.canTrace() then
			log.trace(("Broadcast unreliable message: %s"):format(data))
		end

		session:broadcastUnreliable(data, channel or GNetworkMessage.Channel.Main)

		return true
	else
		return false
	end
end

SNetworkServer.eventSHost = TE.events:new(N_("SHost"), {
	"Reset",
	"Initialize",
})

SNetworkServer.eventSShutdown = TE.events:new(N_("SShutdown"), {
	"Deinitialize",
	"Reset",
})

SNetworkServer.eventSConnect = TE.events:new(N_("SConnect"), {
	"Network",
	"Reset",
	"Clients",
	"Verify",
})

SNetworkServer.eventSDisconnect = TE.events:new(N_("SDisconnect"), {
	"Clients",
	"Reset",
})

SNetworkServer.eventSMessage = TE.events:new(N_("SMessage"), {
	"Overrides",
	"Request",
	"Response",
	"Receive",
	"Broadcast",
})

TE.events:add("ServerHost", function(e)
	TE.events:invoke(SNetworkServer.eventSHost, {})
end)

TE.events:add("ServerShutdown", function(e)
	TE.events:invoke(SNetworkServer.eventSShutdown, {})
end)

--- @param clientID TE.Network.ClientID
local function invokeEventServerConnect(clientID)
	--- @class dr2c.E.SConnect
	--- @field clientID TE.Network.ClientID
	local e = {
		clientID = clientID,
	}

	TE.events:invoke(SNetworkServer.eventSConnect, e)
end

--- @param e TE.E.ServerMessage
TE.events:add("ServerConnect", function(e)
	invokeEventServerConnect(e.data.clientID)
end)

--- @param clientID TE.Network.ClientID
local function invokeEventServerDisconnect(clientID)
	--- @class dr2c.E.SDisconnect
	local e = {
		clientID = clientID,
	}

	TE.events:invoke(SNetworkServer.eventSDisconnect, e, nil, EEventInvocation.None)
end

--- @param e TE.E.ServerDisconnect
TE.events:add("ServerDisconnect", function(e)
	invokeEventServerDisconnect(e.data.clientID)
end)

--- @param messageContent any?
--- @param messageType dr2c.MessageType
local function invokeEventServerMessage(clientID, messageContent, messageType)
	--- @class dr2c.E.SMessage
	--- @field clientID TE.Network.ClientID
	--- @field content any
	--- @field type dr2c.MessageType
	local e = {
		clientID = clientID,
		content = messageContent,
		type = messageType,
	}

	--- @diagnostic disable-next-line: param-type-mismatch
	TE.events:invoke(SNetworkServer.eventSMessage, e, messageType)
end

--- @param e TE.E.ServerMessage
TE.events:add("ServerMessage", function(e)
	local messageType, messageContent = GNetworkMessage.unpack(e.data.message)

	if GNetworkMessage.isUnessential(messageType) then
		-- Let the engine broadcast the message directly if message type is unessential.
		-- 让引擎直接广播非必要的消息。
		e.data.broadcast = e.data.message
	else
		invokeEventServerMessage(e.data.clientID, messageContent, messageType)
	end
end, "BroadcastUnessentialMessage", nil, sessionSlot)

--- @param e dr2c.E.SConnect
TE.events:add(SNetworkServer.eventSConnect, function(e)
	local clientID = e.clientID

	if log.canTrace() then
		log.trace(("Broadcast client %d was connected"):format(clientID))
	end

	local fields = GNetworkMessageFields.CClientConnect

	SNetworkServer.broadcastReliable(GNetworkMessage.Type.CClientConnect, {
		[fields.clientID] = clientID,
	})
end, "BroadcastClientConnect", "Network")

return SNetworkServer
