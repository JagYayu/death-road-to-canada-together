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

local Matrix3x3 = require("tudov.Matrix3x3")

--- @class dr2c.CUICamera
local CUICamera = {}

local centerX = 0
local centerY = 0

--- @return integer x
--- @return integer y
function CUICamera.getCenter()
	return centerX, centerY
end

--- @param x number
--- @param y number
function CUICamera.setCenter(x, y)
	centerX = x
	centerY = y
end

--- @return integer x
--- @return integer y
function CUICamera.getTargetCenterPosition()
	return centerX, centerY
end

--- @type TE.RenderTarget?
local renderTarget
local renderTargetWidth = 960 * 0.5
local renderTargetHeight = 640 * 0.5

function CUICamera.getRenderTarget()
	return renderTarget
end

local imageID = TE.images:getID("gfx/cars/cars_unique_110x96.png")

local eventRenderCamera = TE.events:new(N_("CRenderCamera"), {
	"Begin",
	"Tilemap",
	"Sprites",
	"DebugObjects",
	"Debug",
	"End",
})

local drawRectArgs = DrawRectArgs()

--- @param e dr2c.E.CRender
TE.events:add(N_("CRender"), function(e)
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

	TE.events:invoke(eventRenderCamera, e)

	drawRectArgs.texture = imageID

	for x = -1, 2 do
		drawRectArgs.destination = { x = x * 100, y = x * 100, w = 80, h = 57 }
		drawRectArgs.source = { x = 14, y = 302, w = 80, h = 57 }
		renderer:drawRect(drawRectArgs)
	end

	local drawRect = renderer.drawRect

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

return CUICamera
