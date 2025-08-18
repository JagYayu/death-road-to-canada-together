local eventUpdateGame = events:new(N_("UpdateGame"), {
	"Client",
	"Server",
})

events:add("TickUpdate", function(e)
	-- local t = {}
	-- for i = 1, 1e7 do
	-- 	t[#t + 1] = math.random()
	-- end

	events:invoke(eventUpdateGame, e)
end, N_("gameUpdate"))
