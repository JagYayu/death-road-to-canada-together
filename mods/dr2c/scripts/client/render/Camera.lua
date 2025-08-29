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

local renderTarget
local renderTargetWidth = 960
local renderTargetHeight = 640

function Camera.getRenderTarget()
	return renderTarget
end

local imageID = images:getID("gfx/cars/cars_unique_110x96.png")

local eventRenderCamera = events:new(N_("CRenderCamera"), {
	"Begin",
	"Sprites",
	"DebugObjects",
	"End",
})

--- @param e dr2c.E.ClientRender
events:add(N_("CRender"), function(e)
	local snapScale
	local renderer = e.renderer
	if not renderTarget then
		renderTarget = renderer:newRenderTarget(renderTargetWidth, renderTargetHeight)
		snapScale = true
	elseif renderTarget:resizeToFit() then
		snapScale = true
	end
	renderTarget:update()

	if snapScale then
		renderTarget:snapCameraScale()
	end

	renderer:beginTarget(renderTarget)
	renderer:clear()

	local width, height = e.window:getSize()

	local scale = math.max(width / renderTargetWidth, height / renderTargetHeight)
	renderTarget:setCameraTargetPosition(0, 0)
	renderTarget:setCameraTargetScale(scale, scale)

	events:invoke(eventRenderCamera, e)

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
