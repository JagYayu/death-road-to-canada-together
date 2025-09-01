local String = require("tudov.String")
local Enum = require("tudov.Enum")

local CClients = require("dr2c.client.network.Clients")
local GClient = require("dr2c.shared.network.Client")
local GMessage = require("dr2c.shared.network.Message")
local GPlatform = require("dr2c.client.network.Platform")
local GThrottle = require("dr2c.shared.utils.Throttle")

local network = network

--- @class dr2c.CClient
local CClient = {}

--- @type integer
CClient.sessionSlot = 0

--- @return Network.Client?
function CClient.getNetworkSession()
	return network:getClient(CClient.sessionSlot)
end

--- @return Network.ClientID?
function CClient.getClientID()
	local session = CClient.getNetworkSession()
	return session and session:getSessionID()
end

events:add(N_("CUpdate"), function(e)
	local session = CClient.getNetworkSession()
	if not session then
		return
	end

	local clientID = session:getSessionID()
	if not clientID or CClients.hasClient(clientID) then
		return
	end

	if log.canTrace() then
		log.trace(("Initialize local client %s's attributes"):format(clientID))
	end

	local PublicAttribute = GClient.PublicAttribute
	local PrivateAttribute = GClient.PrivateAttribute
	local setPublicAttribute = CClients.setPublicAttribute
	local setPrivateAttribute = CClients.setPrivateAttribute

	CClients.addClient(clientID)
	setPublicAttribute(clientID, PublicAttribute.ID, clientID)
	setPublicAttribute(clientID, PublicAttribute.State, GClient.State.Verifying)
	setPublicAttribute(clientID, PublicAttribute.Platform, GPlatform.getType())
	setPublicAttribute(clientID, PublicAttribute.PlatformID, nil)
	setPublicAttribute(clientID, PublicAttribute.OperatingSystem, OperatingSystem.getType())
	setPublicAttribute(clientID, PublicAttribute.Statistics, {})
	setPublicAttribute(clientID, PublicAttribute.DisplayName, nil)
	setPublicAttribute(clientID, PublicAttribute.Room, nil)
	setPublicAttribute(clientID, PublicAttribute.Version, engine:getVersion())
	setPublicAttribute(clientID, PublicAttribute.ContentHash, 0)
	setPublicAttribute(clientID, PublicAttribute.ModsHash, 0)
	setPublicAttribute(clientID, PublicAttribute.SocketType, session:getSocketType())
	setPrivateAttribute(clientID, PrivateAttribute.SecretToken, nil)
	setPrivateAttribute(clientID, PrivateAttribute.SecretStatistics, nil)
end, N_("InitializeLocalClientAttributes"), "Network")

CClient.eventClientConnect = events:new(N_("CConnect"), {
	"Initialize",
})

CClient.eventClientDisconnect = events:new(N_("CDisconnect"), {
	"Reset",
})

CClient.eventClientMessage = events:new(N_("CMessage"), {
	"Overrides",
	"Receive",
	"Rollback",
}, Enum.eventKeys(GMessage.Type))

--- @param messageType dr2c.MessageType
--- @param messageContent any?
--- @param channel dr2c.GMessage.Channel?
--- @return boolean success
function CClient.sendReliable(messageType, messageContent, channel)
	local session = CClient.getNetworkSession()
	if session then
		local data = GMessage.pack(messageType, messageContent)

		if log.canTrace() then
			-- log.trace(("Send reliable message to server: %s"):format(data))
		end

		channel = channel or GMessage.Channel.Main
		---@diagnostic disable-next-line: param-type-mismatch
		session:sendReliable(data, channel)

		return true
	else
		return false
	end
end

--- @param messageType dr2c.MessageType
--- @param messageContent any?
--- @param channel dr2c.GMessage.Channel?
--- @return boolean success
function CClient.sendUnreliable(messageType, messageContent, channel)
	local session = CClient.getNetworkSession()
	if session then
		local data = GMessage.pack(messageType, messageContent)

		if log.canTrace() then
			-- log.trace(("Send unreliable message to server: %s"):format(data))
		end

		channel = channel or GMessage.Channel.Main
		---@diagnostic disable-next-line: param-type-mismatch
		session:sendUnreliable(data, channel)

		return true
	else
		return false
	end
