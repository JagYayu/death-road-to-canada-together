local eventContentLoad = events:new(N_("ContentLoad"), {
	"ECS",
})

local loadOnce = true

events:add("TickLoad", function(e)
	-- local t = {}
	-- for i = 1, 1e7 do
	-- 	t[#t + 1] = math.random()
	-- end

	events:invoke(eventContentLoad, e)
end, N_("gameLoad"))

engine:triggerLoadPending()
