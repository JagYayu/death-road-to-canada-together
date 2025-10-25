--[[
-- @module dr2c.Client.Network.ClientUtils
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local EnumFlag = require("TE.EnumFlag")
local Table = require("TE.Table")

local CNetworkClient = require("dr2c.Client.Network.Client")
local CNetworkClients = require("dr2c.Client.Network.Clients")
local CNetworkRoom = require("dr2c.Client.Network.Room")
local GNetworkClient = require("dr2c.Shared.Network.Client")

--- @class dr2c.CNetworkClientUtils
local CNetworkClientUtils = {}

--- @return boolean
--- @nodiscard
function CNetworkClientUtils.hasPermissionAuthority()
	local permissions = CNetworkClient.getPublicAttribute(GNetworkClient.PublicAttribute.Permissions)
	return permissions and EnumFlag.hasAny(permissions, GNetworkClient.Permission.Authority) or false
end

--- @return dr2c.NetworkRoomID roomID
--- @nodiscard
function CNetworkClientUtils.getRoom()
	return CNetworkClient.getPublicAttribute(GNetworkClient.PublicAttribute.Room)
end

--- @return TE.Network.ClientID[] clients @READONLY
--- @nodiscard
function CNetworkClientUtils.getClientsInRoom()
	local roomID = CNetworkClient.getPublicAttribute(GNetworkClient.PublicAttribute.Room)
	if CNetworkRoom.hasRoom(roomID) then
		return CNetworkRoom.getClients(roomID)
	else
		return Table.empty
	end
end

--- @type TE.Network.ClientID[]?
local otherClientsOnServerCache

--- @return TE.Network.ClientID[] clients @READONLY
--- @nodiscard
function CNetworkClientUtils.getOtherClientsOnServer()
	if otherClientsOnServerCache then
		return otherClientsOnServerCache
	end

	otherClientsOnServerCache = {}

	local localClientID = CNetworkClient.getClientID()
	for _, clientID in ipairs(CNetworkClients.getClients()) do
		if clientID ~= localClientID then
			otherClientsOnServerCache[#otherClientsOnServerCache + 1] = clientID
		end
	end

	return otherClientsOnServerCache
end

--- @type TE.Network.ClientID[]?
local otherClientsInRoomCache

--- @return TE.Network.ClientID[]
--- @nodiscard
function CNetworkClientUtils.getOtherClientsInRoom()
	if otherClientsInRoomCache then
		return otherClientsInRoomCache
	end

	otherClientsInRoomCache = {}

	local localClientID = CNetworkClient.getClientID()
	for _, clientID in ipairs(CNetworkClientUtils.getClientsInRoom()) do
		if clientID ~= localClientID then
			otherClientsInRoomCache[#otherClientsInRoomCache + 1] = clientID
		end
	end

	return otherClientsInRoomCache
end

function CNetworkClientUtils.clearCaches()
	otherClientsOnServerCache = nil
	otherClientsInRoomCache = nil
end

TE.events:add(N_("CClientAdded"), CNetworkClientUtils.clearCaches, "ClearNetworkClientUtilsCaches", "Cache")
TE.events:add(N_("CClientRemoved"), CNetworkClientUtils.clearCaches, "ClearNetworkClientUtilsCaches", "Cache")

return CNetworkClientUtils
