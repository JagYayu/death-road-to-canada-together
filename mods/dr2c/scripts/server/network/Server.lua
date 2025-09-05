--[[
-- @module dr2c.server.network.Server
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local GClient = require("dr2c.shared.network.Client")
local GMessage = require("dr2c.shared.network.Message")
local GServer = require("dr2c.shared.network.Server")
local SRoom = require("dr2c.server.network.Room")

--- @class dr2c.SServer
local SServer = {}

SServer.sessionSlot = 0

--- @type table<dr2c.ServerAttribute, Serializable>
local serverAttributes

--- @return Network.Server?
function SServer.getNetworkSession()
	return network:getServer(SServer.sessionSlot)
end

events:add(N_("SUpdate"), function()
	if serverAttributes then
		return
	end

	local session = SServer.getNetworkSession()
	if not session then
		return
	end

	serverAttributes = {
		[GServer.Attribute.DisplayName] = session:getTitle(),
		[GServer.Attribute.Clients] = session:getClients(),
		[GServer.Attribute.MaxClients] = session:getMaxClients(),
		[GServer.Attribute.StartupTime] = Time.getStartupTime(),
		[GServer.Attribute.Version] = engine:getVersion(),
		[GServer.Attribute.Mods] = {}, -- TODO get from mod manager
		[GServer.Attribute.HasPassword] = session:getPassword() ~= "",
		[GServer.Attribute.Rooms] = { SRoom.defaultRoomID },
		[GServer.Attribute.SocketType] = session:getSocketType(),
	}
end, "InitializeServerAttributes", "Network")

--- @param clientID Network.ClientID
--- @param disconnectionCode dr2c.DisconnectionCode
--- @return boolean
function SServer.disconnect(clientID, disconnectionCode)
	local session = SServer.getNetworkSession()
	if session then
		session:disconnect(clientID, disconnectionCode)

		return true
	else
		return false
	end
end

--- @param clientID Network.ClientID
--- @param messageType dr2c.NetworkMessageType
--- @param messageContent any?
--- @param channel dr2c.GMessage.Channel?
--- @return boolean
function SServer.sendReliable(clientID, messageType, messageContent, channel)
	local session = SServer.getNetworkSession()
	if session then
		local data = GMessage.pack(messageType, messageContent)

		if log.canTrace() then
			log.trace(("Send reliable message to client %s: %s"):format(clientID, data))
		end

		channel = channel or GMessage.Channel.Main
		--- @diagnostic disable-next-line: param-type-mismatch
		session:sendReliable(clientID, data, channel)

		return true
	else
		return false
	end
end

--- @param clientID Network.ClientID
--- @param messageType dr2c.NetworkMessageType
--- @param messageContent any?
--- @param channel dr2c.GMessage.Channel?
--- @return boolean success
function SServer.sendUnreliable(clientID, messageType, messageContent, channel)
	local session = SServer.getNetworkSession()
	if session then
		local data = GMessage.pack(messageType, messageContent)

		if log.canTrace() then
			log.trace(("Send unreliable message to client %d: %s"):format(clientID, data))
		end

		channel = channel or GMessage.Channel.Main
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
--- @param channel dr2c.GMessage.Channel?
--- @return boolean success
function SServer.broadcastReliable(messageType, messageContent, channel)
	local session = SServer.getNetworkSession()
	if session then
		local data = GMessage.pack(messageType, messageContent)

		-- if log.canTrace() then
		-- 	log.trace(("Broadcast reliable message: %s"):format(data))
		-- end

		channel = channel or GMessage.Channel.Main
		--- @diagnostic disable-next-line: param-type-mismatch
		session:broadcastReliable(data, channel)

		return true
	else
		return false
	end
end

--- @param messageType dr2c.NetworkMessageType
--- @param messageContent any?
--- @param channel dr2c.GMessage.Channel?
--- @return boolean success
function SServer.broadcastUnreliable(messageType, messageContent, channel)
	local session = SServer.getNetworkSession()
	if session then
		local data = GMessage.pack(messageType, messageContent)

		if log.canTrace() then
			log.trace(("Broadcast unreliable message: %s"):format(data))
		end

		channel = channel or GMessage.Channel.Main
		--- @diagnostic disable-next-line: param-type-mismatch
		session:broadcastUnreliable(data, channel)

		return true
	else
		return false
	end
end

SServer.eventSHost = events:new(N_("SHost"), {
	"Reset",
	"Initialize",
})

SServer.eventSShutdown = events:new(N_("SShutdown"), {
	"Reset",
})

SServer.eventSConnect = events:new(N_("SConnect"), {
	"Clients",
	"PlayerInputBuffer",
})

SServer.eventSDisconnect = events:new(N_("SDisconnect"), {
	"Clients",
	"Reset",
})

SServer.eventSMessage = events:new(N_("SMessage"), {
	"Overrides",
	"Receive",
	"Broadcast",
})

events:add("ServerHost", function(e)
	events:invoke(SServer.eventSHost, {})
end)

events:add("ServerShutdown", function(e)
	events:invoke(SServer.eventSShutdown, {})
end)

--- @param clientID Network.ClientID
local function invokeEventServerConnect(clientID)
	--- @class dr2c.E.ServerConnect
	--- @field clientID Network.ClientID
	local e = {
		clientID = clientID,
	}

	events:invoke(SServer.eventSConnect, e)
end

--- @param e Events.E.ServerMessage
events:add("ServerConnect", function(e)
	invokeEventServerConnect(e.data.clientID)
end)

--- @param clientID Network.ClientID
local function invokeEventServerDisconnect(clientID)
	--- @class dr2c.E.ServerDisconnect
	local e = {
		clientID = clientID,
	}

	events:invoke(SServer.eventSDisconnect, e, nil, EEventInvocation.None)
end

--- @param e Events.E.ServerDisconnect
events:add("ServerDisconnect", function(e)
	invokeEventServerDisconnect(e.data.clientID)
end)

--- @param messageContent any?
--- @param messageType dr2c.NetworkMessageType
local function invokeEventServerMessage(clientID, messageContent, messageType)
	--- @class dr2c.E.ServerMessage
	--- @field clientID Network.ClientID
	--- @field content any
	--- @field type dr2c.NetworkMessageType
	--- @field broadcast table?
	local e = {
		clientID = clientID,
		content = messageContent,
		type = messageType,
	}

	--- @diagnostic disable-next-line: param-type-mismatch
	events:invoke(SServer.eventSMessage, e, messageType)
end

--- @param e Events.E.ServerMessage
events:add("ServerMessage", function(e)
	local messageType, messageContent = GMessage.unpack(e.data.message)

	if GMessage.isUnessential(messageType) then
		-- Let the engine broadcast the message directly if message type is unessential.
		e.data.broadcast = e.data.message
	else
		invokeEventServerMessage(e.data.clientID, messageContent, messageType)
	end
end, "ServerMessage", nil, SServer.sessionSlot)

--- @param e dr2c.E.ServerConnect
events:add(SServer.eventSConnect, function(e)
	local clientID = e.clientID

	if log.canTrace() then
		log.trace(("Broadcast client %s connect message & public attribute state"):format(clientID))
	end

	SServer.broadcastReliable(GMessage.Type.ClientConnect, {
		clientID = clientID,
	})

	SServer.broadcastReliable(GMessage.Type.ClientPublicAttribute, {
		clientID = clientID,
		attribute = GClient.PublicAttribute.State,
		value = GClient.State.Verifying,
	})
end, "BroadcastClientConnectState", "Clients")

--- @param e dr2c.E.ServerMessage
events:add(SServer.eventSMessage, function(e)
	if e.broadcast then
		SServer.broadcastReliable(e.type, e.broadcast)
	end
end, "BroadcastServerMessage", "Broadcast")

return SServer
