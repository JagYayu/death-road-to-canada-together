--[[
-- @module dr2c.client.network.Clients
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local String = require("tudov.String")
local Enum = require("tudov.Enum")
local Utility = require("tudov.Utility")

local CClient = require("dr2c.client.network.Client")
local GThrottle = require("dr2c.shared.utils.Throttle")
local GClient = require("dr2c.shared.network.Client")
local GMessage = require("dr2c.shared.network.Message")

--- @class dr2c.ClientPrivateRequestEntry
--- @field attribute dr2c.NetworkClientPrivateAttribute
--- @field callback fun(success: boolean, attribute: any)
--- @field expireTime number

--- @class dr2c.CClients
local CClients = {}

--- @type table<number, Network.ClientID>
local secretToken2ClientID = {}
secretToken2ClientID = persist("secretToken2ClientID", function()
	return secretToken2ClientID
end)

--- @type table<Network.ClientID, table<dr2c.NetworkClientPublicAttribute, any>>
local clientsPublicAttributes = {}
clientsPublicAttributes = persist("clientsPublicAttributes", function()
	return clientsPublicAttributes
end)

--- @type table<Network.ClientID, table<dr2c.NetworkClientPublicAttribute, any>>
local clientsPrivateAttributes = {}
clientsPrivateAttributes = persist("clientsPrivateAttributes", function()
	return clientsPrivateAttributes
end)

--- @type table<number, dr2c.ClientPrivateRequestEntry>
local privateAttributeRequests = {}
privateAttributeRequests = persist("privateAttributeRequests", function()
	return privateAttributeRequests
end)

local privateAttributeRequestLatestID = 0
privateAttributeRequestLatestID = persist("privateAttributeRequestLatestID", function()
	return privateAttributeRequestLatestID
end)

CClients.eventCClientAdded = events:new(N_("CClientAdded"), {
	"PlayerInputBuffer",
})
CClients.eventCClientRemoved = events:new(N_("CClientRemoved"), {
	"PlayerInputBuffer",
})

--- @param clientID Network.ClientID
--- @return boolean
--- @nodiscard
function CClients.hasClient(clientID)
	return clientsPublicAttributes[clientID] ~= nil
end

--- @param clientID Network.ClientID
function CClients.addClient(clientID)
	if CClients.hasClient(clientID) then
		error(("Client %s already exists"):format(clientID), 2)
	end

	clientsPublicAttributes[clientID] = {}
	clientsPrivateAttributes[clientID] = {}

	--- @class dr2c.E.CClientAdded
	local e = {
		clientID = clientID,
	}
	events:invoke(CClients.eventCClientAdded, e)
end

--- @param clientID Network.ClientID
function CClients.removeClient(clientID)
	if not CClients.hasClient(clientID) then
		error(("Client %s already does not exist"):format(clientID), 2)
	end

	clientsPublicAttributes[clientID] = nil
	clientsPrivateAttributes[clientID] = nil

	--- @class dr2c.E.CClientRemoved
	local e = {
		clientID = clientID,
	}
	events:invoke(CClients.eventCClientRemoved, e)
end

--- @param clientID Network.ClientID
--- @param publicAttribute dr2c.NetworkClientPublicAttribute
--- @return any?
--- @nodiscard
function CClients.getPublicAttribute(clientID, publicAttribute)
	local attributes = clientsPublicAttributes[clientID]
	if attributes then
		return attributes[publicAttribute]
	end
end

--- @param clientID Network.ClientID
--- @param privateAttribute dr2c.NetworkClientPrivateAttribute
--- @param callback fun(success: boolean, attribute: any)
function CClients.requestPrivateAttribute(clientID, privateAttribute, callback)
	local attributes = clientsPublicAttributes[clientID]
	local value = attributes[privateAttribute]

	if value ~= nil then
		callback(true, value)
	else
		local requestID = privateAttributeRequestLatestID + 1
		privateAttributeRequestLatestID = requestID

		CClient.sendReliable(GMessage.Type.ClientPrivateAttribute, {
			requestID = requestID,
			clientID = clientID,
			attribute = privateAttribute,
		})

		privateAttributeRequests[requestID] = {
			attribute = privateAttribute,
			callback = callback,
			expireTime = Time.getSystemTime() + 10,
		}
	end
end

--- @warn Do not modify attributes other than local client as possible.
--- @param clientID Network.ClientID
--- @param publicAttribute dr2c.NetworkClientPublicAttribute
--- @param attributeValue any
function CClients.setPublicAttribute(clientID, publicAttribute, attributeValue)
	local attributes = clientsPublicAttributes[clientID]
	if not attributes then
		error(("Client %s does not exists"):format(clientID), 2)
	end

	attributes[publicAttribute] = attributeValue
end

--- @warn Do not modify attributes other than local client as possible.
--- @param clientID Network.ClientID
--- @param privateAttribute dr2c.NetworkClientPrivateAttribute
--- @param attributeValue any
function CClients.setPrivateAttribute(clientID, privateAttribute, attributeValue)
	local attributes = clientsPrivateAttributes[clientID]
	if not attributes then
		error(("Client %s does not exists"):format(clientID), 2)
	end

	attributes[privateAttribute] = attributeValue
end

events:add(N_("CDisconnect"), function(e)
	clientsPublicAttributes = {}
	clientsPrivateAttributes = {}
	privateAttributeRequests = {}
end, N_("ResetClients"), "Reset")

--- @param e dr2c.E.CMessage
events:add(N_("CMessage"), function(e)
	local clientID = e.content.clientID

	if log.canTrace() then
		log.trace(("Received client %s connect message"):format(clientID))
	end

	CClients.addClient(clientID)
end, "ReceiveClientConnect", "Receive", GMessage.Type.ClientConnect)

events:add(N_("CMessage"), function(e)
	local clientID = e.content.clientID

	if log.canTrace() then
		log.trace(("Received client %s disconnect message"):format(clientID))
	end

	CClients.removeClient(clientID)
end, "ReceiveClientDisconnect", "Receive", GMessage.Type.ClientDisconnect)

local function initializeClientsAttributes(serverClientsPublicAttributes)
	for clientID, clientPublicAttributes in pairs(serverClientsPublicAttributes) do
		if not CClients.hasClient(clientID) then
			CClients.addClient(clientID)

			clientsPublicAttributes[clientID] = clientPublicAttributes
		end
	end
end

--- @param e dr2c.E.CMessage
events:add(N_("CMessage"), function(e)
	initializeClientsAttributes(e.content)
end, "InitializeClientsAttributes", "Receive", GMessage.Type.Clients)

--- @param e dr2c.E.CMessage
local function receiveClientAttribute(e, validate, clientsAttributes)
	local content = e.content
	if type(content) ~= "table" then
		return
	end

	local clientID = content.clientID
	local attribute = content.attribute
	local value = content.value
	if type(clientID) ~= "number" or type(attribute) ~= "number" or not validate(attribute, value) then
		return
	end

	local attributes = clientsAttributes[clientID]
	if attributes then
		attributes[attribute] = value
	end

	if Utility.canBeIndex(content.requestID) then
		privateAttributeRequests[content.requestID] = nil
	end
end

--- @param e dr2c.E.CMessage
events:add(N_("CMessage"), function(e)
	receiveClientAttribute(e, GClient.validatePublicAttribute, clientsPublicAttributes)
end, "ReceiveClientPublicAttribute", "Receive", GMessage.Type.ClientPublicAttribute)

--- @param e dr2c.E.CMessage
events:add(N_("CMessage"), function(e)
	receiveClientAttribute(e, GClient.validatePrivateAttribute, clientsPrivateAttributes)
end, "ReceiveClientPrivateAttribute", "Receive", GMessage.Type.ClientPrivateAttribute)

--- @param e dr2c.E.CMessage
events:add(N_("CMessage"), function(e)
	local content = e.content
	if type(content) ~= "table" or type(content.clientID) ~= "number" or type(content.attribute) ~= "number" then
		return
	end

	local requests = privateAttributeRequests[e.content.clientID]
	if requests then
		requests[content.attribute] = nil
	end
end, "ClearPrivateAttributeCache", "Receive", GMessage.Type.ClientPrivateAttributeChanged)

events:add(N_("CUpdate"), function(e)
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

return CClients
