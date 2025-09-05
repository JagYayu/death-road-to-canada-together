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

function CWorldSession.start(args)
	CClient.sendReliable(GMessage.Type.WorldSessionStart, args)
end

function CWorldSession.restart()
	CClient.sendReliable(GMessage.Type.WorldSessionRestart)
end

function CWorldSession.finish()
	CClient.sendReliable(GMessage.Type.WorldSessionFinish)
end

local WorldSession_pause = CWorldSession.pause

function CWorldSession.pause()
	CClient.sendReliable(GMessage.Type.WorldSessionPause)
end

function CWorldSession.unpause()
	CClient.sendReliable(GMessage.Type.WorldSessionUnpause)
end

CWorldSession.eventCWorldSessionStart = events:new(N_("CWorldSessionStart"), {
	"Level",
	"Rooms",
})

CWorldSession.eventCWorldSessionFinish = events:new(N_("CWorldSessionFinish"), {
	"Level",
	"Rooms",
})

CWorldSession.eventCWorldSessionPause = events:new(N_("CWorldSessionPause"), {
	"Time",
})

CWorldSession.eventCWorldSessionUnpause = events:new(N_("CWorldSessionUnpause"), {
	"Time",
})

events:add(N_("CMessage"), function(e)
	for _, entry in ipairs(e.content.attributes) do
		CWorldSession.setAttribute(entry[1], entry[2])
	end

	events:invoke(CWorldSession.eventCWorldSessionStart, {})
end, "StartWorldSession", "Receive", GMessage.Type.WorldSessionStart)

events:add(N_("CMessage"), function(e)
	CWorldSession.resetAttributes()

	events:invoke(CWorldSession.eventCWorldSessionFinish, {})
end, "FinishWorldSession", "Receive", GMessage.Type.WorldSessionPause)

--- @param e Events.E.DebugCommand
events:add("DebugProvide", function(e)
	e.data:setDebugCommand({
		name = "startWorldSession",
		help = "startWorldSession [args]",
		func = function(arg)
			local success, result = pcall(json.decode, arg)
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
