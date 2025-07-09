local Enum = require("dr2c.utils.Enum")
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
	events:invoke(eventRenderGame, e)
end, N_("render"), nil, "Main")

return Render
