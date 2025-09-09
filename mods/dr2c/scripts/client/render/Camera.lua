--[[
-- @module dr2c.client.render.Camera
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

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
local renderTargetWidth = 960 * 0.5
local renderTargetHeight = 640 * 0.5

function Camera.getRenderTarget()
	return renderTarget
end

local imageID = images:getID("gfx/cars/cars_unique_110x96.png")

local eventRenderCamera = events:new(N_("CRenderCamera"), {
	"Begin",
	"Tilemap",
	"Sprites",
	"DebugObjects",
	"Debug",
	"End",
})

local drawRectArgs = DrawRectArgs()

--- @param e dr2c.E.CRender
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

	drawRectArgs.texture = imageID

	for x = -1, 2 do
		drawRectArgs.destination = { x = x * 100, y = x * 100, w = 80, h = 57 }
		drawRectArgs.source = { x = 14, y = 302, w = 80, h = 57 }
		renderer:drawRect(drawRectArgs)
	end

	drawRectArgs.texture = renderer:endTarget()
	drawRectArgs.destination = {
		x = 0,
		y = 0,
		w = width,
		h = height,
	}
	drawRectArgs.source = nil
	renderer:drawRect(drawRectArgs)
end, N_("RenderCamera"), "Camera")

return Camera
