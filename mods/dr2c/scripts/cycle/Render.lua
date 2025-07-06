local Enum = require "dr2c.utils.Enum"
local Render = {}

-- Render.Buffers = Enum.sequence {
-- 	"Sprite",
-- }

-- Render.Transform = Enum.sequence {
-- 	"UI",
-- 	"World",
-- }

local eventRenderGame = events:new("RenderGame", {
	"Camera",
	"Sprites",
	"Rooms",
	"UI",
})

events:add("Render", function()
	events:invoke(eventRenderGame, {})
end, N_ "render")

return Render
