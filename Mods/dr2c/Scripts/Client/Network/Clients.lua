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

local Enum = require("TE.Enum")
local List = require("TE.List")
local Utility = require("TE.Utility")
local inspect = require("inspect")

local CNetworkClient = require("dr2c.Client.Network.Client")
local CNetworkRPC = require("dr2c.Client.Network.RPC")
local GUtilsThrottle = require("dr2c.Shared.Utils.Throttle")
local GNetworkClient = require("dr2c.Shared.Network.Client")
local GNetworkClients = require("dr2c.Shared.Network.Clients")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")

--- @class dr2c.ClientPrivateAttributeGettingRequest
--- @field attribute dr2c.ClientPrivateAttribute
--- @field callback fun(success: boolean, attributeValue: any, reason?: dr2c.NetworkReasonID)
--- @field expireTime number

--- 不要使用`CNetworkClients.getPrivateAttribute`，读取的值是本地缓存的，很可能是空或过时的值，更安全的做法是使用`CNetworkClients.requestPrivateAttribute`。若希望访问本地私有属性，请使用`CNetworkClient.getPrivateAttribute`。
--- 不要使用`CNetworkClients.setPublicAttribute`、`CNetworkClients.setPrivateAttribute`修改非本地客户端的属性，这会导致与服务器数据不同步。若要修改本地属性最好使用`CNetworkClient.setPublicAttribute`、`CNetworkClient.setPrivateAttribute`
--- @class dr2c.CNetworkClients : dr2c.MNetworkClients
local CNetworkClients = GNetworkClients.new()

local MNetworkClients_addClient = CNetworkClients.addClient
local MNetworkClients_removeClient = CNetworkClients.removeClient
local MNetworkClients_setPublicAttribute = CNetworkClients.setPublicAttribute
local MNetworkClients_setPrivateAttribute = CNetworkClients.setPrivateAttribute

--- @type table<number, dr2c.ClientPrivateAttributeGettingRequest>
local privateAttributeGettingRequests = {}

--- @type table<number, TE.Network.ClientID>
local secretToken2ClientID = {}

privateAttributeGettingRequests = persist("privateAttributeRequests", function()
	return privateAttributeGettingRequests
end)
secretToken2ClientID = persist("secretToken2ClientID", function()
	return secretToken2ClientID
end)

CNetworkClients.eventCClientAdded = TE.events:new(N_("CClientAdded"), {
	"Add",
	"PlayerInputBuffers",
	"Cache",
})

CNetworkClients.eventCClientRemoved = TE.events:new(N_("CClientRemoved"), {
	"Remove",
	"PlayerInputBuffers",
	"Cache",
})

CNetworkClients.eventCClientPublicAttribute = TE.events:new(N_("CClientPublicAttribute"), {
	"Room",
	"Set",
	"Snapshot",
}, Enum.eventKeys(GNetworkClient.PublicAttribute))

CNetworkClients.eventCClientPrivateAttribute = TE.events:new(N_("CClientPrivateAttribute"), {
	"Set",
}, Enum.eventKeys(GNetworkClient.PrivateAttribute))

CNetworkClients.rawSetPublicAttribute = MNetworkClients_setPublicAttribute
CNetworkClients.rawSetPrivateAttribute = MNetworkClients_setPrivateAttribute

--- @param clientID TE.Network.ClientID
function CNetworkClients.addClient(clientID)
	--- @class dr2c.E.CClientAdded
	local e = {
		clientID = clientID,
	}

	TE.events:invoke(CNetworkClients.eventCClientAdded, e)
end

--- @param e dr2c.E.CClientAdded
TE.events:add(CNetworkClients.eventCClientAdded, function(e)
	MNetworkClients_addClient(e.clientID)
end, "AddClient", "Add")

--- @param clientID TE.Network.ClientID
function CNetworkClients.removeClient(clientID)
	--- @class dr2c.E.CClientRemoved
	local e = {
		clientID = clientID,
	}

	TE.events:invoke(CNetworkClients.eventCClientRemoved, e)
end

--- @param e dr2c.E.CClientRemoved
TE.events:add(CNetworkClients.eventCClientRemoved, function(e)
	MNetworkClients_removeClient(e.clientID)
end, "RemoveClient", "Remove")

--- @param clientID TE.Network.ClientID
--- @param publicAttribute dr2c.ClientPublicAttribute
--- @param attributeValue Serializable
function CNetworkClients.setPublicAttribute(clientID, publicAttribute, attributeValue)
	--- @class dr2c.E.CClientPublicAttribute
	local e = {
		clientID = clientID,
		attribute = publicAttribute,
		value = attributeValue,
	}

	TE.events:invoke(CNetworkClients.eventCClientPublicAttribute, e, publicAttribute)
end

--- @param e dr2c.E.CClientPublicAttribute
TE.events:add(CNetworkClients.eventCClientPublicAttribute, function(e)
	MNetworkClients_setPublicAttribute(e.clientID, e.attribute, e.value)
end)

