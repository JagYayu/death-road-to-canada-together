--[[
-- @module dr2c.server.world.Session
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
local GMessage = require("dr2c.Shared.Network.Message")
local SServer = require("dr2c.Server.Network.Server")

--- @class dr2c.SWorldSession : dr2c.WorldSession
local SWorldSession = GWorldSession.new()

SWorldSession.eventSWorldSessionStart = TE.events:new(N_("SWorldSessionStart"), {
	"Reset",
	"Level",
})

SWorldSession.eventSWorldSessionFinish = TE.events:new(N_("SWorldSessionFinish"), {
	"Level",
	"Reset",
})

SWorldSession.eventSWorldSessionPause = TE.events:new(N_("SWorldSessionPause"), {
	"Time",
})

SWorldSession.eventSWorldSessionUnpause = TE.events:new(N_("SWorldSessionUnpause"), {
	"Time",
})

--- @param args table<dr2c.WorldSessionAttribute, any>?
function SWorldSession.start(clientID, args)
	SWorldSession.resetAttributes()

	local worldSessionAttributes = SWorldSession.getAttributes()

	for attribute, value in pairs(args or Table.empty) do
		if GWorldSession.validateAttribute(attribute, value) then
			worldSessionAttributes[attribute] = value
		end
	end

	local time = Time.getSystemTime()

	worldSessionAttributes[GWorldSession.Attribute.State] = GWorldSession.State.Playing
	worldSessionAttributes[GWorldSession.Attribute.TimeStart] = time
	worldSessionAttributes[GWorldSession.Attribute.TimePaused] = time
	worldSessionAttributes[GWorldSession.Attribute.ElapsedPaused] = 0

	local e = {
		attributes = worldSessionAttributes,
		suppressed = nil,
	}
	TE.events:invoke(SWorldSession.eventSWorldSessionStart, e)

	if not e.suppressed then
		SServer.broadcastReliable(GMessage.Type.WorldSessionStart, {
			attributes = worldSessionAttributes,
		})
	elseif clientID then
		SServer.sendReliable(clientID, GMessage.Type.WorldSessionStart, {
			suppressed = e.suppressed,
		})
	end
end

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	SWorldSession.start(e.clientID, e.content)
end, "ReceiveWorldSessionStart", "Receive", GMessage.Type.WorldSessionStart)

function SWorldSession.restart()
	-- TODO
end

TE.events:add(N_("SMessage"), function(e)
	SWorldSession.restart()
end, "ReceiveWorldSessionRestart", "Receive", GMessage.Type.WorldSessionRestart)

function SWorldSession.finish()
	SWorldSession.resetAttributes()

	local e = {}
	TE.events:invoke(SWorldSession.eventSWorldSessionFinish, e)

	if not e.suppressed then
		SServer.broadcastReliable(GMessage.Type.WorldSessionFinish)
	end
end

--- @param e {}
TE.events:add(N_("SMessage"), function(e)
	SWorldSession.finish()
end, "ReceiveWorldSessionFinish", "Receive", GMessage.Type.WorldSessionFinish)

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

		SServer.broadcastReliable(GMessage.Type.WorldSessionPause, time)

		return true
	else
		return false
	end
end

TE.events:add(N_("SMessage"), function(e)
	SWorldSession.pause()
end, "ReceiveWorldSessionPause", "Receive", GMessage.Type.WorldSessionPause)

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

		SServer.broadcastReliable(GMessage.Type.WorldSessionUnpause, time)

		TE.events:invoke(SWorldSession.eventSWorldSessionUnpause)

		return true
	else
		return false
	end
end

TE.events:add(N_("SMessage"), function(e)
	SWorldSession.pause()
end, "ReceiveWorldSessionUnpause", "Receive", GMessage.Type.WorldSessionUnpause)

return SWorldSession
