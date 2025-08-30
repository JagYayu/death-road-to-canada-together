--- @class dr2c.CWorldSession
local CWorldSession = {}

local WorldSessionState_inactive = 0
local WorldSessionState_playing = 1
local WorldSessionState_paused = 2

local state = 0

function CWorldSession.isActive()
	return state ~= WorldSessionState_inactive
end

function CWorldSession.isPlaying()
	return state ~= WorldSessionState_playing
end

function CWorldSession.isPaused()
	return state == WorldSessionState_paused
end

function CWorldSession.start(args)
	--
end

function CWorldSession.finish()
	--
end

function CWorldSession.restart(args)
	if CWorldSession.isActive() then
		
	end
end

events:add("DebugProvide", function(e)
	e.data:setDebugCommand({
		name = N_("WorldSessionStart"),
		help = "nah",
		func = function(arg)
			return { "test", 1 }
		end,
	})
end)

return CWorldSession
