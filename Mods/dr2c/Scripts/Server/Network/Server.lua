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
local GNetworkServer = require("dr2c.Shared.Network.Server")
local SNetworkClients = require("dr2c.Server.Network.Clients")
local SNetworkRoom = require("dr2c.Server.Network.Room")

local network = TE.network

--- @class dr2c.SServer
local SNetworkServer = {}

SNetworkServer.sessionSlot = 0

--- @type table<dr2c.ServerAttribute, Serializable>
local serverAttributes = {}

serverAttributes = persist("serverAttributes", function()
	return serverAttributes
end)

function SNetworkServer.getAttributes()
	return serverAttributes
end

--- @param attribute dr2c.ServerAttribute
function SNetworkServer.getAttribute(attribute)
	return serverAttributes[attribute]
end

--- @return TE.Network.Server?
function SNetworkServer.getNetworkSession()
	return network:getServer(SNetworkServer.sessionSlot)
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
		[GNetworkServer.Attribute.Rooms] = { SNetworkRoom.defaultRoomID },
		[GNetworkServer.Attribute.SocketType] = session:getSocketType(),
	}
end, "InitializeServer", "Network")

--- @param clientID TE.Network.ClientID
--- @param disconnectionCode dr2c.DisconnectionCode
--- @return boolean
function SNetworkServer.disconnect(clientID, disconnectionCode)
	local session = SNetworkServer.getNetworkSession()
	if session then
		session:disconnect(clientID, disconnectionCode)

		return true
	else
		return false
	end
end

--- @param clientID TE.Network.ClientID
--- @param messageType dr2c.NetworkMessageType
--- @param messageContent any?
--- @param channel dr2c.NetworkMessageChannel?
--- @return boolean
function SNetworkServer.sendReliable(clientID, messageType, messageContent, channel)
	local session = SNetworkServer.getNetworkSession()
	if session then
		local data = GNetworkMessage.pack(messageType, messageContent)

		if log.canTrace() then
			log.trace(("Send reliable message to client %s: %s"):format(clientID, data))
		end

		channel = channel or GNetworkMessage.Channel.Main
		--- @diagnostic disable-next-line: param-type-mismatch
		session:sendReliable(clientID, data, channel)

		return true
	else
		return false
	end
end

--- @param clientID TE.Network.ClientID
--- @param messageType dr2c.NetworkMessageType
--- @param messageContent any?
--- @param channel dr2c.NetworkMessageChannel?
--- @return boolean success
function SNetworkServer.sendUnreliable(clientID, messageType, messageContent, channel)
	local session = SNetworkServer.getNetworkSession()
	if session then
		local data = GNetworkMessage.pack(messageType, messageContent)

		if log.canTrace() then
			log.trace(("Send unreliable message to client %d: %s"):format(clientID, data))
		end

		channel = channel or GNetworkMessage.Channel.Main
		--- @diagnostic disable-next-line: param-type-mismatch
		session:sendUnreliable(clientID, data, channel)

		return true
	else
		return false
	end
end

--- Broadcast message to all clients in this server.
--- @param messageType dr2c.NetworkMessageType
--- @param messageContent any?
--- @param channel dr2c.NetworkMessageChannel?
--- @return boolean success
function SNetworkServer.broadcastReliable(messageType, messageContent, channel)
	local session = SNetworkServer.getNetworkSession()
	if session then
		local data = GNetworkMessage.pack(messageType, messageContent)

		-- if log.canTrace() then
		-- 	log.trace(("Broadcast reliable message: %s"):format(data))
		-- end

		channel = channel or GNetworkMessage.Channel.Main
		--- @diagnostic disable-next-line: param-type-mismatch
		session:broadcastReliable(data, channel)

		return true
	else
		return false
	end
end

--- @param messageType dr2c.NetworkMessageType
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

		channel = channel or GNetworkMessage.Channel.Main
		--- @diagnostic disable-next-line: param-type-mismatch
		session:broadcastUnreliable(data, channel)

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
	"Reset",
})

SNetworkServer.eventSConnect = TE.events:new(N_("SConnect"), {
	"Reset",
	"Clients",
	"PlayerInputBuffer",
})

SNetworkServer.eventSDisconnect = TE.events:new(N_("SDisconnect"), {
	"Clients",
	"Reset",
})

SNetworkServer.eventSMessage = TE.events:new(N_("SMessage"), {
	"Overrides",
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

--- @param e Events.E.ServerMessage
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

--- @param e Events.E.ServerDisconnect
TE.events:add("ServerDisconnect", function(e)
	invokeEventServerDisconnect(e.data.clientID)
end)

--- @param messageContent any?
--- @param messageType dr2c.NetworkMessageType
local function invokeEventServerMessage(clientID, messageContent, messageType)
	--- @class dr2c.E.SMessage
	--- @field clientID TE.Network.ClientID
	--- @field content any
	--- @field type dr2c.NetworkMessageType
	--- @field broadcast table?
	local e = {
		clientID = clientID,
		content = messageContent,
		type = messageType,
	}

	--- @diagnostic disable-next-line: param-type-mismatch
	TE.events:invoke(SNetworkServer.eventSMessage, e, messageType)
end

--- @param e Events.E.ServerMessage
TE.events:add("ServerMessage", function(e)
	local messageType, messageContent = GNetworkMessage.unpack(e.data.message)

	if GNetworkMessage.isUnessential(messageType) then
		-- Let the engine broadcast the message directly if message type is unessential.
		e.data.broadcast = e.data.message
	else
		invokeEventServerMessage(e.data.clientID, messageContent, messageType)
	end
end, "ServerMessage", nil, SNetworkServer.sessionSlot)

--- @param e dr2c.E.SConnect
TE.events:add(SNetworkServer.eventSConnect, function(e)
	local clientID = e.clientID

	if log.canTrace() then
		log.trace(("Broadcast client %s connect message & public attribute state"):format(clientID))
	end

	SNetworkServer.broadcastReliable(GNetworkMessage.Type.ClientConnect, {
		clientID = clientID,
	})

	SNetworkServer.broadcastReliable(GNetworkMessage.Type.ClientPublicAttribute, {
		clientID = clientID,
		attribute = GNetworkClient.PublicAttribute.State,
		value = GNetworkClient.State.Verifying,
	})
end, "BroadcastClientConnectState", "Clients")

--- @param e dr2c.E.SMessage
TE.events:add(SNetworkServer.eventSMessage, function(e)
	if e.broadcast then
		SNetworkServer.broadcastReliable(e.type, e.broadcast)
	end
end, "BroadcastServerMessage", "Broadcast")

return SNetworkServer
