local Render = {}

local eventRenderGame = events:new(N_("RenderGame"), {
	"Camera",
	"UI",
})

events:add("TickRender", function(e)
	events:invoke(eventRenderGame, e)
end, N_("renderGame"), nil, "Main")

return Render
