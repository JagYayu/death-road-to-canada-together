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

local Table = require("tudov.Table")

local GWorldSession = require("dr2c.shared.world.Session")
local GMessage = require("dr2c.shared.network.Message")
local SServer = require("dr2c.server.network.Server")

--- @class dr2c.SWorldSession : dr2c.WorldSession
local SWorldSession = GWorldSession.new()

SWorldSession.eventSWorldSessionStart = events:new(N_("SWorldSessionStart"), {
	"Reset",
	"Level",
})

SWorldSession.eventSWorldSessionFinish = events:new(N_("SWorldSessionFinish"), {
	"Level",
	"Reset",
})

SWorldSession.eventSWorldSessionPause = events:new(N_("SWorldSessionPause"), {
	"Time",
})

SWorldSession.eventSWorldSessionUnpause = events:new(N_("SWorldSessionUnpause"), {
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

	worldSessionAttributes[GWorldSession.Attribute.Internal] = {
		state = GWorldSession.State.Playing,
		timeStart = time,
		timePaused = time,
		elapsedPaused = 0,
	}

	local e = {
		attributes = worldSessionAttributes,
		suppressed = nil,
	}
	events:invoke(SWorldSession.eventSWorldSessionStart, e)

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
events:add(N_("SMessage"), function(e)
	SWorldSession.start(e.clientID, e.content)
end, "ReceiveWorldSessionStart", "Receive", GMessage.Type.WorldSessionStart)

function SWorldSession.restart()
	-- TODO
end

events:add(N_("SMessage"), function(e)
	SWorldSession.restart()
end, "ReceiveWorldSessionRestart", "Receive", GMessage.Type.WorldSessionRestart)

function SWorldSession.finish()
	SWorldSession.resetAttributes()

	local e = {}
	events:invoke(SWorldSession.eventSWorldSessionFinish, e)

	if not e.suppressed then
		SServer.broadcastReliable(GMessage.Type.WorldSessionFinish)
	end
end

events:add(N_("SMessage"), function(e)
	SWorldSession.finish()
end, "ReceiveWorldSessionFinish", "Receive", GMessage.Type.WorldSessionFinish)

--- @return boolean
function SWorldSession.pause()
	local internal = SWorldSession.getAttribute(GWorldSession.Attribute.Internal)
	if internal.state ~= GWorldSession.State.Playing then
		return false
	end

	local e = {}
	events:invoke(SWorldSession.eventSWorldSessionPause, e)

	if not e.suppressed then
		local time = Time.getSystemTime()

		internal.state = GWorldSession.State.Paused
		internal.pausedTime = time

		SServer.broadcastReliable(GMessage.Type.WorldSessionPause, time)

		return true
	else
		return false
	end
end

events:add(N_("SMessage"), function(e)
	SWorldSession.pause()
end, "ReceiveWorldSessionPause", "Receive", GMessage.Type.WorldSessionPause)

--- @return boolean
function SWorldSession.unpause()
	local internal = SWorldSession.getAttribute(GWorldSession.Attribute.Internal)
	if internal.state ~= GWorldSession.State.Paused then
		return false
	end

	local e = {}
	events:invoke(SWorldSession.eventSWorldSessionUnpause, e)

	if not e.suppressed then
		local time = Time.getSystemTime()

		internal.state = GWorldSession.State.Paused
		internal.elapsedPaused = internal.elapsedPaused + time - internal.timePaused

		SServer.broadcastReliable(GMessage.Type.WorldSessionUnpause, time)

		events:invoke(SWorldSession.eventSWorldSessionUnpause)

		return true
	else
		return false
	end
end

events:add(N_("SMessage"), function(e)
	SWorldSession.pause()
end, "ReceiveWorldSessionUnpause", "Receive", GMessage.Type.WorldSessionUnpause)

return SWorldSession
