local eventUpdateGame = events:new(N_("UpdateGame"), {
	"Client",
	"Server",
})

events:add("TickUpdate", function(e)
	events:invoke(eventUpdateGame, e)
end, N_("gameUpdate"))
