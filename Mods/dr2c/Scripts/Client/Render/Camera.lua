--[[
-- @module dr2c.Client.Render.Camera
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class dr2c.CUICamera
local CUICamera = {}

local centerX = 0
local centerY = 0
local viewWidth = 960 * 0.5
local viewHeight = 640 * 0.5
--- @type TE.RenderTarget?
local renderTarget

centerX = persist("centerX", function()
	return centerX
end)
centerY = persist("centerY", function()
	return centerY
end)
viewWidth = persist("viewWidth", function()
	return viewWidth
end)
viewHeight = persist("viewHeight", function()
	return viewHeight
end)
renderTarget = persist("renderTarget", function()
	return renderTarget
end)

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

--- @return integer centerX
--- @return integer centerY
function CUICamera.getTargetCenterPosition()
	return centerX, centerY
end

--- @return number viewWidth
--- @return number viewHeight
function CUICamera.getViewSize()
	return viewWidth, viewHeight
end

--- @return TE.RenderTarget
function CUICamera.getRenderTarget()
	return renderTarget
end

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
		renderTarget = renderer:newRenderTarget(viewWidth, viewHeight)
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

	local scale = math.max(width / viewWidth, height / viewHeight)
	renderTarget:setCameraTargetPosition(centerX, centerY)
	renderTarget:setCameraTargetScale(scale, scale)

	TE.events:invoke(eventRenderCamera, e)

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

TE.events:add(N_("CUpdate"), function(e)
	local CInput = require("dr2c.Client.System.Input")

	if CInput.isKeyboardAvailable then
		-- local dx = (CInput.isScanCodeHeld(EScanCode.A) and -1 or 0) + (CInput.isScanCodeHeld(EScanCode.D) and 1 or 0)
		-- local dy = (CInput.isScanCodeHeld(EScanCode.W) and -1 or 0) + (CInput.isScanCodeHeld(EScanCode.S) and 1 or 0)
		-- centerX = centerX + dx
		-- centerY = centerY + dy
	end
end, "MoveCamera", "Inputs")

return CUICamera
