--[[
-- @module dr2c.client.world.Session
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("tudov.Enum")
local json = require("json")

local CClient = require("dr2c.client.network.Client")
local GMessage = require("dr2c.shared.network.Message")
local GWorldSession = require("dr2c.shared.world.Session")

--- @class dr2c.CWorldSession : dr2c.WorldSession
local CWorldSession = GWorldSession.new()

--- @param args table<dr2c.WorldSessionAttribute, any>
--- @return boolean
function CWorldSession.start(args)
	return CClient.sendReliable(GMessage.Type.WorldSessionStart, args)
end

--- @return boolean
function CWorldSession.restart()
	return CClient.sendReliable(GMessage.Type.WorldSessionRestart)
end

--- @return boolean
function CWorldSession.finish()
	return CClient.sendReliable(GMessage.Type.WorldSessionFinish)
end

--- @return boolean
function CWorldSession.pause()
	return CClient.sendReliable(GMessage.Type.WorldSessionPause)
end

--- @return boolean
function CWorldSession.unpause()
	return CClient.sendReliable(GMessage.Type.WorldSessionUnpause)
end

CWorldSession.eventCWorldSessionStart = events:new(N_("CWorldSessionStart"), {
	"Reset",
	"Level",
	"Rooms",
})

CWorldSession.eventCWorldSessionFinish = events:new(N_("CWorldSessionFinish"), {
	"Rooms",
	"Level",
	"Reset",
})

CWorldSession.eventCWorldSessionRestart = events:new(N_("CWorldSessionRestart"), {
	"", -- TODO
})

CWorldSession.eventCWorldSessionPause = events:new(N_("CWorldSessionPause"), {
	"Time",
})

CWorldSession.eventCWorldSessionUnpause = events:new(N_("CWorldSessionUnpause"), {
	"Time",
})

events:add(N_("CMessage"), function(e)
	if not e.suppressed then
		CWorldSession.setAttributes(e.content.attributes)

		events:invoke(CWorldSession.eventCWorldSessionStart, {})
	elseif log.canWarn() then
		log.warn(("Cannot start world session: "):format(e.suppressed))
	end
end, "StartWorldSession", "Receive", GMessage.Type.WorldSessionStart)

events:add(N_("CMessage"), function(e)
	if not e.suppressed then
		CWorldSession.resetAttributes()

		events:invoke(CWorldSession.eventCWorldSessionFinish, {})
	elseif log.canWarn() then
		log.warn(("Cannot finish world session: "):format(e.suppressed))
	end
end, "FinishWorldSession", "Receive", GMessage.Type.WorldSessionFinish)

events:add(N_("CMessage"), function(e)
	local internal = CWorldSession.getAttribute(GWorldSession.Attribute.Internal)
	if internal.state == GWorldSession.State.Playing then
		internal.state = GWorldSession.State.Paused
		internal.pausedTime = e.content

		events:invoke(CWorldSession.eventCWorldSessionPause)
	end
end, "PauseWorldSession", "Receive", GMessage.Type.WorldSessionPause)

events:add(N_("CMessage"), function(e)
	local internal = CWorldSession.getAttribute(GWorldSession.Attribute.Internal)
	if internal.state == GWorldSession.State.Paused then
		print("Unpause")

		internal.state = GWorldSession.State.Paused
		internal.elapsedPaused = internal.elapsedPaused + e.content - internal.timePaused

		events:invoke(CWorldSession.eventCWorldSessionUnpause)
	end
end, "UnpauseWorldSession", "Receive", GMessage.Type.WorldSessionUnpause)

--- @param e Events.E.DebugCommand
events:add("DebugProvide", function(e)
	e.data:setDebugCommand({
		name = "startWorldSession",
		help = "startWorldSession [args]",
		func = function(arg)
			local success, result
			if arg ~= "" then
				success, result = pcall(json.decode, arg)
			else
				success, result = true, {}
			end

			if success then
				CWorldSession.start(result)
				return { "Sent start world session request to server", EDebugConsoleCode.Success }
			else
				return { result, EDebugConsoleCode.Failure }
			end
		end,
	})
end, "ProvideClientWorldSessionCommands")

events:add(N_("CScanCodeDown"), function(e)
	if CWorldSession.getState() == GWorldSession.State.Paused then
		CWorldSession.unpause()
	elseif CWorldSession.getState() == GWorldSession.State.Paused then
		CWorldSession.pause()
	end
end, "WorldSessionPauseState", "Pause", EScanCode.P)

return CWorldSession
