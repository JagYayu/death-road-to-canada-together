local Enum = require "tudov.Enum"

--- @class dr2c.RoomID : integer

--- @class dr2c.GRoom
local GRoom = {}

--- @enum dr2c.RoomAttribute
GRoom.Attribute = Enum.sequence({
	ID = 0,
	Name = 1,
})

return GRoom
