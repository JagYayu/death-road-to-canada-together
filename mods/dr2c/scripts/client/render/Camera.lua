--- @class Camera
local Camera = {}
local Matrix3x3 = require("tudov.Matrix3x3")

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

local layers = {}
local renderTarget
local renderTargetWidth = 1280
local renderTargetHeight = 720

function Camera.getRenderTarget()
	return renderTarget
end

local imageID = images:getID("gfx/cars/cars_unique_110x96.png")

local renderTargetWidth, renderTargetHeight = 960, 640

local eventRenderCamera = events:new(N_("RenderCamera"), {
	"Begin",
	"Sprites",
	"DebugObjects",
	"End",
})

events:add(N_("CRender"), function(e)
	local snapScale
	local renderer = e.window.renderer
	if not renderTarget then
		renderTarget = renderer:newRenderTarget(renderTargetWidth, renderTargetHeight)
		snapScale = true
	elseif renderTarget:resizeToFit() then
		snapScale = true
	end
	renderTarget:update()

	events:invoke(eventRenderCamera, e)

	if snapScale then
		renderTarget:snapCameraScale()
	end

	renderer:beginTarget(renderTarget)
	renderer:clear()

	local width, height = e.window:getSize()

	local scale = math.max(width / renderTargetWidth, height / renderTargetHeight)
	renderTarget:setCameraTargetPosition(300, 0)
	renderTarget:setCameraTargetScale(scale, scale)

	for x = -1, 2 do
		renderer:draw({
			image = imageID,
			destination = { x * 100, x * 100, 80, 57 },
			source = { 14, 302, 80, 57 },
		})
	end

	renderer:draw({
		renderTarget = renderer:endTarget(),
		destination = {
			0,
			0,
			width,
			height,
		},
	})
end, N_("RenderCamera"), "Camera")

return Camera
