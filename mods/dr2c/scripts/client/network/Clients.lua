local String = require("tudov.String")
local Enum = require("tudov.Enum")
local Utility = require("tudov.Utility")

local GThrottle = require("dr2c.shared.utils.Throttle")
local CNetwork = require("dr2c.client.misc.Network")
local GClient = require("dr2c.shared.network.Client")
local GMessage = require("dr2c.shared.network.Message")

--- @class dr2c.CClients
local CClients = {}

--- @type table<number, Network.ClientID>
local secretToken2ClientID = {}
secretToken2ClientID = persist("secretToken2ClientID", {}, function()
	return secretToken2ClientID
end)

--- @type table<Network.ClientID, table<dr2c.ClientPublicAttribute, any>>
local clientsPublicAttributes
clientsPublicAttributes = persist("clientsPublicAttributes", {}, function()
	return clientsPublicAttributes
end)

--- @type table<Network.ClientID, table<dr2c.ClientPublicAttribute, any>>
local clientsPrivateAttributes
clientsPrivateAttributes = persist("clientsPrivateAttributes", {}, function()
	return clientsPrivateAttributes
end)

--- @type table<number, {callback: fun(success: boolean, attribute: any), expireTime: integer}>
local privateAttributeRequests
privateAttributeRequests = persist("privateAttributeRequests", {}, function()
	return privateAttributeRequests
end)

local privateAttributeRequestLatestID
privateAttributeRequestLatestID = persist("privateAttributeRequestLatestID", 0, function()
	return privateAttributeRequestLatestID
end)

--- @param clientID Network.ClientID
--- @return boolean
function CClients.hasClient(clientID)
	return clientsPublicAttributes[clientID] ~= nil
end

--- @param clientID Network.ClientID
function CClients.addClient(clientID)
	clientsPublicAttributes[clientID] = {}
	clientsPrivateAttributes[clientID] = {}
end

--- @param clientID Network.ClientID
function CClients.removeClient(clientID)
	clientsPublicAttributes[clientID] = nil
	clientsPrivateAttributes[clientID] = nil
end

--- @param clientPublicAttribute dr2c.ClientPublicAttribute
--- @nodiscard
function CClients.getPublicAttribute(clientID, clientPublicAttribute)
	local attributes = clientsPublicAttributes[clientID]

	local attribute = attributes[clientPublicAttribute]

	return attribute
end

--- @param clientID Network.ClientID
--- @param clientPrivateAttribute dr2c.ClientPrivateAttribute
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
			callback = callback,
			expireTime = Time.getSystemTime() + 30,
		}
	end
end

events:add(N_("CDisconnect"), function(e)
	clientsPublicAttributes = {}
	clientsPrivateAttributes = {}
	privateAttributeRequests = {}
end, N_("ClientResetClients"), "Reset")

events:add(N_("CMessage"), function(e)
	local content = e.content
	if type(content) ~= "table" then
		return
	end

	local clientID = content.clientID
	local attribute = content.attribute
	local value = content.value
	if
		type(clientID) ~= "number"
		or type(attribute) ~= "number"
		or not GClient.validatePrivateAttribute(attribute, value)
	then
		return
	end

	local attributes = clientsPrivateAttributes[clientID]
	if attributes then
		attributes[attribute] = value
	end

	if Utility.isIndexKey(content.requestID) then
		privateAttributeRequests[content.requestID] = nil
	end
end, "CReceivePrivateAttribute", "Receive", GMessage.Type.ClientPrivateAttribute)

events:add(N_("CMessage"), function(e)
	local content = e.content
	if type(content) ~= "table" or type(content.clientID) ~= "number" or type(content.attribute) ~= "number" then
		return
	end

	local requests = privateAttributeRequests[e.content.clientID]
	if requests then
		requests[content.attribute] = nil
	end
end, "CClearPrivateAttributeCache", "Receive", GMessage.Type.ClientPrivateAttributeChanged)

local throttleClientUpdateClientsPrivateAttributeRequests = GThrottle.newTime(1)

events:add(N_("CUpdate"), function(e)
	if throttleClientUpdateClientsPrivateAttributeRequests() then
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
end, "ClientUpdateClientsPrivateAttributeRequests", "Network")

return CClients
