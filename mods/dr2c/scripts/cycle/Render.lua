local Enum = require "dr2c.utils.Enum"
local Render = {}

-- Render.Buffers = Enum.sequence {
-- 	"Sprite",
-- }

-- Render.Transform = Enum.sequence {
-- 	"UI",
-- 	"World",
-- }

local eventRenderGame = Events.new("RenderGame", {
	"Camera",
	"Sprites",
	"Rooms",
	"UI",
})

Events.add("Render", function()
	Events.invoke(eventRenderGame, {})
end, N_ "render")

return Render
