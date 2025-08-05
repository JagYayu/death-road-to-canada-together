-- --- @class Camera
-- local Camera = {}
-- local Matrix3x3 = require("dr2c.shared.utils.Matrix3x3")

-- local centerX = 0
-- local centerY = 0
-- local rotation = 0
-- local scaleX = 1
-- local scaleY = 1
-- local viewWidth = 1280
-- local viewHeight = 768

-- function Camera.getCenter()
-- 	return centerX, centerY
-- end

-- --- @param x number
-- --- @param y number
-- function Camera.setCenter(x, y)
-- 	centerX = x
-- 	centerY = y
-- end

-- function Camera.getTargetCenterPosition()
-- 	return centerX, centerY
-- end

-- local timer = 0

-- -- Events.add("KeyDown", function(e)
-- -- 	print(e)
-- -- end)

-- local layers = {}
-- local renderTarget
-- local renderTargetWidth = 1280
-- local renderTargetHeight = 720

-- function Camera.newLayer(z)
-- 	layers[#layers+1] = 
-- end

-- function Camera.getRenderTarget()
-- 	return renderTarget
-- end

-- local eventRenderCamera = events:new(N_("RenderCamera"), {
-- 	"Sprites",
-- })

-- local id = images:getID("gfx/cars/cars_unique_110x96.png")

-- local test = 2

-- events:add(N_("RenderGame"), function(e)
-- end, N_("updateCamera"), "Camera")

-- return Camera
