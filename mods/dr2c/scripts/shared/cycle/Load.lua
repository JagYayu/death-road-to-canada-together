local eventContentLoad = events:new(N_("ContentLoad"), {
	"ECS",
})

local loadOnce = true

events:add("TickLoad", function(e)
	events:invoke(eventContentLoad, e)
end, N_("gameLoad"))
