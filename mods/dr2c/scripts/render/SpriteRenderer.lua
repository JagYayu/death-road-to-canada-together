local Camera = require "dr2c.render.Camera"
local SpriteRenderer = {}

function SpriteRenderer.test()
end

local buffer = Render.newBuffer()

Events.add("RenderGame", function(e)
	buffer:setTransform(Camera.getTransform())

	buffer:clear()
	buffer:draw(0, 0, 1000, 1000, 1, 0, 0, 240, 240, 0, 0, 0, 0)
	buffer:render()
end, N_ "renderSprites")

return SpriteRenderer
