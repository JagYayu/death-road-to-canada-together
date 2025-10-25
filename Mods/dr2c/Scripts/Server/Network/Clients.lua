--[[
-- @module dr2c.Server.Network.Clients
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")
local EnumFlag = require("TE.EnumFlag")
local List = require("TE.List")

local GNetworkClient = require("dr2c.Shared.Network.Client")
local GNetworkClients = require("dr2c.Shared.Network.Clients")
local GNetworkDisconnection = require("dr2c.Shared.Network.Disconnection")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local SNetworkServer = require("dr2c.Server.Network.Server")

--- @class dr2c.ServerUnverifiedClient
--- @field clientID TE.Network.ClientID
--- @field expireTime number

--- @class dr2c.SNetworkClients : dr2c.MNetworkClients
local SNetworkClients = GNetworkClients.new()

local MNetworkClients_setPublicAttribute = SNetworkClients.setPublicAttribute
local MNetworkClients_setPrivateAttribute = SNetworkClients.setPrivateAttribute

--- @type dr2c.ServerUnverifiedClient[]
local unverifiedClients = {}

--- @type number
local verifyingTimeLimit = 15

unverifiedClients = persist("unverifiedClients", function()
	return unverifiedClients
end)
verifyingTimeLimit = persist("verifyingTimeLimit", function()
	return verifyingTimeLimit
end)

--#region Misc Functions

local function stringClientNotExists(clientID)
	return ("Client %d does not exist in server"):format(clientID)
end

local function logGetAuthoritativeClientDuplicated(clients) end

local function logReceiveClientAttributeInvalidClient(what, clientID)
	if log.canInfo() then
		log.info(
			("Received client %d's %s attribute, ignoring because client %d has not been added to client list."):format(
				clientID,
				what,
				clientID
			)
		)
	end
end

local function logReceiveClientAttributeInvalidAttribute(what, clientID, attribute)
	if log.canInfo() then
		log.info(
			("Received client %d's %s attribute, ignoring because attribute '%s' is invalid."):format(
				clientID,
				what,
				attribute
			)
		)
	end
end

local function logReceiveClientAttributeInvalidValue(what, clientID, attribute, value)
	if log.canInfo() then
		log.info(
			("Received client %d's %s attribute, ignoring because attribute %s's value '%s' is invalid."):format(
				clientID,
				what,
				attribute,
				value
			)
		)
	end
end

local function logReceiveClientAttributeDisconnectedClient(what, clientID)
	if log.canInfo() then
		log.info(
			("Received client %d's %s attribute, ignoring because client is on disconnected state."):format(
				clientID,
				what
			)
		)
	end
end

local function logReceiveClientAttributeValidationAttribute(what, clientID, attribute)
	if log.canInfo() then
		log.info(
			("Received client %d's %s attribute, ignoring because attribute %s cannot modify outside of validation state."):format(
				clientID,
				what,
				attribute
			)
		)
	end
end

local function logReceiveClientAttributeAuthoritativeAttribute(what, clientID, attribute)
	if log.canInfo() then
		log.info(
			("Received client %d's %s attribute, ignoring because attribute %s is authoritative."):format(
				clientID,
				what,
				attribute
			)
		)
	end
end

local function logReceiveClientAttributeUnknownStateClient(what, clientID)
	if log.canInfo() then
		log.info(
			("Received client %d's %s attribute, ignoring because client is on an unknown state."):format(
				clientID,
				what
			)
		)
	end
end

local function logReceiveClientAttributeSuccessful(what, clientID, attribute, value)
	if log.canInfo() then
		log.info(
			("Received client %d's %s attribute, attribute %s, value %s."):format(clientID, what, attribute, value)
		)
	end
end

local function logReceiveClientAttributeFailedToVerify(what, clientID, reason)
	if log.canInfo() then
		log.info(("Received client %d's %s attribute, but failed to verify, reason %d."):format(clientID, what, reason))
	end
end

--#endregion

SNetworkClients.eventSClientPublicAttribute = TE.events:new(N_("SClientPublicAttribute"), {
	"Room",
	"Set",
	"Network",
}, Enum.eventKeys(GNetworkClient.PublicAttribute))

SNetworkClients.eventSClientPrivateAttribute = TE.events:new(N_("SClientPrivateAttribute"), {
	"Set",
}, Enum.eventKeys(GNetworkClient.PrivateAttribute))

SNetworkClients.eventSClientVerified = TE.events:new(N_("SClientVerified"), {
	"Host",
	"Clients",
	"Rooms",
	"ForthSession",
	"WorldSession",
	"PlayerInputBuffers",
	"Snapshot",
})

SNetworkClients.rawSetPublicAttribute = MNetworkClients_setPublicAttribute
SNetworkClients.rawSetPrivateAttribute = MNetworkClients_setPrivateAttribute

--- @param clientID TE.Network.ClientID
--- @param publicAttribute dr2c.ClientPublicAttribute
--- @param attributeValue? Serializable
function SNetworkClients.setPublicAttribute(clientID, publicAttribute, attributeValue)
	--- @class dr2c.E.SClientPublicAttribute
	local e = {
		clientID = clientID,
		attribute = publicAttribute,
		value = attributeValue,
	}

	TE.events:invoke(SNetworkClients.eventSClientPublicAttribute, e, publicAttribute, EEventInvocation.CacheHandlers)
end

--- @param e dr2c.E.SClientPublicAttribute
TE.events:add(SNetworkClients.eventSClientPublicAttribute, function(e)
	MNetworkClients_setPublicAttribute(e.clientID, e.attribute, e.value)
end, "SetAttribute", "Set")

--- @param e dr2c.E.SClientPublicAttribute
TE.events:add(SNetworkClients.eventSClientPublicAttribute, function(e)
	local fields = GNetworkMessageFields.CClientSetPublicAttribute

	SNetworkServer.broadcastReliable(GNetworkMessage.Type.CClientSetPublicAttribute, {
		[fields.clientID] = e.clientID,
		[fields.attribute] = e.attribute,
		[fields.value] = e.value,
	})
end, "BroadcastReliable", "Network")

--- @param clientID TE.Network.ClientID
--- @param privateAttribute dr2c.ClientPrivateAttribute
--- @param attributeValue? Serializable
function SNetworkClients.setPrivateAttribute(clientID, privateAttribute, attributeValue)
	--- @class dr2c.E.SClientPrivateAttribute
	local e = {
		clientID = clientID,
		attribute = privateAttribute,
		value = attributeValue,
	}

	TE.events:invoke(SNetworkClients.eventSClientPrivateAttribute, e, privateAttribute, EEventInvocation.CacheHandlers)
end

--- @param e dr2c.E.SClientPrivateAttribute
TE.events:add(SNetworkClients.eventSClientPrivateAttribute, function(e)
	MNetworkClients_setPrivateAttribute(e.clientID, e.attribute, e.value)
end, "SetAttribute", "Set")

--- Verify the client.
--- 验证客户端是否合法
--- @param clientID TE.Network.ClientID
--- @return boolean verified
--- @return dr2c.NetworkReasonID? reason
function SNetworkClients.verifyClient(clientID)
	-- TODO

	return true
end

--- @param entry table
--- @param _ integer
--- @param clientID TE.Network.ClientID
--- @return boolean
local function conditionUnverifiedClientEquals(entry, _, clientID)
	return entry.clientID == clientID
end

--- @param clientID TE.Network.ClientID
local function onClientVerified(clientID)
	List.removeFirstIfV(unverifiedClients, conditionUnverifiedClientEquals, clientID)

	--- @class dr2c.E.SClientVerified
	--- @field clientID TE.Network.ClientID
	local e = {
		clientID = clientID,
	}

	TE.events:invoke(SNetworkClients.eventSClientVerified, e)
end

TE.events:add(SNetworkClients.eventSClientVerified, function(e)
	local clientID = e.clientID

	local hasHostClient
	for _, publicAttributes in pairs(SNetworkClients.getClientsPublicAttributes()) do
		local permissions = publicAttributes[GNetworkClient.PublicAttribute.Permissions]
		--- @cast permissions integer
		if permissions and EnumFlag.hasAll(permissions, GNetworkClient.Permissions.Host) then
			hasHostClient = true
		end
	end

	-- 若服务器内没有任何主机客户端，即拥有所有权限的客户端，则将主机属性赋予给该客户端
	if not hasHostClient then
		local permissions = SNetworkClients.getPublicAttribute(clientID, GNetworkClient.PublicAttribute.Permissions)
		--- @cast permissions integer
		permissions = EnumFlag.mask(permissions, GNetworkClient.Permissions.Host)
		SNetworkClients.setPublicAttribute(clientID, GNetworkClient.PublicAttribute.Permissions, permissions)
	end
end, "AssignDefaultHost", "Host")

TE.events:add(SNetworkClients.eventSClientVerified, function(e)
	local clientID = e.clientID

	local clients
	for existingClientID, publicAttributes in pairs(SNetworkClients.getClientsPublicAttributes()) do
		if existingClientID ~= clientID then
			clients = clients or {}
			clients[existingClientID] = publicAttributes
		end
	end

	--- Send server clients public attributes to lately verified client.
	if clients then
		SNetworkServer.sendReliable(clientID, GNetworkMessage.Type.CClients, clients)
	end
end, "SendClients", "Clients")

--- @param clientID TE.Network.ClientID
--- @param attribute? dr2c.ClientPublicAttribute
--- @param value? any
--- @return boolean received
--- @return dr2c.NetworkReasonID? reason
function SNetworkClients.receivePublicAttribute(clientID, attribute, value)
	local what = "public"

	if not (attribute and Enum.hasValue(GNetworkClient.PublicAttribute, attribute)) then
		logReceiveClientAttributeInvalidAttribute(what, clientID, attribute)

		return false
	end

	if not GNetworkClient.validatePublicAttribute(attribute, value) then
		logReceiveClientAttributeInvalidValue(what, clientID, attribute, value)

		return false
	end

	local attributes = SNetworkClients.getPublicAttributes(clientID)
	local state = attributes[GNetworkClient.PublicAttribute.State]
	local trait = GNetworkClient.getPublicAttributeTrait(attribute)

	if state == GNetworkClient.State.Disconnected then
		logReceiveClientAttributeDisconnectedClient(what, clientID)

		return false
	elseif state == GNetworkClient.State.Verifying then
		if trait == GNetworkClient.AttributeTrait.Editable then
			-- pass
		elseif trait == GNetworkClient.AttributeTrait.Validation then
			-- pass
		elseif trait == GNetworkClient.AttributeTrait.Authoritative then
			logReceiveClientAttributeAuthoritativeAttribute(what, clientID, attribute)

			return false
		else
			error(("Unexpected client attribute trait: %s"):format(trait))
		end
	elseif state == GNetworkClient.State.Verified then
		if trait == GNetworkClient.AttributeTrait.Editable then
			-- pass
		elseif trait == GNetworkClient.AttributeTrait.Validation then
			logReceiveClientAttributeValidationAttribute(what, clientID, attribute)

			return false
		elseif trait == GNetworkClient.AttributeTrait.Authoritative then
			logReceiveClientAttributeAuthoritativeAttribute(what, clientID, attribute)

			return false
		else
			error(("Unexpected client attribute trait: %s"):format(trait))
		end
	else
		logReceiveClientAttributeUnknownStateClient(what, clientID)

		return false
	end

	if attribute == GNetworkClient.PublicAttribute.State and value == GNetworkClient.State.Verified then
		local verified, reason = SNetworkClients.verifyClient(clientID)
		if not verified then
			logReceiveClientAttributeFailedToVerify(what, clientID, reason)

			return false, reason
		end

		logReceiveClientAttributeSuccessful(what, clientID, attribute, value)

		SNetworkClients.setPublicAttribute(clientID, attribute, value)

		onClientVerified(clientID)
	else
		logReceiveClientAttributeSuccessful(what, clientID, attribute, value)

		attributes[attribute] = value
	end

	return true
end

--- @param clientID TE.Network.ClientID
--- @param attribute? dr2c.ClientPrivateAttribute
--- @param value? any
--- @return boolean received
--- @return dr2c.NetworkReasonID? reason
function SNetworkClients.receivePrivateAttribute(clientID, attribute, value)
	local what = "private"

	local attributes = clientsPrivateAttributes[clientID]
	if not attributes then
		logReceiveClientAttributeInvalidClient(what, clientID)

		return false
	end

	if not (attribute and Enum.hasValue(GNetworkClient.PrivateAttribute, attribute)) then
		logReceiveClientAttributeInvalidAttribute(what, clientID, attribute)

		return false
	end

	if not GNetworkClient.validatePrivateAttribute(attribute, value) then
		logReceiveClientAttributeInvalidValue(what, clientID, attribute, value)

		return false
	end

	local trait = GNetworkClient.getPrivateAttributeTrait(attribute)
	if trait == GNetworkClient.AttributeTrait.Editable then
		-- pass
	elseif trait == GNetworkClient.AttributeTrait.Validation then
		if
			clientsPublicAttributes[clientID][GNetworkClient.PublicAttribute.State] ~= GNetworkClient.State.Verifying
		then
			logReceiveClientAttributeValidationAttribute(what, clientID, attribute)

			return false
		else
			-- pass
		end
	elseif trait == GNetworkClient.AttributeTrait.Authoritative then
		logReceiveClientAttributeAuthoritativeAttribute(what, clientID, attribute)

		return false
	else
		error(("Unexpected client attribute trait: %s"):format(trait))
	end

	logReceiveClientAttributeSuccessful(what, clientID, attribute, value)

	SNetworkClients.setPrivateAttribute(clientID, attribute, value)

	return true
end

--- @param e dr2c.E.SConnect
TE.events:add(N_("SConnect"), function(e)
	SNetworkClients.addClient(e.clientID)

	SNetworkClients.setPublicAttribute(e.clientID, GNetworkClient.PublicAttribute.State, GNetworkClient.State.Verifying)

	unverifiedClients[#unverifiedClients + 1] = {
		clientID = e.clientID,
		expireTime = Time.getSystemTime() + 15,
	}
end, "AddUnverifiedClient", "Clients")

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	local clientID = e.clientID
	local fields = GNetworkMessageFields.SClientSetPublicAttribute
	local attribute = e.content[fields.attribute]
	local value = e.content[fields.value]

	local received, reason = SNetworkClients.receivePublicAttribute(clientID, attribute, value)
	if received then
		fields = GNetworkMessageFields.CClientSetPublicAttribute
		SNetworkServer.broadcastReliable(GNetworkMessage.Type.CClientSetPublicAttribute, {
			[fields.clientID] = clientID,
			[fields.attribute] = attribute,
			[fields.value] = value,
		})
	end
end, "ReceiveClientPublicAttribute", "Receive", GNetworkMessage.Type.SClientSetPublicAttribute)

TE.events:add(N_("SMessage"), function(e)
	local clientID = e.clientID
	local fields = GNetworkMessageFields.SClientSetPrivateAttribute
	local attribute = e.content[fields.attribute]
	local value = e.content[fields.value]
	local requestID = e.content[fields.requestID]

	local received = SNetworkClients.receivePrivateAttribute(clientID, attribute, value)
	if received then
		fields = GNetworkMessageFields.CClientSetPrivateAttribute
		SNetworkServer.sendReliable(clientID, GNetworkMessage.Type.CClientSetPrivateAttribute, {
			[fields.clientID] = requestID,
			[fields.attribute] = attribute,
			[fields.value] = value,
		})
	end
end, "ReceiveClientPrivateAttribute", "Receive", GNetworkMessage.Type.SClientSetPrivateAttribute)

local function updateUnverifiedClientsFunc(entry)
	if Time.getSystemTime() > entry.expireTime then
		entry.expireTime = entry.expireTime + 5

		if log.canTrace() then
			log.trace(("Disconnect client %s due to verification timeout"):format(entry.clientID))
		end

		SNetworkServer.disconnect(entry.clientID, GNetworkDisconnection.Code.Timeout)

		return true
	end
end

TE.events:add(N_("SUpdate"), function(e)
	List.removeIfV(unverifiedClients, updateUnverifiedClientsFunc)
end, N_("UpdateUnverifiedClients"), "Network")

return SNetworkClients
