--[[
-- @module dr2c.server.network.Clients
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("tudov.Enum")
local Table = require("tudov.Table")

local GClient = require("dr2c.shared.network.Client")
local GDisconnection = require("dr2c.shared.network.Disconnection")
local GMessage = require("dr2c.shared.network.Message")
local SServer = require("dr2c.server.network.Server")

--- @class dr2c.ServerUnverifiedClient
--- @field clientID Network.ClientID
--- @field expireTime number

--- @class dr2c.SClients
local SClients = {}

--- @type dr2c.ServerUnverifiedClient[]
local unverifiedClients = {}
unverifiedClients = persist("unverifiedClients", function()
	return unverifiedClients
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

--- @param e dr2c.E.ServerConnect
events:add(N_("SConnect"), function(e)
	local clientID = e.clientID

	unverifiedClients[#unverifiedClients + 1] = {
		clientID = clientID,
		expireTime = Time.getSystemTime() + 15,
	}

	clientsPublicAttributes[clientID] = {
		[GClient.PublicAttribute.State] = GClient.State.Verifying,
	}
	clientsPrivateAttributes[clientID] = {}
end, "AddUnverifiedClient", "Clients")

local verifyPublicAttributes = {
	GClient.PublicAttribute.ID,
}

local verifyPrivateAttributes = {
	GClient.PrivateAttribute.SecretToken,
}

-- local function setAttribute()
--
-- end

local function unverifiedClientEquals(entry, _, clientID)
	return entry.clientID == clientID
end

--- Send server clients attributes to lately verified client.
--- @param clientID Network.ClientID
local function onVerifiedClient(clientID)
	Table.listRemoveFirstIf(unverifiedClients, unverifiedClientEquals, clientID)

	local clients
	for existedClientID, publicAttributes in ipairs(clientsPublicAttributes) do
		if existedClientID ~= clientID then
			clients = clients or {}
			clients[existedClientID] = publicAttributes
		end
	end

	if clients then
		SServer.sendReliable(clientID, GMessage.Type.Clients, clients)
	end
end

--- @param e dr2c.E.ServerMessage
events:add(N_("SMessage"), function(e)
	local clientID = e.clientID

	local attributes = clientsPublicAttributes[clientID]
	if not attributes then
		return
	end

	local attribute = e.content.attribute

	local trait = GClient.getPublicAttributeTrait(attribute)
	if trait == GClient.AttributeTrait.Authoritative then
		return
	end

	local value = e.content.value
	if not GClient.validatePublicAttribute(attribute, value) then
		return
	end

	if attributes[GClient.PublicAttribute.State] == GClient.State.Verifying then
		if attribute == GClient.PublicAttribute.State then
			if value ~= GClient.State.Verified then
				return
			end

			onVerifiedClient(clientID)
		end
	elseif trait == GClient.AttributeTrait.Validation then
		return
	end

	if log.canTrace() then
		local key = Enum.resolveValue(GClient.PublicAttribute, attribute)
		log.trace(("Set client %s public attribute: <%s>%s = %s"):format(clientID, attribute, key, value))
	end

	attributes[e.content.attribute] = e.content.value

	e.broadcast = {
		clientID = clientID,
		attribute = attribute,
		value = value,
	}
end, "ReceiveClientPublicAttribute", "Receive", GMessage.Type.ClientPublicAttribute)

local function updateUnverifiedClientsFunc(entry)
	if Time.getSystemTime() > entry.expireTime then
		entry.expireTime = entry.expireTime + 5

		if log.canTrace() then
			log.trace(("Disconnect client %s due to verification timeout"):format(entry.clientID))
		end

		SServer.disconnect(entry.clientID, GDisconnection.Code.Timeout)

		return true
	end
end

events:add(N_("SUpdate"), function(e)
	Table.listRemoveIf(unverifiedClients, updateUnverifiedClientsFunc)
end, N_("UpdateUnverifiedClients"), "Network", nil, -1)

return SClients
