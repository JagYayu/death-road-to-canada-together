--[[
-- @module dr2c.Client.Network.Room
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local inspect = require("inspect")

local GNetworkClient = require("dr2c.Shared.Network.Client")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local GNetworkRoom = require("dr2c.Shared.Network.Room")

--- @class dr2c.CNetworkRoom : dr2c.MNetworkRoom
local CNetworkRoom = GNetworkRoom.new()

local MNetworkRoom_changeClientRoom = CNetworkRoom.changeClientRoom

CNetworkRoom.eventCClientChangeRoom = TE.events:new(N_("CClientChangeRoom"), {
	"Change",
	"PlayerInputBuffers",
})

--- @param clientID TE.Network.ClientID
--- @param toRoomID dr2c.NetworkRoomID
--- @return dr2c.NetworkRoomID? fromRoomID
function CNetworkRoom.changeClientRoom(clientID, toRoomID)
	--- @class dr2c.E.CClientChangeRoom
	--- @field fromRoomID? dr2c.NetworkRoomID
	local e = {
		clientID = clientID,
		toRoomID = toRoomID,
	}

	TE.events:invoke(CNetworkRoom.eventCClientChangeRoom, e)

	return tonumber(e.fromRoomID)
end

--- @param e dr2c.E.CClientChangeRoom
TE.events:add(CNetworkRoom.eventCClientChangeRoom, function(e)
	if not e.fromRoomID then
		e.fromRoomID = MNetworkRoom_changeClientRoom(e.clientID, e.toRoomID)
	end
end, "ChangeClientRoom", "Change")

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	local rooms = e.content

	if type(rooms) ~= "table" then
		if log.canDebug() then
			log.debug("Received rooms from server, ignoring because `content` is not a table that storing rooms")
		end

		return
	end

	if log.canDebug() then
		log.debug(("Received rooms from server: %s"):format(inspect(rooms)))
	end

	for roomID, roomAttributes in pairs(rooms) do
		if not CNetworkRoom.hasRoom(roomID) then
			CNetworkRoom.createRoom(roomID)
		end

		CNetworkRoom.setAttributes(roomID, roomAttributes)
	end
end, "ReceiveRooms", "Receive", GNetworkMessage.Type.CRooms)

--- @param e dr2c.E.CClientPublicAttribute
TE.events:add(N_("CClientPublicAttribute"), function(e)
	local roomID = e.value
	assert(type(roomID) == "number")

	if log.canDebug() then
		log.debug(("Change client %d's room to %s"):format(e.clientID, roomID))
	end

	CNetworkRoom.changeClientRoom(e.clientID, roomID)
end, "ReceiveClientChangeRoom", "Room", GNetworkClient.PublicAttribute.Room)

return CNetworkRoom