--- @param clientID TE.Network.ClientID
--- @param privateAttribute dr2c.ClientPrivateAttribute
--- @param attributeValue Serializable
function CNetworkClients.setPrivateAttribute(clientID, privateAttribute, attributeValue)
	--- @class dr2c.E.CClientPrivateAttribute
	local e = {
		clientID = clientID,
		attribute = privateAttribute,
		value = attributeValue,
	}

	TE.events:invoke(CNetworkClients.eventCClientPrivateAttribute, e, privateAttribute)
end

--- @param e dr2c.E.CClientPrivateAttribute
TE.events:add(CNetworkClients.eventCClientPrivateAttribute, function(e)
	MNetworkClients_setPrivateAttribute(e.clientID, e.attribute, e.value)
end)

--- @param content table?
--- @param callback? fun(clientID: TE.Network.ClientID, attribute: dr2c.ClientPrivateAttribute, success: boolean, value?: any)
local function requestPrivateAttributeRPCCallback(content, clientID, attribute, callback)
	if content then
		local fields = GNetworkMessageFields.CClientGetPrivateAttribute
		local value = content[fields.value]

		CNetworkClients.setPrivateAttribute(clientID, attribute, value)

		if callback then
			callback(clientID, attribute, true, value)
		end
	else
		if callback then
			callback(clientID, attribute, false)
		end
	end
end

--- 向服务器申请某个客户端的私有属性
--- @param clientID TE.Network.ClientID
--- @param privateAttribute dr2c.ClientPrivateAttribute
--- @param callback? fun(clientID: TE.Network.ClientID, attribute: dr2c.ClientPrivateAttribute, success?: boolean, value?: any)
function CNetworkClients.requestPrivateAttribute(clientID, privateAttribute, callback, ...)
	local fields = GNetworkMessageFields.SClientGetPrivateAttribute

	return CNetworkRPC.sendReliable(GNetworkMessage.Type.ClientGetPrivateAttribute, {
		[fields.clientID] = clientID,
		[fields.attribute] = privateAttribute,
	}, nil, requestPrivateAttributeRPCCallback, nil, clientID, privateAttribute, callback)
end

TE.events:add(N_("CDisconnect"), function(e)
	CNetworkClients.clear()
end, N_("ResetClients"), "Reset")

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	if type(e.content) ~= "table" then
		return
	end

	local fields = GNetworkMessageFields.CClientConnect

	local clientID = e.content[fields.clientID]
	if not clientID then
		return
	end

	if log.canTrace() then
		log.trace(("Received client %s connect message"):format(clientID))
	end

	CNetworkClients.addClient(clientID)
end, "ReceiveClientConnect", "Receive", GNetworkMessage.Type.CClientConnect)

TE.events:add(N_("CMessage"), function(e)
	if type(e.content) ~= "table" then
		return
	end

	local fields = GNetworkMessageFields.CClientConnect

	local clientID = e.content[fields.clientID]
	if not clientID then
		return
	end

	if log.canTrace() then
		log.trace(("Received client %s disconnect message"):format(clientID))
	end

	CNetworkClients.removeClient(clientID)
end, "ReceiveClientDisconnect", "Receive", GNetworkMessage.Type.CClientDisconnect)

local function initializeClientsPublicAttributes(serverClientsPublicAttributes)
	for clientID, clientPublicAttributes in pairs(serverClientsPublicAttributes) do
		if not CNetworkClients.hasClient(clientID) then
			CNetworkClients.addClient(clientID)
		end

		for attribute, value in pairs(clientPublicAttributes) do
			MNetworkClients_setPublicAttribute(clientID, attribute, value)
		end
	end
end

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	local clientsPublicAttributes = e.content

	if type(clientsPublicAttributes) ~= "table" then
		if log.canDebug() then
			log.debug(
				"Received clients' public attributes, ignoring because `content` is not a table storing clients' public attributes"
			)
		end

		return
	end

	if log.canDebug() then
		log.debug(("Received clients' public attributes, initializing: %s"):format(inspect(clientsPublicAttributes)))
	end

	initializeClientsPublicAttributes(clientsPublicAttributes)
end, "InitializeClientsPublicAttributes", "Receive", GNetworkMessage.Type.CClients)

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	if type(e.content) ~= "table" then
		return
	end

	local fields = GNetworkMessageFields.CClientSetPublicAttribute
	local clientID = e.content[fields.clientID]
	local attribute = e.content[fields.attribute]
	local value = e.content[fields.value]
	if
		type(clientID) ~= "number"
		or type(attribute) ~= "number"
		or not GNetworkClient.validatePublicAttribute(attribute, value)
	then
		return
	end

	local attributes = CNetworkClients.getClientsPublicAttributes()[clientID]
	if not attributes then
		-- TODO log reason

		return
	end

	attributes[attribute] = value
end, "ReceiveClientPublicAttribute", "Receive", GNetworkMessage.Type.CClientSetPublicAttribute)

return CNetworkClients
