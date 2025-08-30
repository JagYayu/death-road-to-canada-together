local String = require("tudov.String")
local Enum = require("tudov.Enum")
local Utility = require("tudov.Utility")

local CNetwork = require("dr2c.client.misc.Network")
local GThrottle = require("dr2c.shared.utils.Throttle")
local GClient = require("dr2c.shared.network.Client")
local GMessage = require("dr2c.shared.network.Message")

--- @class dr2c.ClientPrivateRequestEntry
--- @field attribute dr2c.GClient.PrivateAttribute
--- @field callback fun(success: boolean, attribute: any)
--- @field expireTime number

--- @class dr2c.CClients
local CClients = {}

--- @type table<number, Network.ClientID>
local secretToken2ClientID = {}
secretToken2ClientID = persist("secretToken2ClientID", function()
	return secretToken2ClientID
end)

--- @type table<Network.ClientID, table<dr2c.GClient.PublicAttribute, any>>
local clientsPublicAttributes = {}
clientsPublicAttributes = persist("clientsPublicAttributes", function()
	return clientsPublicAttributes
end)

--- @type table<Network.ClientID, table<dr2c.GClient.PublicAttribute, any>>
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
end

--- @param clientID Network.ClientID
function CClients.removeClient(clientID)
	if not CClients.hasClient(clientID) then
		error(("Client %s already does not exist"):format(clientID), 2)
	end

	clientsPublicAttributes[clientID] = nil
	clientsPrivateAttributes[clientID] = nil
end

--- @param clientID Network.ClientID
--- @param clientPublicAttribute dr2c.GClient.PublicAttribute
--- @return any?
--- @nodiscard
function CClients.getPublicAttribute(clientID, clientPublicAttribute)
	local attributes = clientsPublicAttributes[clientID]
	if attributes then
		return attributes[clientPublicAttribute]
	end
end

--- @param clientID Network.ClientID
--- @param clientPrivateAttribute dr2c.GClient.PrivateAttribute
--- @param callback fun(success: boolean, attribute: any)
function CClients.requestPrivateAttribute(clientID, clientPrivateAttribute, callback)
	local attributes = clientsPublicAttributes[clientID]
	local value = attributes[clientPrivateAttribute]

	if value ~= nil then
		callback(true, value)
	else
		privateAttributeRequestLatestID = privateAttributeRequestLatestID + 1
		local requestID = privateAttributeRequestLatestID

		CNetwork.sendMessage(GMessage.pack(GMessage.Type.ClientPrivateAttribute, {
			requestID = requestID,
			clientID = clientID,
			attribute = clientPrivateAttribute,
		}))

		privateAttributeRequests[requestID] = {
			attribute = clientPrivateAttribute,
			callback = callback,
			expireTime = Time.getSystemTime() + 10,
		}
	end
end

--- @warn Do not modify attributes other than local client as possible.
--- @param clientID Network.ClientID
--- @param clientPublicAttribute dr2c.GClient.PublicAttribute
--- @param attributeValue any
function CClients.setPublicAttribute(clientID, clientPublicAttribute, attributeValue)
	local attributes = clientsPublicAttributes[clientID]
	if not attributes then
		error(("Client %s does not exists"):format(clientID), 2)
	end

	attributes[clientPublicAttribute] = attributeValue
end

--- @warn Do not modify attributes other than local client as possible.
--- @param clientID Network.ClientID
--- @param clientPrivateAttribute dr2c.GClient.PrivateAttribute
--- @param attributeValue any
function CClients.setPrivateAttribute(clientID, clientPrivateAttribute, attributeValue)
	local attributes = clientsPrivateAttributes[clientID]
	if not attributes then
		error(("Client %s does not exists"):format(clientID), 2)
	end

	attributes[clientPrivateAttribute] = attributeValue
end

events:add(N_("CDisconnect"), function(e)
	clientsPublicAttributes = {}
	clientsPrivateAttributes = {}
	privateAttributeRequests = {}
end, N_("ResetClients"), "Reset")

--- @param e dr2c.E.ClientMessage
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

--- @param e dr2c.E.ClientMessage
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

--- @param e dr2c.E.ClientMessage
events:add(N_("CMessage"), function(e)
	receiveClientAttribute(e, GClient.validatePublicAttribute, clientsPublicAttributes)
end, "ReceiveClientPublicAttribute", "Receive", GMessage.Type.ClientPublicAttribute)

--- @param e dr2c.E.ClientMessage
events:add(N_("CMessage"), function(e)
	receiveClientAttribute(e, GClient.validatePrivateAttribute, clientsPrivateAttributes)
end, "ReceiveClientPrivateAttribute", "Receive", GMessage.Type.ClientPrivateAttribute)

--- @param e dr2c.E.ClientMessage
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
