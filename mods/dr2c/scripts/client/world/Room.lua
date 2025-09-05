local CECS = require("dr2c.client.ecs.ECS")

local CWorldRoom = {}

--- @class dr2c.WorldRoomID : integer

--- @class dr2c.WorldRoom
--- @field tilemap table

--- @type table<dr2c.WorldRoomID, dr2c.WorldRoom>
local worldRooms = {}

function CWorldRoom.getRooms()
	return worldRooms
end

function CWorldRoom.getRoom(roomID)
	return worldRooms[roomID]
end

function CWorldRoom.getCollision(roomID)
	--
end

local entityIDsInRoomCache = {}

local filterGameObject = { "GameObject" }

--- @param roomID dr2c.NetworkRoomID
--- @return dr2c.EntityID[]
function CWorldRoom.getEntityIDsInRoom(roomID)
	local entityIDs = entityIDsInRoomCache[roomID]
	if not entityIDs then
		entityIDs = {}

		for _, entityID in CECS.iterateEntities(filterGameObject) do
			entityIDs[#entityIDs + 1] = entityID
		end

		entityIDsInRoomCache[roomID] = entityIDs
	end

	return entityIDs
end

--- @param e dr2c.E.CWorldTickProcess
events:add(N_("CWorldTickProcess"), function(e)
	if e.entitiesChanged then
		entityIDsInRoomCache = {}
	end
end, "ClearEntityIDsInRoomCache", "ECS", nil, 1)

--- @param e dr2c.E.CWorldRollback
events:add(N_("CWorldRollback"), function(e)
	if not e.suppressed then
		entityIDsInRoomCache = {}
	end
end, "ClearEntityIDsInRoomCache", "Reset")

return CWorldRoom
