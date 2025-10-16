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

local GWorldSession = require("dr2c.Shared.World.Session")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
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
})

SWorldSession.eventSWorldSessionPause = TE.events:new(N_("SWorldSessionPause"), {
	"Time",
})

SWorldSession.eventSWorldSessionUnpause = TE.events:new(N_("SWorldSessionUnpause"), {
	"Time",
})

--- @param attributes table<dr2c.WorldSessionAttribute, any>?
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

	local e = {
		attributes = worldSessionAttributes,
		sponsorClientID = sponsorClientID,
		suppressed = nil,
	}
	TE.events:invoke(SWorldSession.eventSWorldSessionStart, e)
end

TE.events:add(SWorldSession.eventSWorldSessionStart, function(e)
	print(e.attributes)
	if not e.suppressed then
		SNetworkServer.broadcastReliable(GNetworkMessage.Type.WorldSessionStart, {
			attributes = e.attributes,
			sponsorClientID = e.sponsorClientID,
		})
	elseif e.sponsorClientID then
		SNetworkServer.sendReliable(e.sponsorClientID, GNetworkMessage.Type.WorldSessionStart, {
			suppressed = e.suppressed,
			sponsorClientID = e.sponsorClientID,
		})
	end
end, "SendMessage", "Network")

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	SWorldSession.start(e.content, e.clientID)
end, "ReceiveWorldSessionStart", "Receive", GNetworkMessage.Type.WorldSessionStart)

function SWorldSession.restart()
	-- TODO
end

TE.events:add(N_("SMessage"), function(e)
	SWorldSession.restart()
end, "ReceiveWorldSessionRestart", "Receive", GNetworkMessage.Type.WorldSessionRestart)

function SWorldSession.finish()
	SWorldSession.resetAttributes()

	local e = {}
	TE.events:invoke(SWorldSession.eventSWorldSessionFinish, e)

	if not e.suppressed then
		SNetworkServer.broadcastReliable(GNetworkMessage.Type.WorldSessionFinish)
	end
end

--- @param e {}
TE.events:add(N_("SMessage"), function(e)
	SWorldSession.finish()
end, "ReceiveWorldSessionFinish", "Receive", GNetworkMessage.Type.WorldSessionFinish)

--- @return boolean
function SWorldSession.pause()
	if SWorldSession.getAttribute(GWorldSession.Attribute.State) ~= GWorldSession.State.Playing then
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

TE.events:add(N_("SMessage"), function(e)
	SWorldSession.pause()
end, "ReceiveWorldSessionPause", "Receive", GNetworkMessage.Type.WorldSessionPause)

--- @return boolean
function SWorldSession.unpause()
	if SWorldSession.getAttribute(GWorldSession.Attribute.State) ~= GWorldSession.State.Paused then
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

TE.events:add(N_("SMessage"), function(e)
	SWorldSession.pause()
end, "ReceiveWorldSessionUnpause", "Receive", GNetworkMessage.Type.WorldSessionUnpause)

return SWorldSession
