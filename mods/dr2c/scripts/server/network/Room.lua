--[[
-- @module dr2c.server.network.Room
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class dr2c.SRoom
local SRoom = {}

SRoom.defaultRoomID = 1

--- @type dr2c.NetworkRoomID
local serverRoomLatestID = SRoom.defaultRoomID
--- @type table<dr2c.NetworkRoomID, dr2c.RoomAttribute>
local serverRooms = {}

serverRoomLatestID = persist("serverRoomLatestID", function()
	return serverRoomLatestID
end)
serverRooms = persist("serverRooms", function()
	return serverRooms
end)

function SRoom.getAll()
	return serverRooms
end

function SRoom.getRoomAttribute(roomID, roomAttribute) end

return SRoom
