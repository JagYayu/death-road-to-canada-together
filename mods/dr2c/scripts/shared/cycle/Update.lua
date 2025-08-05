local Render = {}

local eventRenderGame = events:new(N_("UpdateGame"), {
	"Client",
	"Server",
})

events:add("TickRender", function(e)
	events:invoke(eventRenderGame, e)
end, N_("gameUpdate"), nil, "Main")

return Render
