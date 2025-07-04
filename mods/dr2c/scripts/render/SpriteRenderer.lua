local Camera = require "dr2c.render.Camera"
local SpriteRenderer = {}

function SpriteRenderer.test()
end

local renderer = Window.newRenderer()

Events.add("RenderGame", function(e)
	-- renderer:setTransform(Camera.getTransform())

	-- renderer:clear()

	-- -- buffer:draw(0, 0, 1000, 1000, 1, 0, 0, 240, 240, -1, 0, 0, 0, 0)
	-- renderer:drawText("hello world", { 36, 1 }, 16, 100, 100, -1, 1, 0, 0, 0, 1)

	-- renderer:render()
end, N_ "renderSprites")

return SpriteRenderer
