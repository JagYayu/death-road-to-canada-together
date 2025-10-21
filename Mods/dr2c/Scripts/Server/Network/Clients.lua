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
local Table = require("TE.Table")
local List = require("TE.List")
local inspect = require("inspect")

local GNetworkClient = require("dr2c.Shared.Network.Client")
local GNetworkClients = require("dr2c.Shared.Network.Clients")
local GNetworkDisconnection = require("dr2c.Shared.Network.Disconnection")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local SNetworkServer = require("dr2c.Server.Network.Server")

--- @class dr2c.ServerUnverifiedClient
--- @field clientID TE.Network.ClientID
--- @field expireTime number

--- @class dr2c.SNetworkClients
local SNetworkClients = {}

--- @type TE.Network.ClientID[]
local clientList = {}

--- @type table<TE.Network.ClientID, table<dr2c.NetworkClientPublicAttribute, any>>
local clientsPublicAttributes = {}

--- @type table<TE.Network.ClientID, table<dr2c.NetworkClientPublicAttribute, any>>
local clientsPrivateAttributes = {}

--- @type dr2c.ServerUnverifiedClient[]
local unverifiedClients = {}

local authoritativeClient

clientList = persist("clientList", function()
	return clientList
end)
clientsPublicAttributes = persist("clientsPublicAttributes", function()
	return clientsPublicAttributes
end)
clientsPrivateAttributes = persist("clientsPrivateAttributes", function()
	return clientsPrivateAttributes
end)
unverifiedClients = persist("unverifiedClients", function()
	return unverifiedClients
end)

--- @param clientID TE.Network.ClientID
--- @return boolean
function SNetworkClients.hasClient(clientID)
	return not not clientsPublicAttributes[clientID]
end

--- 添加一个客户端
--- @param clientID TE.Network.ClientID
--- @param verified boolean?
function SNetworkClients.addClient(clientID, verified)
	if clientsPublicAttributes[clientID] then
		error("Client %d has already been added to server", 2)
	end

	clientList[#clientList + 1] = clientID
	local publicAttributes, privateAttributes = GNetworkClient.initializeClientAttributes(clientID, {}, {})
	clientsPublicAttributes[clientID] = publicAttributes
	clientsPrivateAttributes[clientID] = privateAttributes

	if verified then
		publicAttributes[GNetworkClient.PublicAttribute.State] = GNetworkClient.State.Verified
	else
		publicAttributes[GNetworkClient.PublicAttribute.State] = GNetworkClient.State.Verifying

		unverifiedClients[#unverifiedClients + 1] = {
			clientID = clientID,
			expireTime = Time.getSystemTime() + 15,
		}
	end

	authoritativeClient = nil
end

--- 移除一个客户端
--- @param clientID TE.Network.ClientID
function SNetworkClients.removeClient(clientID)
	if not clientsPublicAttributes[clientID] then
		error("Client %d does not exist in server", 2)
	end

	List.removeFirst(clientList, clientID)
	authoritativeClient = nil
end

--- 获取权威客户端
--- @return TE.Network.ClientID?
--- @nodiscard
function SNetworkClients.getAuthoritativeClient()
	if authoritativeClient ~= nil then
		return authoritativeClient or nil
	end

	local clients = GNetworkClients.getAuthoritativeClients(clientList, clientsPublicAttributes)
	if #clients == 1 then
		authoritativeClient = clients[1]
	elseif #clients == 0 then
		authoritativeClient = false
	elseif log.canWarn() then
		log.warn(("Multiple authoritative clients detected in this server: %s"):format( --
			inspect(clients)
		))
	end
end

--- @param clientID TE.Network.ClientID
--- @return boolean?
function SNetworkClients.isAuthoritativeClient(clientID)
	return SNetworkClients.getAuthoritativeClient() == clientID
end

--- @param e dr2c.E.SConnect
TE.events:add(N_("SConnect"), function(e)
	SNetworkClients.addClient(e.clientID)
end, "AddUnverifiedClient", "Clients")

local verifyPublicAttributes = {
	GNetworkClient.PublicAttribute.ID,
}

local verifyPrivateAttributes = {
	GNetworkClient.PrivateAttribute.SecretToken,
}

--- @param entry table
--- @param _ integer
--- @param clientID TE.Network.ClientID
--- @return boolean
local function conditionUnverifiedClientEquals(entry, _, clientID)
	return entry.clientID == clientID
end

--- Send server clients attributes to lately verified client.
--- @param clientID TE.Network.ClientID
local function onVerifiedClient(clientID)
	List.removeFirstIfV(unverifiedClients, conditionUnverifiedClientEquals, clientID)

	local hasHostClient
	local clients

	for existedClientID, publicAttributes in pairs(clientsPublicAttributes) do
		if existedClientID ~= clientID then
			clients = clients or {}
			clients[existedClientID] = publicAttributes
		end

		local permissions = publicAttributes[GNetworkClient.PublicAttribute.Permissions]
		if permissions and EnumFlag.hasAllV(permissions, GNetworkClient.Permissions.Host) then
			hasHostClient = true
		end
	end

	if not hasHostClient then
		clientsPublicAttributes[clientID][GNetworkClient.PublicAttribute.Permissions] = EnumFlag.mask(
			clientsPublicAttributes[clientID][GNetworkClient.PublicAttribute.Permissions]
				or GNetworkClient.Permissions.None,
			GNetworkClient.Permissions.Host
		)
	end

	if clients then
		SNetworkServer.sendReliable(clientID, GNetworkMessage.Type.Clients, clients)
	end
end

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	local clientID = e.clientID

	local attributes = clientsPublicAttributes[clientID]
	if not attributes then
		return
	end

	local fields = GNetworkMessageFields.ClientPublicAttribute

	local attribute = e.content[fields.attribute]
	if not attribute then
		return
	end

	local trait = GNetworkClient.getPublicAttributeTrait(attribute)
	if trait == GNetworkClient.AttributeTrait.Authoritative then
		return
	end

	local value = e.content[fields.value]
	if not GNetworkClient.validatePublicAttribute(attribute, value) then
		return
	end

	if attributes[GNetworkClient.PublicAttribute.State] == GNetworkClient.State.Verifying then
		if attribute == GNetworkClient.PublicAttribute.State then
			if value ~= GNetworkClient.State.Verified then
				return
			end

			onVerifiedClient(clientID)
		end
	elseif trait == GNetworkClient.AttributeTrait.Validation then
		return
	end

	if log.canTrace() then
		local key = Enum.resolveValue(GNetworkClient.PublicAttribute, attribute)
		log.trace(("Set client %s public attribute: <%s>%s = %s"):format(clientID, attribute, key, value))
	end

	attributes[attribute] = value

	e.broadcasts[#e.broadcasts + 1] = {
		[fields.clientID] = clientID,
		[fields.attribute] = attribute,
		[fields.value] = value,
	}
end, "ReceiveClientPublicAttribute", "Receive", GNetworkMessage.Type.ClientPublicAttribute)

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
end, N_("UpdateUnverifiedClients"), "Network", nil, -1)

return SNetworkClients
