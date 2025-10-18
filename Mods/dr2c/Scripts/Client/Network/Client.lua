--[[
-- @module dr2c.Client.network.Client
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local String = require("TE.String")
local Enum = require("TE.Enum")

local CNetworkClients = require("dr2c.Client.Network.Clients")
local GNetworkClient = require("dr2c.Shared.Network.Client")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkPlatform = require("dr2c.Client.Network.Platform")
local GUtilsThrottle = require("dr2c.Shared.Utils.Throttle")

local network = TE.network

--- @class dr2c.CNetworkClient
local CNetworkClient = {}

local clientPrivateAttributes = {}

--- @type integer
local sessionSlot = 0

--- @return TE.Network.Client?
function CNetworkClient.getNetworkSession()
	return network:getClient(sessionSlot)
end

--- @return TE.Network.ClientID?
function CNetworkClient.getClientID()
	local session = CNetworkClient.getNetworkSession()
	return session and session:getSessionID()
end

--- @param publicAttribute dr2c.NetworkClientPublicAttribute
--- @return any?
function CNetworkClient.getPublicAttribute(publicAttribute)
	local clientID = CNetworkClient.getClientID()
	if clientID then
		return CNetworkClients.getPublicAttribute(clientID, publicAttribute)
	end
end

--- @param privateAttribute dr2c.NetworkClientPrivateAttribute
--- @return any?
function CNetworkClient.getPrivateAttribute(privateAttribute)
	return clientPrivateAttributes[privateAttribute]
end

CNetworkClient.eventCConnect = TE.events:new(N_("CConnect"), {
	"Reset",
	"Initialize",
})

CNetworkClient.eventCDisconnect = TE.events:new(N_("CDisconnect"), {
	"Reset",
})

CNetworkClient.eventCMessage = TE.events:new(N_("CMessage"), {
	"Overrides",
	"Receive",
	"PlayerInputBuffer",
	"Rollback",
}, Enum.eventKeys(GNetworkMessage.Type))

--- @param messageType dr2c.NetworkMessageType
--- @param messageContent? any
--- @param messageChannel? dr2c.NetworkMessageChannel
--- @return boolean success
function CNetworkClient.sendReliable(messageType, messageContent, messageChannel, messageDictionary)
	local session = CNetworkClient.getNetworkSession()
	if session then
		local data = GNetworkMessage.pack(messageType, messageContent)

		if log.canTrace() then
			log.trace(("Send reliable message to server: %s"):format(data))
		end

		messageChannel = messageChannel or GNetworkMessage.Channel.Main
		---@diagnostic disable-next-line: param-type-mismatch
		session:sendReliable(data, messageChannel)

		return true
	else
		return false
	end
end

--- @param messageType dr2c.NetworkMessageType
--- @param messageContent any?
--- @param channel dr2c.NetworkMessageChannel?
--- @return boolean success
function CNetworkClient.sendUnreliable(messageType, messageContent, channel)
	local session = CNetworkClient.getNetworkSession()
	if session then
		local data = GNetworkMessage.pack(messageType, messageContent)

		if log.canTrace() then
			-- log.trace(("Send unreliable message to server: %s"):format(data))
		end

		channel = channel or GNetworkMessage.Channel.Main
		---@diagnostic disable-next-line: param-type-mismatch
		session:sendUnreliable(data, channel)

		return true
	else
		return false
	end
end

TE.events:add(N_("CUpdate"), function(e)
	-- local session = CClient.getNetworkSession()
	-- if not session then
	-- 	return
	-- end

	-- local clientID = session:getSessionID()
	-- if not clientID or CClients.hasClient(clientID) then
	-- 	return
	-- end

	-- if log.canTrace() then
	-- 	log.trace(("Initialize local client %s's attributes"):format(clientID))
	-- end

	-- local PublicAttribute = GClient.PublicAttribute
	-- local PrivateAttribute = GClient.PrivateAttribute
	-- local setPublicAttribute = CClients.setPublicAttribute
	-- local setPrivateAttribute = CClients.setPrivateAttribute

	-- CClients.addClient(clientID)
	-- setPublicAttribute(clientID, PublicAttribute.ID, clientID)
	-- setPublicAttribute(clientID, PublicAttribute.State, GClient.State.Verifying)
	-- setPublicAttribute(clientID, PublicAttribute.Platform, GPlatform.getType())
	-- setPublicAttribute(clientID, PublicAttribute.PlatformID, nil)
	-- setPublicAttribute(clientID, PublicAttribute.OperatingSystem, OperatingSystem.getType())
	-- setPublicAttribute(clientID, PublicAttribute.Statistics, {})
	-- setPublicAttribute(clientID, PublicAttribute.DisplayName, nil)
	-- setPublicAttribute(clientID, PublicAttribute.Room, nil)
	-- setPublicAttribute(clientID, PublicAttribute.Version, TE.engine:getVersion())
	-- setPublicAttribute(clientID, PublicAttribute.ContentHash, 0)
	-- setPublicAttribute(clientID, PublicAttribute.ModsHash, 0)
	-- setPublicAttribute(clientID, PublicAttribute.SocketType, session:getSocketType())
	-- setPrivateAttribute(clientID, PrivateAttribute.SecretToken, nil)
	-- setPrivateAttribute(clientID, PrivateAttribute.SecretStatistics, nil)
end, N_("InitializeLocalClientAttributes"), "Network")

local function invokeEventClientConnect()
	--- @class dr2c.E.ClientConnect
	local e = {}
	TE.events:invoke(CNetworkClient.eventCConnect, e)
end

--- @param e Events.E.ClientConnect
TE.events:add("ClientConnect", function(e)
	invokeEventClientConnect()
end)

local function invokeEventClientDisconnect(clientID)
	--- @class dr2c.E.ClientDisconnect
	local e = {
		clientID = clientID,
	}

	TE.events:invoke(CNetworkClient.eventCDisconnect, e)
end

--- @param e Events.E.ClientConnect
TE.events:add("ClientDisconnect", function(e)
	invokeEventClientDisconnect(e.data.clientID)
end)

--- @param messageContent any?
--- @param messageType dr2c.NetworkMessageType
local function invokeEventClientMessage(messageContent, messageType)
	--- @class dr2c.E.CMessage
	--- @field content any?
	--- @field type dr2c.NetworkMessageType
	local e = {
		content = messageContent,
		type = messageType,
	}

	TE.events:invoke(CNetworkClient.eventCMessage, e, messageType)
end

--- @param e Events.E.ClientMessage
TE.events:add("ClientMessage", function(e)
	if type(e.data) ~= "userdata" then
		return
	end

	local messageType, messageContent = GNetworkMessage.unpack(e.data.message)

	invokeEventClientMessage(messageContent, messageType)
end, "clientMessage", nil, sessionSlot)

local throttleClientUpdateClientsPrivateAttributeRequests = GUtilsThrottle.newTime(0.25)

--- @param e dr2c.E.ClientUpdate
TE.events:add(N_("CUpdate"), function(e)
	e.networkThrottle = throttleClientUpdateClientsPrivateAttributeRequests()
end, "AssignNetworkThrottle", "Network", nil, -1)

local sendClientVerifyingAttributesTime = 0

CNetworkClient.eventCCollectVerifyAttributes = TE.events:new(N_("CCollectVerifyAttributes"), {
	"Public",
	"Private",
	"State",
})

TE.events:add(N_("CCollectVerifyAttributes"), function(e)
	e.attributes[#e.attributes + 1] = {
		public = true,
		attribute = GNetworkClient.PublicAttribute.State,
		value = GNetworkClient.State.Verified,
	}
end, "SendState", "State")

--- @param pe dr2c.E.ClientUpdate
local function invokeEventClientCollectVerifyAttributes(pe)
	local attributes = {}

	--- @class dr2c.E.ClientCollectVerifyAttributes
	local e = {
		pe = pe,
		attributes = attributes,
	}

	TE.events:invoke(CNetworkClient.eventCCollectVerifyAttributes, e)

	if log.canTrace() then
		log.trace("Collected verify attributes: ", attributes)
	end

	return attributes
end

--- @param e dr2c.E.ClientUpdate
TE.events:add(N_("CUpdate"), function(e)
	if e.networkThrottle then
		return
	end

	local clientID = CNetworkClient.getClientID()
	if not clientID then
		return
	end

	local time = Time.getSystemTime()
	if time < sendClientVerifyingAttributesTime then
		return
	end

	local state = CNetworkClients.getPublicAttribute(clientID, GNetworkClient.PublicAttribute.State)
	if state ~= GNetworkClient.State.Verifying then
		return
	end

	local entries = invokeEventClientCollectVerifyAttributes(e)

	for _, entry in ipairs(entries) do
		if entry.public then
			CNetworkClient.sendReliable(GNetworkMessage.Type.ClientPublicAttribute, entry)
		else
			CNetworkClient.sendReliable(GNetworkMessage.Type.ClientPrivateAttribute, entry)
		end
	end

	sendClientVerifyingAttributesTime = time + 5
end, "SendClientVerifyingAttributes", "Network")

return CNetworkClient
