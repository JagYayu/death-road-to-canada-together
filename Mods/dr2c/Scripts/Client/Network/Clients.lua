--[[
-- @module dr2c.Client.Network.Clients
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local List = require("TE.List")
local Utility = require("TE.Utility")

local CClient = require("dr2c.Client.Network.Client")
local GThrottle = require("dr2c.Shared.Utils.Throttle")
local GClient = require("dr2c.Shared.Network.Client")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")

--- @class dr2c.ClientPrivateRequestEntry
--- @field attribute dr2c.NetworkClientPrivateAttribute
--- @field callback fun(success: boolean, attribute: any)
--- @field expireTime number

--- @class dr2c.CClients
local CNetworkClients = {}

--- @type TE.Network.ClientID[]
local clientList = {}

--- @type table<TE.Network.ClientID, table<dr2c.NetworkClientPublicAttribute, any>>
local clientsPublicAttributes = {}

--- @type table<TE.Network.ClientID, table<dr2c.NetworkClientPublicAttribute, any>>
local clientsPrivateAttributes = {}

--- @type table<number, dr2c.ClientPrivateRequestEntry>
local privateAttributeRequests = {}

--- @type integer
local latestPrivateAttributeRequestID = 0

clientsPublicAttributes = persist("clientsPublicAttributes", function()
	return clientsPublicAttributes
end)
clientsPrivateAttributes = persist("clientsPrivateAttributes", function()
	return clientsPrivateAttributes
end)
privateAttributeRequests = persist("privateAttributeRequests", function()
	return privateAttributeRequests
end)
latestPrivateAttributeRequestID = persist("latestPrivateAttributeRequestID", function()
	return latestPrivateAttributeRequestID
end)

--- @type table<number, TE.Network.ClientID>
local secretToken2ClientID = {}
secretToken2ClientID = persist("secretToken2ClientID", function()
	return secretToken2ClientID
end)

CNetworkClients.eventCClientAdded = TE.events:new(N_("CClientAdded"), {
	"PlayerInputBuffer",
})
CNetworkClients.eventCClientRemoved = TE.events:new(N_("CClientRemoved"), {
	"PlayerInputBuffer",
})

--- @param clientID TE.Network.ClientID
--- @return boolean
--- @nodiscard
function CNetworkClients.hasClient(clientID)
	return clientsPublicAttributes[clientID] ~= nil
end

--- @param clientID TE.Network.ClientID
function CNetworkClients.addClient(clientID)
	if CNetworkClients.hasClient(clientID) then
		error(("Client %s already exists"):format(clientID), 2)
	end

	clientList[#clientList + 1] = clientID
	clientsPublicAttributes[clientID] = {}
	clientsPrivateAttributes[clientID] = {}

	--- @class dr2c.E.CClientAdded
	local e = {
		clientID = clientID,
	}
	TE.events:invoke(CNetworkClients.eventCClientAdded, e)
end

--- @param clientID TE.Network.ClientID
function CNetworkClients.removeClient(clientID)
	if not CNetworkClients.hasClient(clientID) then
		error(("Client %s already does not exist"):format(clientID), 2)
	end

	List.removeFirst(clientList, clientID)
	clientsPublicAttributes[clientID] = nil
	clientsPrivateAttributes[clientID] = nil

	--- @class dr2c.E.CClientRemoved
	local e = {
		clientID = clientID,
	}
	TE.events:invoke(CNetworkClients.eventCClientRemoved, e)
end

--- @param clientID TE.Network.ClientID
--- @param publicAttribute dr2c.NetworkClientPublicAttribute
--- @return any?
--- @nodiscard
function CNetworkClients.getPublicAttribute(clientID, publicAttribute)
	local attributes = clientsPublicAttributes[clientID]
	if attributes then
		return attributes[publicAttribute]
	end
end

--- @param clientID TE.Network.ClientID
--- @param privateAttribute dr2c.NetworkClientPrivateAttribute
--- @param callback fun(success: boolean, attribute: any)
function CNetworkClients.requestPrivateAttribute(clientID, privateAttribute, callback)
	local attributes = clientsPublicAttributes[clientID]
	local value = attributes[privateAttribute]

	if value ~= nil then
		callback(true, value)
	else
		local requestID = latestPrivateAttributeRequestID + 1
		latestPrivateAttributeRequestID = requestID

		local fields = GNetworkMessageFields.ClientPrivateAttribute
		CClient.sendReliable(GNetworkMessage.Type.ClientPrivateAttribute, {
			[fields.requestID] = requestID,
			[fields.clientID] = clientID,
			[fields.attribute] = privateAttribute,
		})

		privateAttributeRequests[requestID] = {
			attribute = privateAttribute,
			callback = callback,
			expireTime = Time.getSystemTime() + 10,
		}
	end
end

--- @warn Do not modify attributes other than local client as possible.
--- @param clientID TE.Network.ClientID
--- @param publicAttribute dr2c.NetworkClientPublicAttribute
--- @param attributeValue any
function CNetworkClients.setPublicAttribute(clientID, publicAttribute, attributeValue)
	local attributes = clientsPublicAttributes[clientID]
	if not attributes then
		error(("Client %s does not exists"):format(clientID), 2)
	end

	attributes[publicAttribute] = attributeValue
end

--- @warn Do not modify attributes other than local client as possible.
--- @param clientID TE.Network.ClientID
--- @param privateAttribute dr2c.NetworkClientPrivateAttribute
--- @param attributeValue any
function CNetworkClients.setPrivateAttribute(clientID, privateAttribute, attributeValue)
	local attributes = clientsPrivateAttributes[clientID]
	if not attributes then
		error(("Client %s does not exists"):format(clientID), 2)
	end

	attributes[privateAttribute] = attributeValue
end

TE.events:add(N_("CDisconnect"), function(e)
	clientsPublicAttributes = {}
	clientsPrivateAttributes = {}
	privateAttributeRequests = {}
end, N_("ResetClients"), "Reset")

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	if type(e.content) ~= "table" then
		return
	end

	local fields = GNetworkMessageFields.ClientConnect

	local clientID = e.content[fields.clientID]
	if not clientID then
		return
	end

	if log.canTrace() then
		log.trace(("Received client %s connect message"):format(clientID))
	end

	CNetworkClients.addClient(clientID)
end, "ReceiveClientConnect", "Receive", GNetworkMessage.Type.ClientConnect)

TE.events:add(N_("CMessage"), function(e)
	if type(e.content) ~= "table" then
		return
	end

	local fields = GNetworkMessageFields.ClientConnect

	local clientID = e.content[fields.clientID]
	if not clientID then
		return
	end

	if log.canTrace() then
		log.trace(("Received client %s disconnect message"):format(clientID))
	end

	CNetworkClients.removeClient(clientID)
end, "ReceiveClientDisconnect", "Receive", GNetworkMessage.Type.ClientDisconnect)

local function initializeClientsAttributes(serverClientsPublicAttributes)
	for clientID, clientPublicAttributes in pairs(serverClientsPublicAttributes) do
		if not CNetworkClients.hasClient(clientID) then
			CNetworkClients.addClient(clientID)

			clientsPublicAttributes[clientID] = clientPublicAttributes
		end
	end
end

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	initializeClientsAttributes(e.content)
end, "InitializeClientsAttributes", "Receive", GNetworkMessage.Type.Clients)

--- @param e dr2c.E.CMessage
local function receiveClientAttribute(e, validate, clientsAttributes)
	if type(e.content) ~= "table" then
		return
	end

	local fields = GNetworkMessageFields.ClientPublicAttribute

	local clientID = e.content[fields.clientID]
	local attribute = e.content[fields.attribute]
	local value = e.content[fields.value]
	if type(clientID) ~= "number" or type(attribute) ~= "number" or not validate(attribute, value) then
		return
	end

	local attributes = clientsAttributes[clientID]
	if attributes then
		attributes[attribute] = value
	end

	local requestID = e.content[fields.requestID]
	if requestID then
		privateAttributeRequests[requestID] = nil
	end
end

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	receiveClientAttribute(e, GClient.validatePublicAttribute, clientsPublicAttributes)
end, "ReceiveClientPublicAttribute", "Receive", GNetworkMessage.Type.ClientPublicAttribute)

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	receiveClientAttribute(e, GClient.validatePrivateAttribute, clientsPrivateAttributes)
end, "ReceiveClientPrivateAttribute", "Receive", GNetworkMessage.Type.ClientPrivateAttribute)

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	if type(e.content) ~= "table" then
		return
	end

	local fields = GNetworkMessageFields.ClientPublicAttribute

	local clientID = e.content[fields.clientID]
	local attribute = e.content[fields.attribute]
	if type(clientID) ~= "number" or type(attribute) ~= "number" then
		return
	end

	local requests = privateAttributeRequests[clientID]
	if requests then
		requests[attribute] = nil
	end
end, "ClearPrivateAttributeCache", "Receive", GNetworkMessage.Type.ClientPrivateAttribute)

TE.events:add(N_("CUpdate"), function(e)
	if e.networkThrottle then
		return
	end

	local time = Time.getSystemTime()

	for clientID, request in pairs(privateAttributeRequests) do
		if time > request.expireTime then
			if log.canTrace() then
				log.warn(
					("Private attribute request expired: clientID=%d, attribute=%d"):format(clientID, request.attribute)
				)
			end

			privateAttributeRequests[clientID] = nil
		end
	end
end, "UpdatePrivateAttributeRequests", "Network")

return CNetworkClients
