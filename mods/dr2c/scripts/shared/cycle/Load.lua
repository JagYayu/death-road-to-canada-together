local eventContentLoad = events:new(N_("ContentLoad"), {
	"Sprites",
	"ECS",
})

events:add("TickLoad", function(e)
	events:invoke(eventContentLoad, e)
end, N_("gameLoad"))

engine:triggerLoadPending()
