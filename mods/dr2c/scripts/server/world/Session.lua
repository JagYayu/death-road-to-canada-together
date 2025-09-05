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

local GWorldSession = require("dr2c.shared.world.Session")
local GMessage = require("dr2c.shared.network.Message")
local SServer = require("dr2c.server.network.Server")

--- @class dr2c.SWorldSession : dr2c.WorldSession
local SWorldSession = GWorldSession.new()

SWorldSession.eventSWorldSessionStart = events:new(N_("SWorldSessionStart"), {
	"Level",
})

SWorldSession.eventSWorldSessionFinish = events:new(N_("SWorldSessionFinish"), {
	"Level",
})

SWorldSession.eventSWorldSessionPause = events:new(N_("SWorldSessionPause"), {
	"Time",
})

SWorldSession.eventSWorldSessionUnpause = events:new(N_("SWorldSessionUnpause"), {
	"Time",
})

--- @param args table<dr2c.WorldSessionAttribute, any>
function SWorldSession.start(args)
	SWorldSession.resetAttributes()

	local worldSessionAttributes = SWorldSession.getAttributes()

	for attribute, value in pairs(args) do
		if GWorldSession.validateAttribute(attribute, value) then
			worldSessionAttributes[attribute] = value
		end
	end

	local e = {
		attributes = worldSessionAttributes,
		suppressed = nil,
	}
	events:invoke(SWorldSession.eventSWorldSessionStart, e)

	if not e.suppressed then
		SServer.broadcastReliable(GMessage.Type.WorldSessionStart, {
			attributes = worldSessionAttributes,
		})
	end
end

events:add(N_("SMessage"), function(e)
	SWorldSession.start(e.content)
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
		internal.state = GWorldSession.State.Paused
		internal.pausedTime = Time.getSystemTime()

		SServer.broadcastReliable(GMessage.Type.WorldSessionPause)

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
	if internal.state ~= GWorldSession.State.Playing then
		return false
	end

	local e = {}
	events:invoke(SWorldSession.eventSWorldSessionUnpause, e)

	if not e.suppressed then
		internal.state = GWorldSession.State.Paused
		internal.elapsedPaused = internal.elapsedPaused + (Time.getSystemTime() - internal.timePaused)

		SServer.broadcastReliable(GMessage.Type.WorldSessionUnpause)

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
