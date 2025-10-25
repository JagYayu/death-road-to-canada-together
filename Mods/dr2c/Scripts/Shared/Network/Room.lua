--[[
-- @module dr2c.Shared.Network.Room
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")
local Function = require("TE.Function")
local List = require("TE.List")
local Utility = require("TE.Utility")

local GNetworkServer = require("dr2c.Shared.Network.Server")
local GUtilsAttribute = require("dr2c.Shared.Utils.Attribute")

--- @class dr2c.NetworkRoomID : integer

--- @class dr2c.GNetworkRoom
local GNetworkRoom = {}

--#region Misc Functions

local function strHasRoom(roomID)
	return ("Room %s already existed"):format(roomID)
end

local function strNoRoom(roomID)
	return ("Room %s does not exists"):format(roomID)
end

--#endregion

local modules = {}

modules = persist("modules", function()
	return modules
end)

--- @alias dr2c.NetworkRoomAttribute dr2c.GNetworkRoom.Attribute

GNetworkRoom.Attribute = Enum.sequence({
	ID = 0,
	--- 房间名
	Name = 1,
	--- 最大玩家数
	MaxPlayers = 2,
})

local Attribute = GNetworkRoom.Attribute

local proxyAttributeProperties = GUtilsAttribute.newProperties({
	[Attribute.ID] = {
		noReset = true,
		validator = Function.alwaysTrue,
	},
	[Attribute.Name] = {
		default = "",
		validator = Function.isTypeString,
	},
	[Attribute.MaxPlayers] = {
		default = 4,
		validator = Function.isTypeInteger,
	},
})

--- @type fun(attribute: dr2c.NetworkRoomAttribute): dr2c.AttributeProperty
GNetworkRoom.getAttributeProperty = proxyAttributeProperties.getAttributeProperty

--- @type fun(attribute: dr2c.NetworkRoomAttribute, args: dr2c.AttributePropertyArgs)
GNetworkRoom.setAttributeProperty = proxyAttributeProperties.setAttributeProperty

--- @type fun(attribute: dr2c.NetworkRoomAttribute, value: Serializable): boolean
GNetworkRoom.validateAttribute = proxyAttributeProperties.validateAttribute

local resetAttributes = proxyAttributeProperties.resetAttributes

--- @return dr2c.MNetworkRoom
function GNetworkRoom.new()
	local SNetworkClients = require("dr2c.Server.Network.Clients")

	--- @class dr2c.MNetworkRoom
	local NetworkRoom = {}

	--- @type table<dr2c.NetworkRoomID, table<dr2c.NetworkRoomAttribute, Serializable>>
	local roomsAttributes = {}
	--- @type table<dr2c.NetworkRoomID, TE.Network.ClientID[]>
	local roomsClients = {}

	Utility.persistModule(modules, function()
		return { roomsAttributes, roomsClients }
	end, function(t)
		roomsAttributes, roomsClients = t[1], t[2]
	end)

	--- @return table<dr2c.NetworkRoomID, table<dr2c.GNetworkRoom.Attribute, Serializable>>
	--- @nodiscard
	function NetworkRoom.getRoomsAttributes()
		return roomsAttributes
	end

	--- @param roomID dr2c.NetworkRoomID
	--- @return boolean
	--- @nodiscard
	function NetworkRoom.hasRoom(roomID)
		return not not roomsAttributes[tonumber(roomID) or true]
	end

	--- @param roomID dr2c.NetworkRoomID
	function NetworkRoom.createRoom(roomID)
		if type(roomID) ~= "number" then
			error("Invalid room id type", 2)
		elseif roomsAttributes[roomID] then
			error(strHasRoom(roomID), 2)
		end

		local roomAttributes = {}
		resetAttributes(roomAttributes)

		local room2Clients = {}
		for _, clientID in ipairs(SNetworkClients.getClients()) do
			room2Clients[#room2Clients + 1] = clientID
		end

		roomsAttributes[roomID] = roomAttributes
		roomsClients[roomID] = room2Clients
	end

	--- @param roomID dr2c.NetworkRoomID
	function NetworkRoom.destroyRoom(roomID)
		if type(roomID) ~= "number" then
			error("Invalid room id type", 2)
		elseif not roomsAttributes[roomID] then
			error(strNoRoom(roomID), 2)
		end

		roomsAttributes[roomID] = nil
		roomsClients[roomID] = nil
	end

	--- @param roomID dr2c.NetworkRoomID
	function NetworkRoom.resetAttributes(roomID)
		local attributes = roomsAttributes[roomID]
		if not attributes then
			error(strNoRoom(roomID), 2)
		end

		resetAttributes(attributes)
	end

	--- @param roomID dr2c.NetworkRoomID
	--- @nodiscard
	function NetworkRoom.getAttributes(roomID)
		local attributes = roomsAttributes[roomID]
		if not attributes then
			error(strNoRoom(roomID), 2)
		end

		return attributes
	end

	--- @param roomID dr2c.NetworkRoomID
	function NetworkRoom.setAttributes(roomID, roomAttributes)
		local attributes = roomsAttributes[roomID]
		if not attributes then
			error(strNoRoom(roomID), 2)
		end

		for roomAttribute, attributeValue in pairs(roomAttributes) do
			if not GNetworkRoom.validateAttribute(roomAttribute, attributeValue) then
				error(("Invalid room attribute %s's value %s"):format(roomAttribute, attributeValue), 2)
			end

			attributes[roomAttribute] = attributeValue
		end
	end

	--- @param roomID dr2c.NetworkRoomID
	--- @param roomAttribute dr2c.NetworkRoomAttribute
	--- @return Serializable
	--- @nodiscard
	function NetworkRoom.getAttribute(roomID, roomAttribute)
		local attributes = roomsAttributes[roomID]
		if not attributes then
			error(strNoRoom(roomID), 2)
		end

		return attributes[roomAttribute]
	end

	--- @param roomID dr2c.NetworkRoomID
	--- @param roomAttribute dr2c.NetworkRoomAttribute
	--- @param attributeValue Serializable
	function NetworkRoom.setAttribute(roomID, roomAttribute, attributeValue)
		local attributes = roomsAttributes[roomID]
		if not attributes then
			error(strNoRoom(roomID), 2)
		end

		if not GNetworkRoom.validateAttribute(roomAttribute, attributeValue) then
			error(("Invalid room attribute %s's value %s"):format(roomAttribute, attributeValue), 2)
		end

		attributes[roomAttribute] = attributeValue
	end

	--- @param roomID dr2c.NetworkRoomID?
	--- @return TE.Network.ClientID[]
	--- @nodiscard
	function NetworkRoom.getClients(roomID)
		local clients = roomsClients[roomID]
		if not clients and roomID then
			error(strNoRoom(roomID), 2)
		end

		return clients
	end

	--- @param clientID TE.Network.ClientID
	--- @param toRoomID dr2c.NetworkRoomID?
	--- @return dr2c.NetworkRoomID? fromRoomID
	function NetworkRoom.changeClientRoom(clientID, toRoomID)
		local newRoomsClients = roomsClients[toRoomID]
		if not newRoomsClients and toRoomID then
			error(strNoRoom(toRoomID), 2)
		end

		if List.findFirst(newRoomsClients, clientID) then
			return
		end

		local fromRoomID = SNetworkClients.getRoomID(clientID)

		local oldRoomsClients = roomsClients[fromRoomID]
		if oldRoomsClients and not List.removeFirst(oldRoomsClients, clientID) then
			error(("Client %d was no in previous room %s"):format(clientID, fromRoomID), 2)
		end

		if newRoomsClients then
			newRoomsClients[#newRoomsClients + 1] = clientID
		end

		return fromRoomID
	end

	return NetworkRoom
end

return GNetworkRoom
