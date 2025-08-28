--- @class dr2c.SRoom
local SRoom = {}

SRoom.defaultRoomID = 1

--- @type dr2c.RoomID
local serverRoomLatestID = SRoom.defaultRoomID
--- @type table<dr2c.RoomID, dr2c.RoomAttribute>
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
