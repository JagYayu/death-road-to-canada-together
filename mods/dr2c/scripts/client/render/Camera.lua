--- @class Camera
local Camera = {}
local Matrix3x3 = require("dr2c.shared.utils.Matrix3x3")

local centerX = 0
local centerY = 0
local rotation = 0
local scaleX = 1
local scaleY = 1
local viewWidth = 1280
local viewHeight = 768

function Camera.getCenter()
	return centerX, centerY
end

--- @param x number
--- @param y number
function Camera.setCenter(x, y)
	centerX = x
	centerY = y
end

function Camera.getTargetCenterPosition()
	return centerX, centerY
end

local timer = 0

-- Events.add("KeyDown", function(e)
-- 	print(e)
-- end)

local renderTarget
local renderTargetWidth = 1280
local renderTargetHeight = 720

function Camera.getRenderTarget()
	return renderTarget
end

local eventRenderCamera = events:new(N_("RenderCamera"), {
	"Sprites",
})

local id = images:getID("gfx/cars/cars_unique_110x96.png")

local test = 2

events:add(N_("RenderGame"), function(e)
	local renderer = e.window.renderer

	if renderTarget then
		-- renderTarget:resizeToFit()
	else
		renderTarget = renderer:newRenderTarget(renderTargetWidth, renderTargetHeight)
	end

	do
		local width, height = e.window:getSize()
		local scale = math.min(width / renderTargetWidth, height / renderTargetHeight)
		test = test
		scale = test
		renderTarget:setTargetScale(scale, scale)
	end
	renderTarget:setTargetPosition(0, 0)
	renderTarget:update()

	e.renderTarget = renderTarget

	renderer:beginTarget(renderTarget)
	renderer:clear()

	events:invoke(eventRenderCamera, e)

	renderer:draw({
		image = id,
		source = { 14, 302, 80, 57 },
		-- destination = { renderTargetWidth / 2 - 800 / 2, renderTargetHeight / 2 - 570 / 2, 800, 570 },
		-- source = { 49, 343, 1, 1 },
		destination = { 1280 / 4, 720 / 4, 1280 / 2, 720 / 2 },
	})

	renderer:endTarget()
end, N_("updateCamera"), "Camera")

return Camera
