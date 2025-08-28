--- @class dr2c.CWorldSession
local CWorldSession = {}

function CWorldSession.start()
	--
end

function CWorldSession.finish() end

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
