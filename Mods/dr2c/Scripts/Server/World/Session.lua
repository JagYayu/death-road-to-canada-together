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
local SNetworkServer = require("dr2c.Server.Network.Server")

--- @class dr2c.SWorldSession : dr2c.WorldSession
local SWorldSession = GWorldSession.new()

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

--- @param attributes? table<dr2c.WorldSessionAttribute, any>
--- @param sponsorClientID? TE.Network.ClientID
function SWorldSession.start(attributes, sponsorClientID)
	SWorldSession.resetAttributes()

	local worldSessionAttributes = SWorldSession.getAttributes()

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
	local e = {
		attributes = worldSessionAttributes,
		sponsorClientID = sponsorClientID,
		suppressed = nil,
	}

	TE.events:invoke(SWorldSession.eventSWorldSessionStart, e)
end

--- @param e dr2c.E.SWorldSessionStart
TE.events:add(SWorldSession.eventSWorldSessionStart, function(e)
	if not e.suppressed then
		local fields = GNetworkMessageFields.WorldSessionStart
		SNetworkServer.broadcastReliable(GNetworkMessage.Type.WorldSessionStart, {
			[fields.sponsorClientID] = e.sponsorClientID,
			[fields.attributes] = e.attributes,
		})
	elseif e.sponsorClientID then
		local fields = GNetworkMessageFields.WorldSessionStart
		SNetworkServer.sendReliable(e.sponsorClientID, GNetworkMessage.Type.WorldSessionStart, {
			[fields.sponsorClientID] = e.sponsorClientID,
			[fields.suppressedReason] = e.suppressed,
		})
	end
end, "SendMessage", "Network")

function SWorldSession.restart()
	-- TODO
end

--- @param sponsorClientID? TE.Network.ClientID
function SWorldSession.finish(sponsorClientID)
	SWorldSession.resetAttributes()

	--- @class dr2c.E.SWorldSessionFinish
	--- @field suppressed? boolean
	local e = {
		attributes = SWorldSession.getAttributes(),
		sponsorClientID = sponsorClientID,
	}

	TE.events:invoke(SWorldSession.eventSWorldSessionFinish, e)
end

--- @param e dr2c.E.SWorldSessionFinish
TE.events:add(SWorldSession.eventSWorldSessionFinish, function(e)
	if not e.suppressed then
		local fields = GNetworkMessageFields.WorldSessionFinish
		SNetworkServer.broadcastReliable(GNetworkMessage.Type.WorldSessionStart, {
			[fields.sponsorClientID] = e.sponsorClientID,
		})
	elseif e.sponsorClientID then
		local fields = GNetworkMessageFields.WorldSessionFinish
		SNetworkServer.sendReliable(e.sponsorClientID, GNetworkMessage.Type.WorldSessionStart, {
			[fields.sponsorClientID] = e.sponsorClientID,
			[fields.attributes] = e.attributes,
			[fields.suppressedReason] = e.suppressed,
		})
	end
end, "SendMessage", "Network")

--- @return boolean
function SWorldSession.pause()
	if not SWorldSession.isPlaying() then
		return false
	end

	local e = {}
	TE.events:invoke(SWorldSession.eventSWorldSessionPause, e)

	if not e.suppressed then
		local time = Time.getSystemTime()

		SWorldSession.setAttribute(GWorldSession.Attribute.State, GWorldSession.State.Paused)
		SWorldSession.setAttribute(GWorldSession.Attribute.TimePaused, time)

		SNetworkServer.broadcastReliable(GNetworkMessage.Type.WorldSessionPause, time)

		return true
	else
		return false
	end
end

--- @return boolean
function SWorldSession.unpause()
	if not SWorldSession.isPaused() then
		return false
	end

	local e = {}
	TE.events:invoke(SWorldSession.eventSWorldSessionUnpause, e)

	if not e.suppressed then
		local time = Time.getSystemTime()

		local elapsedPaused = SWorldSession.getAttribute(GWorldSession.Attribute.ElapsedPaused)
		local timePaused = SWorldSession.getAttribute(GWorldSession.Attribute.TimePaused)

		SWorldSession.setAttribute(GWorldSession.Attribute.State, GWorldSession.State.Paused)
		SWorldSession.setAttribute(GWorldSession.Attribute.ElapsedPaused, elapsedPaused + time - timePaused)

		SNetworkServer.broadcastReliable(GNetworkMessage.Type.WorldSessionUnpause, time)

		TE.events:invoke(SWorldSession.eventSWorldSessionUnpause)

		return true
	else
		return false
	end
end

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	SWorldSession.start(e.content, e.clientID)
end, "ReceiveWorldSessionStart", "Receive", GNetworkMessage.Type.WorldSessionStart)

TE.events:add(N_("SMessage"), function(e)
	SWorldSession.restart()
end, "ReceiveWorldSessionRestart", "Receive", GNetworkMessage.Type.WorldSessionRestart)

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	SWorldSession.finish(e.clientID)
end, "ReceiveWorldSessionFinish", "Receive", GNetworkMessage.Type.WorldSessionFinish)

TE.events:add(N_("SMessage"), function(e)
	SWorldSession.pause()
end, "ReceiveWorldSessionPause", "Receive", GNetworkMessage.Type.WorldSessionPause)

TE.events:add(N_("SMessage"), function(e)
	SWorldSession.pause()
end, "ReceiveWorldSessionUnpause", "Receive", GNetworkMessage.Type.WorldSessionUnpause)

return SWorldSession
