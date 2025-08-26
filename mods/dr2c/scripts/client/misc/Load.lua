local eventClientContentLoad = events:new(N_("CContentLoad"), {
	"Sprites",
	"ECS",
})

events:add(N_("GLoad"), function(e)
	events:invoke(eventClientContentLoad, e)
end, N_("ClientContentLoad"), "Client")
