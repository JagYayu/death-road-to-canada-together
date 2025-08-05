-- local Camera = require "dr2c.render.Camera"
-- local SpriteRenderer = {}

-- local renderer = Engine.getMainWindow().getRenderer()

-- function SpriteRenderer.test()
-- end

local renderTarget
local drawOnce = true

local imageID = images:getID("gfx/cars/cars_unique_110x96.png")

local renderTargetWidth, renderTargetHeight = 960, 640

events:add(N_("RenderGame"), function(e)
	-- local snapScale
	-- local renderer = e.window.renderer
	-- if not renderTarget then
	-- 	renderTarget = renderer:newRenderTarget(renderTargetWidth, renderTargetHeight)
	-- 	snapScale = true
	-- elseif renderTarget:resizeToFit() then
	-- 	snapScale = true
	-- end
	-- renderTarget:update()

	-- local width, height = e.window:getSize()

	-- local scale = math.max(width / renderTargetWidth, height / renderTargetHeight)
	-- renderTarget:setCameraTargetPosition(100, 0)
	-- renderTarget:setCameraTargetScale(scale, scale)

	-- if snapScale then
	-- 	renderTarget:snapCameraScale()
	-- end

	-- renderer:beginTarget(renderTarget)
	-- renderer:clear()

	-- for x = -1, 2 do
	-- 	renderer:draw({
	-- 		image = imageID,
	-- 		destination = { x * 100, x * 100, 80, 57 },
	-- 		source = { 14, 302, 80, 57 },
	-- 	})
	-- end

	-- local renderTarget = renderer:endTarget()
	-- renderer:draw({
	-- 	renderTarget = renderTarget,
	-- 	destination = {
	-- 		0,
	-- 		0,
	-- 		width,
	-- 		height,
	-- 	},
	-- })
end, N_("renderSprites"))

-- return SpriteRenderer
