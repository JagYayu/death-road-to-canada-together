--[[
-- @module dr2c.Server.Network.Room
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local GNetworkClient = require("dr2c.Shared.Network.Client")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local GNetworkRoom = require("dr2c.Shared.Network.Room")
local SNetworkClients = require("dr2c.Server.Network.Clients")
local SNetworkServer = require("dr2c.Server.Network.Server")

--- @class dr2c.SNetworkRoom : dr2c.MNetworkRoom
local SNetworkRoom = GNetworkRoom.new()

local NetworkRoom_createRoom = SNetworkRoom.createRoom
local NetworkRoom_destroyRoom = SNetworkRoom.destroyRoom
local MNetworkRoom_changeClientRoom = SNetworkRoom.changeClientRoom

local latestRoomID = 0

--- @type dr2c.NetworkRoomID?
local primaryRoomID

latestRoomID = persist("latestRoomID", function()
	return latestRoomID
end)
primaryRoomID = persist("primaryRoomID", function()
	return primaryRoomID
end)

SNetworkRoom.eventSCreateRoom = TE.events:new(N_("SCreateRoom"), {
	"ForthSession",
	"WorldSession",
	"PlayerInputBuffers",
	"Snapshot",
	"Network",
})

SNetworkRoom.eventSDestroyRoom = TE.events:new(N_("SDestroyRoom"), {
	"Snapshot",
	"PlayerInputBuffers",
	"WorldSession",
	"ForthSession",
	"Network",
})

SNetworkRoom.eventSClientChangeRoom = TE.events:new(N_("SClientChangeRoom"), {
	"Change",
	"PlayerInputBuffers",
})

--- @param roomID dr2c.NetworkRoomID
--- @param sponsorClientID? TE.Network.ClientID
function SNetworkRoom.createRoom(roomID, sponsorClientID)
	NetworkRoom_createRoom(roomID, sponsorClientID)

	--- @class dr2c.E.SCreateRoom
	local e = {
		roomID = roomID,
		sponsorClientID = sponsorClientID,
	}

	TE.events:invoke(SNetworkRoom.eventSCreateRoom, e)
end

--- @param e dr2c.E.SCreateRoom
TE.events:add(SNetworkRoom.eventSCreateRoom, function(e)
	local fields = GNetworkMessageFields.CCreateRoom
	SNetworkServer.broadcastReliable(GNetworkMessage.Type.CCreateRoom, {
		[fields.roomID] = e.roomID,
	})
end, "BroadcastToClients", "Network")

--- @return dr2c.NetworkRoomID
function SNetworkRoom.createNewRoom()
	latestRoomID = latestRoomID + 1
	SNetworkRoom.createRoom(latestRoomID)
	return latestRoomID
end

--- @param roomID dr2c.NetworkRoomID
function SNetworkRoom.destroyRoom(roomID)
	NetworkRoom_destroyRoom(roomID)

	--- @class dr2c.E.SDestroyRoom
	local e = {
		roomID = roomID,
	}

	TE.events:invoke(SNetworkRoom.eventSDestroyRoom, e)
end

--- Send reliable message to all clients in room.
--- 向房间内所有客户端发送可靠消息
--- @param roomID dr2c.NetworkRoomID
--- @param messageType dr2c.MessageType
--- @param messageContent any?
--- @param channel dr2c.NetworkMessageChannel?
--- @return boolean success
function SNetworkRoom.sendReliable(roomID, messageType, messageContent, channel)
	local session = SNetworkServer.getNetworkSession()
	if not (session and SNetworkRoom.hasRoom(roomID)) then
		return false
	end

	local packet = GNetworkMessage.pack(messageType, messageContent)

	if log.canTrace() then
		log.trace(("Send reliable message to clients in room %s: %s"):format(roomID, packet))
	end

	channel = channel or GNetworkMessage.Channel.Main
	for _, clientID in ipairs(SNetworkRoom.getClients(roomID)) do
		session:sendReliable(clientID, packet, channel)
	end

	return true
end

--- Send unreliable message to all clients in room.
--- 向房间内所有客户端发送不可靠消息
--- @param roomID dr2c.NetworkRoomID
--- @param messageType dr2c.MessageType
--- @param messageContent any?
--- @param channel dr2c.NetworkMessageChannel?
--- @return boolean success
function SNetworkRoom.sendUnreliable(roomID, messageType, messageContent, channel)
	local session = SNetworkServer.getNetworkSession()
	if not (session and SNetworkRoom.hasRoom(roomID)) then
		return false
	end

	local packet = GNetworkMessage.pack(messageType, messageContent)

	if log.canTrace() then
		log.trace(("Send unreliable message to clients in room %s: %s"):format(roomID, packet))
	end

	channel = channel or GNetworkMessage.Channel.Main
	for _, clientID in ipairs(SNetworkRoom.getClients(roomID)) do
		session:sendReliable(clientID, packet, channel)
	end

	return true
end

--- @param clientID TE.Network.ClientID
--- @param toRoomID dr2c.NetworkRoomID
--- @return dr2c.NetworkRoomID? fromRoomID
function SNetworkRoom.changeClientRoom(clientID, toRoomID)
	--- @class dr2c.E.SClientChangeRoom
	local e = {
		clientID = clientID,
		toRoomID = toRoomID,
		fromRoomID = nil,
	}

	TE.events:invoke(SNetworkRoom.eventSClientChangeRoom, e)

	return tonumber(e.fromRoomID)
end

--- @param e dr2c.E.SClientChangeRoom
TE.events:add(SNetworkRoom.eventSClientChangeRoom, function(e)
	if not e.fromRoomID then
		e.fromRoomID = MNetworkRoom_changeClientRoom(e.clientID, e.toRoomID)
	end
end, "ChangeClientRoom", "Change")

--- @param e dr2c.E.SHost
TE.events:add(N_("SHost"), function(e)
	if primaryRoomID then
		return
	end

	primaryRoomID = SNetworkRoom.createNewRoom()

	SNetworkRoom.setAttribute(primaryRoomID, GNetworkRoom.Attribute.Name, "Death Road to Canada")

	if log.canInfo() then
		log.info(("Created and initialized primary room %s"):format(primaryRoomID))
	end
end, "InitializeGameServer", "Initialize")

--- @param e dr2c.E.SShutdown
TE.events:add(N_("SShutdown"), function(e)
	primaryRoomID = nil
end, "DeinitializeGameServer", "Deinitialize")

--- @param e dr2c.E.SClientVerified
TE.events:add(N_("SClientVerified"), function(e)
	SNetworkServer.sendReliable(e.clientID, GNetworkMessage.Type.CRooms, SNetworkRoom.getRoomsAttributes())

	if not primaryRoomID then
		return
	end

	SNetworkClients.setPublicAttribute(e.clientID, GNetworkClient.PublicAttribute.Room, primaryRoomID)
end, "SendRoomsAndChangeRoom", "Rooms")

--- @param e dr2c.E.SClientPublicAttribute
TE.events:add(N_("SClientPublicAttribute"), function(e)
	local roomID = tonumber(e.value)
	assert(roomID)
	SNetworkRoom.changeClientRoom(e.clientID, roomID)
end, "ChangeClientRoom", "Room", GNetworkClient.PublicAttribute.Room)

return SNetworkRoom