end

local function invokeEventClientConnect(clientID)
	--- @class dr2c.E.ClientConnect
	local e = {
		clientID = clientID,
	}

	events:invoke(CClient.eventClientConnect, e)
end

--- @param e Events.E.ClientConnect
events:add("ClientConnect", function(e)
	invokeEventClientConnect(e.data.clientID)
end)

local function invokeEventClientDisconnect(clientID)
	--- @class dr2c.E.ClientDisconnect
	local e = {
		clientID = clientID,
	}

	events:invoke(CClient.eventClientDisconnect, e)
end

--- @param e Events.E.ClientConnect
events:add("ClientDisconnect", function(e)
	invokeEventClientDisconnect(e.data.clientID)
end)

--- @param messageContent any?
--- @param messageType dr2c.MessageType
local function invokeEventClientMessage(messageContent, messageType)
	--- @class dr2c.E.ClientMessage
	--- @field content any?
	--- @field type dr2c.MessageType
	local e = {
		content = messageContent,
		type = messageType,
	}

	events:invoke(CClient.eventClientMessage, e, messageType)
end

--- @param e Events.E.ClientMessage
events:add("ClientMessage", function(e)
	if type(e.data) ~= "userdata" then
		return
	end

	local messageType, messageContent = GMessage.unpack(e.data.message)

	invokeEventClientMessage(messageContent, messageType)
end, "clientMessage", nil, CClient.sessionSlot)

local throttleClientUpdateClientsPrivateAttributeRequests = GThrottle.newTime(0.25)

--- @param e dr2c.E.ClientUpdate
events:add(N_("CUpdate"), function(e)
	e.networkThrottle = throttleClientUpdateClientsPrivateAttributeRequests()
end, "AssignNetworkThrottle", "Network", nil, -1)

local sendClientVerifyingAttributesTime = 0

CClient.eventCCollectVerifyAttributes = events:new(N_("CCollectVerifyAttributes"), {
	"Public",
	"Private",
	"State",
})

events:add(N_("CCollectVerifyAttributes"), function(e)
	e.public[#e.public + 1] = {
		attribute = GClient.PublicAttribute.State,
		value = GClient.State.Verified,
	}
end, "SendState", "State")

--- @param pe dr2c.E.ClientUpdate
local function invokeEventClientCollectVerifyAttributes(pe)
	local public = {}
	local private = {}

	--- @class dr2c.E.ClientCollectVerifyAttributes
	local e = {
		pe = pe,
		public = public,
		private = private,
	}

	events:invoke(CClient.eventCCollectVerifyAttributes, e)

	if log.canTrace() then
		log.trace("Collected verify attributes: ", public, private)
	end

	return public, private
end

--- @param e dr2c.E.ClientUpdate
events:add(N_("CUpdate"), function(e)
	if e.networkThrottle then
		return
	end

	local clientID = CClient.getClientID()
	if not clientID then
		return
	end

	local time = Time.getSystemTime()
	if time < sendClientVerifyingAttributesTime then
		return
	end

	local state = CClients.getPublicAttribute(clientID, GClient.PublicAttribute.State)
	if state ~= GClient.State.Verifying then
		return
	end

	local publicEntries, privateEntries = invokeEventClientCollectVerifyAttributes(e)

	print("Send client verifying attributes")

	for _, entry in ipairs(privateEntries) do
		CClient.sendReliable(GMessage.Type.ClientPrivateAttribute, entry)
	end

	for _, entry in ipairs(publicEntries) do
		CClient.sendReliable(GMessage.Type.ClientPublicAttribute, entry)
	end

	sendClientVerifyingAttributesTime = time + 5
end, "SendClientVerifyingAttributes", "Network")

function CClient.getRoom() end

return CClient
