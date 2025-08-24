--- @class dr2c.CFrame
local CRender = {}

--- @type integer
local renderFrame
renderFrame = persist("renderFrame", 0, function()
	return renderFrame
end)

--- @return integer
function CRender.getFrame()
	return renderFrame
end

local eventRenderGame = events:new(N_("RenderGame"), {
	"Camera",
	"UI",
})

events:add("TickRender", function(e)
	events:invoke(eventRenderGame, e)
end, N_("gameRender"), nil, "Main")

return CRender
