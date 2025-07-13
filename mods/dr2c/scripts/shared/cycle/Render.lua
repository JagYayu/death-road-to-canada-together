local Enum = require("dr2c.shared.utils.Enum")
local Render = {}

-- Render.Buffers = Enum.sequence {
-- 	"Sprite",
-- }

-- Render.Transform = Enum.sequence {
-- 	"UI",
-- 	"World",
-- }

local eventRenderGame = events:new(N_("RenderGame"), {
	"Camera",
	"Sprites",
	"Rooms",
	"UI",
})

events:add("TickRender", function(e)
	-- print(e)
	events:invoke(eventRenderGame, e)
end, N_("renderGame"), nil, "Main")

return Render
