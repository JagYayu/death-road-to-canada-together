--[[
-- @module dr2c.Server.World.Session
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("TE.Table")

local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local GNetworkReason = require("dr2c.Shared.Network.Reason")
local GWorldSession = require("dr2c.Shared.World.Session")
local SNetworkClients = require("dr2c.Server.Network.Clients")
local SNetworkRoom = require("dr2c.Server.Network.Room")
local SNetworkServer = require("dr2c.Server.Network.Server")

--- @class dr2c.SWorldSession
local SWorldSession = {}

--- @type table<dr2c.NetworkRoomID, dr2c.MWorldSession>
local room2WorldSessions = {}

SWorldSession.eventSWorldSessionStart = TE.events:new(N_("SWorldSessionStart"), {
	"Reset",
	"Level",
	"Scene",
	"Map",
	"Network",
})

SWorldSession.eventSWorldSessionFinish = TE.events:new(N_("SWorldSessionFinish"), {
	"Map",
	"Scene",
	"Level",
	"Reset",
	"Network",
})

SWorldSession.eventSWorldSessionPause = TE.events:new(N_("SWorldSessionPause"), {
	"Time",
})

SWorldSession.eventSWorldSessionUnpause = TE.events:new(N_("SWorldSessionUnpause"), {
	"Time",
})

--- @param roomID dr2c.NetworkRoomID
--- @return boolean
--- @nodiscard
function SWorldSession.contains(roomID)
	return not not room2WorldSessions[roomID]
end

--- @param roomID dr2c.NetworkRoomID
--- @param attributes? table<dr2c.WorldSessionAttribute, Serializable>
--- @param sponsorClientID? TE.Network.ClientID
function SWorldSession.start(roomID, attributes, sponsorClientID)
	local worldSession = room2WorldSessions[roomID]
	if not worldSession then
		error(("World session does not exists in room %s"):format(roomID), 2)
	end

	worldSession.resetAttributes()

	local worldSessionAttributes = worldSession.getAttributes()

	for attribute, value in pairs(attributes or Table.empty) do
		if GWorldSession.validateAttribute(attribute, value) then
			worldSessionAttributes[attribute] = value
		elseif log.canTrace() then
			log.trace(("Validation to attribute %s failed"):format(attribute))
		end
	end

	local time = Time.getSystemTime()

	worldSessionAttributes[GWorldSession.Attribute.State] = GWorldSession.State.Playing
	worldSessionAttributes[GWorldSession.Attribute.TimeStart] = time
	worldSessionAttributes[GWorldSession.Attribute.TimePaused] = time
	worldSessionAttributes[GWorldSession.Attribute.ElapsedPaused] = 0

	--- @class dr2c.E.SWorldSessionStart
	--- @field suppressed? boolean
	local e = {
		worldSession = worldSession,
		attributes = worldSessionAttributes,
		sponsorClientID = sponsorClientID,
	}

	TE.events:invoke(SWorldSession.eventSWorldSessionStart, e)
end

--- @param e dr2c.E.SWorldSessionStart
TE.events:add(SWorldSession.eventSWorldSessionStart, function(e)
	if not e.suppressed then
		local fields = GNetworkMessageFields.CWorldSessionStart
		SNetworkServer.broadcastReliable(GNetworkMessage.Type.CWorldSessionStart, {
			[fields.worldSessionID] = e.worldSession.getSessionID(),
			[fields.sponsorClientID] = e.sponsorClientID,
			[fields.attributes] = e.attributes,
		})
	elseif e.sponsorClientID then
		local fields = GNetworkMessageFields.CWorldSessionStart
		SNetworkServer.sendReliable(e.sponsorClientID, GNetworkMessage.Type.CWorldSessionStart, {
			[fields.worldSessionID] = e.worldSession.getSessionID(),
			[fields.sponsorClientID] = e.sponsorClientID,
			[fields.suppressedReason] = e.suppressed,
		})
	end
end, "SendMessage", "Network")

function SWorldSession.restart(room, sponsorClientID)
	-- TODO
end

--- @param roomID dr2c.NetworkRoomID
--- @param sponsorClientID? TE.Network.ClientID
function SWorldSession.finish(roomID, sponsorClientID)
	local worldSession = room2WorldSessions[roomID]
	if not worldSession then
		error(("World session does not exists in room %s"):format(roomID), 2)
	end

	worldSession.resetAttributes()

	--- @class dr2c.E.SWorldSessionFinish
	--- @field suppressed? boolean
	local e = {
		roomID = roomID,
		attributes = worldSession.getAttributes(),
		sponsorClientID = sponsorClientID,
	}

	TE.events:invoke(SWorldSession.eventSWorldSessionFinish, e)
end

--- @param e dr2c.E.SWorldSessionFinish
TE.events:add(SWorldSession.eventSWorldSessionFinish, function(e)
	if not e.suppressed then
		local fields = GNetworkMessageFields.CWorldSessionFinish
		SNetworkServer.broadcastReliable(GNetworkMessage.Type.CWorldSessionFinish, {
			[fields.roomID] = e.roomID,
			[fields.worldSessionID] = e.attributes,
			[fields.sponsorClientID] = e.sponsorClientID,
		})
	elseif e.sponsorClientID then
		local fields = GNetworkMessageFields.CWorldSessionFinish
		SNetworkServer.sendReliable(e.sponsorClientID, GNetworkMessage.Type.CWorldSessionFinish, {
			[fields.roomID] = e.roomID,
			[fields.sponsorClientID] = e.sponsorClientID,
			[fields.attributes] = e.attributes,
			[fields.suppressedReason] = e.suppressed,
		})
	end
end, "SendMessage", "Network")

--- @param roomID dr2c.NetworkRoomID
--- @return boolean
function SWorldSession.pause(roomID)
	local worldSession = room2WorldSessions[roomID]
	if not worldSession then
		error(("World session not found in room %s"):format(roomID), 2)
	end

	local e = {}
	TE.events:invoke(SWorldSession.eventSWorldSessionPause, e)

	if not e.suppressed then
		local time = Time.getSystemTime()

		worldSession.setAttribute(GWorldSession.Attribute.State, GWorldSession.State.Paused)
		worldSession.setAttribute(GWorldSession.Attribute.TimePaused, time)

		SNetworkServer.broadcastReliable(GNetworkMessage.Type.CWorldSessionPause, time)

		return true
	else
		return false
	end
end

--- @param roomID dr2c.NetworkRoomID
--- @return boolean
function SWorldSession.unpause(roomID)
	local worldSession = room2WorldSessions[roomID]
	if not worldSession then
		error(("World session not found in room %s"):format(roomID), 2)
	end

	if not worldSession.isPaused() then
		return false
	end

	local e = {}
	TE.events:invoke(SWorldSession.eventSWorldSessionUnpause, e)

	if not e.suppressed then
		local time = Time.getSystemTime()

		local elapsedPaused = worldSession.getAttribute(GWorldSession.Attribute.ElapsedPaused)
		local timePaused = worldSession.getAttribute(GWorldSession.Attribute.TimePaused)

		worldSession.setAttribute(GWorldSession.Attribute.State, GWorldSession.State.Paused)
		worldSession.setAttribute(GWorldSession.Attribute.ElapsedPaused, elapsedPaused + time - timePaused)

		SNetworkServer.broadcastReliable(GNetworkMessage.Type.CWorldSessionUnpause, time)

		TE.events:invoke(SWorldSession.eventSWorldSessionUnpause)

		return true
	else
		return false
	end
end

--- @param roomID dr2c.NetworkRoomID
function SWorldSession.resetAttributes(roomID)
	local worldSession = room2WorldSessions[roomID]
	if not worldSession then
		error(("World session not found in room %s"):format(roomID), 2)
	end

	worldSession.resetAttributes()
end

--- @param roomID dr2c.NetworkRoomID
function SWorldSession.getAttributes(roomID)
	local worldSession = room2WorldSessions[roomID]
	if not worldSession then
		error(("World session not found in room %s"):format(roomID), 2)
	end

	return worldSession.getAttributes()
end

--- @param roomID dr2c.NetworkRoomID
function SWorldSession.setAttributes(roomID)
	local worldSession = room2WorldSessions[roomID]
	if not worldSession then
		error(("World session not found in room %s"):format(roomID), 2)
	end
end

--- @param roomID dr2c.NetworkRoomID
function SWorldSession.getAttribute(roomID, attribute)
	local worldSession = room2WorldSessions[roomID]
	if not worldSession then
		error(("World session not found in room %s"):format(roomID), 2)
	end

	return worldSession.getAttribute(attribute)
end

--- @param roomID dr2c.NetworkRoomID
function SWorldSession.setAttribute(roomID, attribute, value)
	local worldSession = room2WorldSessions[roomID]
	if not worldSession then
		error(("World session not found in room %s"):format(roomID), 2)
	end

	return worldSession.setAttribute(attribute, value)
end

--- @param roomID dr2c.NetworkRoomID
function SWorldSession.getSessionID(roomID)
	local worldSession = room2WorldSessions[roomID]
	if not worldSession then
		error(("World session not found in room %s"):format(roomID), 2)
	end

	return worldSession.getSessionID()
end

--- @param roomID dr2c.NetworkRoomID
function SWorldSession.nextSessionID(roomID)
	local worldSession = room2WorldSessions[roomID]
	if not worldSession then
		error(("World session not found in room %s"):format(roomID), 2)
	end

	worldSession.nextSessionID()
end

--- @param roomID dr2c.NetworkRoomID
--- @return boolean
--- @nodiscard
function SWorldSession.isInactive(roomID)
	local worldSession = room2WorldSessions[roomID]
	if not worldSession then
		error(("World session not found in room %s"):format(roomID), 2)
	end

	return worldSession.isInactive()
end

--- @param roomID dr2c.NetworkRoomID
--- @return boolean
--- @nodiscard
function SWorldSession.isPlaying(roomID)
	local worldSession = room2WorldSessions[roomID]
	if not worldSession then
		error(("World session not found in room %s"):format(roomID), 2)
	end

	return worldSession.isPlaying()
end

--- @param roomID dr2c.NetworkRoomID
--- @return boolean
--- @nodiscard
function SWorldSession.isPaused(roomID)
	local worldSession = room2WorldSessions[roomID]
	if not worldSession then
		error(("World session not found in room %s"):format(roomID), 2)
	end

	return worldSession.isPaused()
end

--- @param e dr2c.E.SCreateRoom
TE.events:add(N_("SCreateRoom"), function(e)
	room2WorldSessions[e.roomID] = GWorldSession.new(e.roomID)
end, "NewWorldSession", "WorldSession")

--- @param e dr2c.E.SCreateRoom
TE.events:add(N_("SDestroyRoom"), function(e)
	room2WorldSessions[e.roomID] = nil
end, "DeleteWorldSession", "WorldSession")

--- @param e dr2c.E.SClientVerified
TE.events:add(N_("SClientVerified"), function(e)
	local clientID = e.clientID
	local roomID = SNetworkClients.getRoomID(clientID)
	if not roomID then
		if log.canWarn() then
			log.warn(("Client %d is supposed to be in a room"):format(clientID))
		end

		return
	end

	local worldSession = room2WorldSessions[roomID]

	SNetworkServer.sendReliable(clientID, GNetworkMessage.Type.CWorldSessionAttributes, worldSession.getAttributes())
end, "SendWorldSessionAttributes", "WorldSession")

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	local clientID = e.clientID
	local roomID = SNetworkClients.getRoomID(clientID)
	if not (roomID and SNetworkRoom.hasRoom(roomID)) then
		if log.canInfo() then
			log.info(
				("Client %d attempt to start world session, ignoring because its in an invalid room %s"):format(
					clientID,
					roomID
				)
			)
		end

		return
	end

	SWorldSession.start(roomID, e.content, e.clientID)
end, "ReceiveWorldSessionStart", "Receive", GNetworkMessage.Type.SWorldSessionStart)

TE.events:add(N_("SMessage"), function(e)
	local clientID = e.clientID
	local roomID = SNetworkClients.getRoomID(e.clientID)
	if not (roomID and SNetworkRoom.hasRoom(roomID)) then
		if log.canInfo() then
			log.info(
				("Client %d attempt to restart world session, ignoring because its in an invalid room %s"):format(
					clientID,
					roomID
				)
			)
		end

		return
	end

	SWorldSession.restart(roomID, clientID)
end, "ReceiveWorldSessionRestart", "Receive", GNetworkMessage.Type.SWorldSessionRestart)

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	local clientID = e.clientID
	local roomID = SNetworkClients.getRoomID(clientID)
	if not (roomID and SNetworkRoom.hasRoom(roomID)) then
		if log.canInfo() then
			log.info(
				("Client %d attempt to finish world session, ignoring because its in an invalid room %s"):format(
					clientID,
					roomID
				)
			)
		end

		return
	end

	SWorldSession.finish(roomID, clientID)
end, "ReceiveWorldSessionFinish", "Receive", GNetworkMessage.Type.SWorldSessionFinish)

TE.events:add(N_("SMessage"), function(e)
	local roomID = SNetworkClients.getRoomID(e.clientID)
	if roomID and SNetworkRoom.hasRoom(roomID) then
		SWorldSession.pause(roomID) -- TODO
	end
end, "ReceiveWorldSessionPause", "Receive", GNetworkMessage.Type.SWorldSessionPause)

TE.events:add(N_("SMessage"), function(e)
	local roomID = SNetworkClients.getRoomID(e.clientID)
	if roomID and SNetworkRoom.hasRoom(roomID) then
		SWorldSession.unpause(roomID) -- TODO
	end
end, "ReceiveWorldSessionUnpause", "Receive", GNetworkMessage.Type.SWorldSessionUnpause)

return SWorldSession
