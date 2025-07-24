-- local Camera = require "dr2c.render.Camera"
-- local SpriteRenderer = {}

-- local renderer = Engine.getMainWindow().getRenderer()

-- function SpriteRenderer.test()
-- end

local renderTarget
local drawOnce = true

local imageID = images:getID("gfx/cars/cars_unique_110x96.png")

local i
i = persist("i", 1, function()
	return i
end)

events:add(N_("RenderGame"), function(e)
	i = i + 1
	-- local renderer = e.window.renderer
	-- renderTarget = renderTarget or renderer:newRenderTarget(1280, 720)

	-- renderer:beginTarget(renderTarget)

	-- -- renderer:clear()

	-- renderer:draw({
	-- 	image = imageID,
	-- 	destination = {
	-- 		i,
	-- 		0,
	-- 		1000,
	-- 		1000,
	-- 	},
	-- })

	-- renderer:endTarget({
	-- 	0,
	-- 	0,
	-- 	1280,
	-- 	720,
	-- }, {
	-- 	0,
	-- 	0,
	-- 	1280,
	-- 	720,
	-- })
end, N_("renderSprites"))

-- return SpriteRenderer
