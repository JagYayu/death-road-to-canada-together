--[[
-- @module dr2c.Shared.Network.Clients
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
local Utility = require("TE.Utility")
local inspect = require("inspect")

local GNetworkClient = require("dr2c.Shared.Network.Client")

--- 只提供一些工具函数给`dr2c.CNetworkClients`, `dr2c.SNetworkClients`使用
--- @class dr2c.GNetworkClients
local GNetworkClients = {}

local modules = {}

modules = persist("modules", function()
	return modules
end)

--#region Misc Functions

local function stringClientNotExists(clientID)
	return ("Client %d does not exist in server"):format(clientID)
end

--#endregion

--- @param clientList TE.Network.ClientID[]
--- @param clientsPublicAttributes table<TE.Network.ClientID, table<dr2c.ClientPublicAttribute, any>>
--- @return TE.Network.ClientID[]
local function getHostClients(clientList, clientsPublicAttributes)
	local hostClients = {}

	for _, clientID in ipairs(clientList) do
		local permissions = clientsPublicAttributes[clientID][GNetworkClient.PublicAttribute.Permissions]
		if EnumFlag.hasAll(permissions, GNetworkClient.Permissions.Host) then
			hostClients[#hostClients + 1] = clientID
		end
	end

	return hostClients
end

--- @param clientList TE.Network.ClientID[]
--- @param clientsPublicAttributes table<TE.Network.ClientID, table<dr2c.ClientPublicAttribute, any>>
--- @return TE.Network.ClientID[]
local function getAuthoritativeClients(clientList, clientsPublicAttributes, roomID)
	local authoritativeClients = {}

	for _, clientID in ipairs(clientList) do
		local attributes = clientsPublicAttributes[clientID]
		local room = attributes[GNetworkClient.PublicAttribute.Room]
		local permissions = attributes[GNetworkClient.PublicAttribute.Permissions]
		if room == roomID and EnumFlag.hasAll(permissions, GNetworkClient.Permission.Authority) then
			authoritativeClients[#authoritativeClients + 1] = clientID
		end
	end

	return authoritativeClients
end

function GNetworkClients.new()
	--- @class dr2c.MNetworkClients
	local NetworkClients = {}

	--- @type TE.Network.ClientID[]
	local clientList = {}

	--- @type table<TE.Network.ClientID, table<dr2c.ClientPublicAttribute, Serializable>>
	local clientsPublicAttributes = {}

	--- @type table<TE.Network.ClientID, table<dr2c.ClientPublicAttribute, Serializable>>
	local clientsPrivateAttributes = {}

	--- @type TE.Network.ClientID | false?
	local hostClientCache
	--- @type table<dr2c.NetworkRoomID, TE.Network.ClientID | false>
	local authoritativeClientCaches = {}

	Utility.persistModule(modules, function()
		return { clientList, clientsPublicAttributes, clientsPrivateAttributes }
	end, function(module)
		clientList, clientsPublicAttributes, clientsPrivateAttributes = module[1], module[2], module[3]
	end)

	--- @return TE.Network.ClientID[] clients @READONLY
	--- @nodiscard
	function NetworkClients.getClients()
		return clientList
	end

	--- @return table<TE.Network.ClientID, table<dr2c.GNetworkClient.PublicAttribute, Serializable>>
	--- @nodiscard
	function NetworkClients.getClientsPublicAttributes()
		return clientsPublicAttributes
	end

	--- @return table<TE.Network.ClientID, table<dr2c.GNetworkClient.PrivateAttribute, Serializable>>
	--- @nodiscard
	function NetworkClients.getClientsPrivateAttributes()
		return clientsPrivateAttributes
	end

	function NetworkClients.clear()
		clientList = {}
		clientsPublicAttributes = {}
		clientsPrivateAttributes = {}
		hostClientCache = nil
		authoritativeClientCaches = {}
	end

	--- @param clientID TE.Network.ClientID
	--- @return boolean
	--- @nodiscard
	function NetworkClients.hasClient(clientID)
		return clientsPublicAttributes[clientID] ~= nil
	end

	--- @param clientID TE.Network.ClientID
	function NetworkClients.addClient(clientID)
		if clientsPublicAttributes[clientID] then
			error("Client %d has already been added", 2)
		end

		local publicAttributes = {}
		local privateAttributes = {}
		GNetworkClient.resetPublicAttributes(publicAttributes)
		GNetworkClient.resetPrivateAttributes(privateAttributes)

		clientList[#clientList + 1] = clientID
		clientsPublicAttributes[clientID] = publicAttributes
		clientsPrivateAttributes[clientID] = privateAttributes

		hostClientCache = nil
		authoritativeClientCaches = {}
	end

	--- @param clientID TE.Network.ClientID
	function NetworkClients.removeClient(clientID)
		if not clientsPublicAttributes[clientID] then
			error(("Client %s does not exist"):format(clientID), 2)
		end

		List.removeFirst(clientList, clientID)
		clientsPublicAttributes[clientID] = nil
		clientsPrivateAttributes[clientID] = nil

		hostClientCache = nil
		authoritativeClientCaches = {}
	end

	--- @param clientID TE.Network.ClientID
	--- @return table<dr2c.GNetworkClient.PublicAttribute, Serializable>
	--- @nodiscard
	function NetworkClients.getPublicAttributes(clientID)
		local attributes = clientsPublicAttributes[clientID]
		if not attributes then
			error(("Client %s does not exist"):format(clientID), 2)
		end

		return attributes
	end

	--- @param clientID TE.Network.ClientID
	--- @return table<dr2c.GNetworkClient.PublicAttribute, Serializable>>
	--- @nodiscard
	function NetworkClients.getPrivateAttributes(clientID)
		local attributes = clientsPrivateAttributes[clientID]
		if not attributes then
			error(("Client %s does not exist"):format(clientID), 2)
		end

		return attributes
	end

	--- 读取客户端公有属性
	--- @param clientID TE.Network.ClientID
	--- @param publicAttribute dr2c.ClientPublicAttribute
	--- @return Serializable?
	--- @nodiscard
	function NetworkClients.getPublicAttribute(clientID, publicAttribute)
		local attributes = clientsPublicAttributes[clientID]
		if not attributes then
			error(stringClientNotExists(clientID), 2)
		end

		return attributes[publicAttribute]
	end

	--- 读取客户端私有属性
	--- @param clientID TE.Network.ClientID
	--- @param privateAttribute dr2c.ClientPrivateAttribute
	--- @return Serializable?
	--- @nodiscard
	function NetworkClients.getPrivateAttribute(clientID, privateAttribute)
		local attributes = clientsPrivateAttributes[clientID]
		if not attributes then
			error(stringClientNotExists(clientID), 2)
		end

		return attributes[privateAttribute]
	end

	--- @deprecated TODO
	function NetworkClients.setPublicAttributes(clientID, publicAttributes)
		-- TODO
	end

	--- @param clientID TE.Network.ClientID
	--- @param publicAttribute dr2c.ClientPublicAttribute
	--- @param attributeValue? Serializable
	function NetworkClients.setPublicAttribute(clientID, publicAttribute, attributeValue)
		local attributes = clientsPublicAttributes[clientID]
		if not attributes then
			error(("Client %s does not exists"):format(clientID), 2)
		elseif not GNetworkClient.validatePublicAttribute(publicAttribute, attributeValue) then
			error("Invalid public attribute", 2) -- TODO
		end

		attributes[publicAttribute] = attributeValue
	end

	--- @warn Do not modify attributes other than local client, this will cause desync with server data. Prefer using `CNetworkClient.setPrivateAttribute`.
	--- @warn 不要手动修改非本地客户端的属性，这会导致与服务器数据不同步。最好使用`CNetworkClient.setPrivateAttribute`
	--- @param clientID TE.Network.ClientID
	--- @param privateAttribute dr2c.ClientPrivateAttribute
	--- @param attributeValue any
	function NetworkClients.setPrivateAttribute(clientID, privateAttribute, attributeValue)
		local attributes = clientsPrivateAttributes[clientID]
		if not attributes then
			error(("Client %s does not exists"):format(clientID), 2)
		elseif not GNetworkClient.validatePublicAttribute(privateAttribute, attributeValue) then
			error("Invalid private attribute", 2) -- TODO
		end

		attributes[privateAttribute] = attributeValue
	end

	--- 获取服务器的主持者客户端
	--- @return TE.Network.ClientID?
	--- @nodiscard
	function NetworkClients.getHostClient()
		if hostClientCache ~= nil then
			return hostClientCache or nil
		end

		local clients = getHostClients(clientList, clientsPublicAttributes)
		if #clients == 1 then
			hostClientCache = clients[1]
		elseif #clients == 0 then
			hostClientCache = false
		elseif log.canWarn() then
			log.warn(("Multiple host clients detected on the server: %s"):format(inspect(clients)))
		end
	end

	--- 获取房间内的权威客户端
	--- @param roomID dr2c.NetworkRoomID
	--- @return TE.Network.ClientID?
	--- @nodiscard
	function NetworkClients.getAuthoritativeClient(roomID)
		local authoritativeClient = authoritativeClientCaches[roomID]
		if authoritativeClient ~= nil then
			return authoritativeClient or nil
		end

		local clients = getAuthoritativeClients(clientList, clientsPublicAttributes, roomID)
		if #clients == 1 then
			authoritativeClient = clients[1]
		elseif #clients == 0 then
			authoritativeClient = false
		elseif log.canWarn() then
			log.warn(("Multiple authoritative clients detected in the room %d: %s"):format(roomID, inspect(clients)))
		end

		authoritativeClientCaches[roomID] = authoritativeClient
	end

	--- @param clientID TE.Network.ClientID
	--- @return boolean
	--- @nodiscard
	function NetworkClients.isVerifiedClient(clientID)
		local attributes = clientsPublicAttributes[clientID]
		if not attributes then
			error(stringClientNotExists(clientID), 2)
		end

		return attributes[GNetworkClient.PublicAttribute.State] == GNetworkClient.State.Verified
	end

	--- @param clientID dr2c.NetworkRoomID
	--- @return dr2c.NetworkRoomID?
	--- @nodiscard
	function NetworkClients.getRoomID(clientID)
		local attributes = clientsPublicAttributes[clientID]
		if not attributes then
			error(stringClientNotExists(clientID), 2)
		end

		--- @diagnostic disable-next-line: return-type-mismatch
		return attributes[GNetworkClient.PublicAttribute.Room]
	end

	return NetworkClients
end

return GNetworkClients
