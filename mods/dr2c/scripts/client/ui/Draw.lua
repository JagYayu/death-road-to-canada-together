--[[
-- @module dr2c.Client.UI.Draw
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Color = require("TE.Color")
local Math = require("TE.Math")

local Math_lerp = Math.lerp

--- @class dr2c.CUIDraw
local CUIDraw = {}

--- @type TE.Renderer
local renderer = TE.engine.primaryWindow.renderer

local drawRectArgs = DrawRectArgs()
drawRectArgs.texture = 0
drawRectArgs.source = { x = 0, y = 0, w = 0, h = 0 }
local drawRectDst = drawRectArgs.destination
local drawRectSrc = drawRectArgs.source

local drawTextArgs = DrawTextArgs()

local uiBorderTexture = TE.images:getID("Mods/dr2c/GFX/UI.png")
local uiBorderTextureX = 0
local uiBorderTextureY = 0
local uiBorderTextureWidth = 9
local uiBorderTextureHeight = 9
local uiBorderTextureSize = 4
local uiSelectionBoxTexture = TE.images:getID("gfx/misc/dr2c_particles.png")
local uiSelectionBoxTextureX = 208
local uiSelectionBoxTextureY = 128
local uiSelectionBoxTextureWidth = 16
local uiSelectionBoxTextureHeight = 16
local uiFont = TE.fonts:getID("Mods/dr2c/Fonts/Galmuri7.ttf")

--- @class dr2c.UI.DrawBorder
--- @field x number @Frame top left position
--- @field y number @Frame top left position
--- @field width number @Frame middle width
--- @field height number @Frame middle height
--- @field size number @Corner size (width & height)
--- @field texture DrawArgTexture?
--- @field textureX number?
--- @field textureY number?
--- @field textureWidth number?
--- @field textureHeight number?
--- @field textureSize number?
--- @field color Color?

--- @param newRenderer TE.Renderer
function CUIDraw.overrideRenderer(newRenderer)
	renderer = newRenderer
end

--- 画一个通用的矩形边框
--- @param args dr2c.UI.DrawBorder
--- @return boolean
function CUIDraw.drawBorder(args)
	-- 绘制顺序：左上、上、右上、左、右、左下、下、右下

	local w = args.width
	local h = args.height
	if w <= 0 or h <= 0 then
		return false
	end

	local x = args.x
	local y = args.y
	local sz = args.size

	local tx = args.textureX or uiBorderTextureX
	local ty = args.textureY or uiBorderTextureY
	local tw = args.textureWidth or uiBorderTextureWidth
	local th = args.textureHeight or uiBorderTextureHeight
	local tsz = args.textureSize or uiBorderTextureSize

	drawRectArgs.texture = args.texture or uiBorderTexture
	drawRectArgs.color = args.color or Color.White

	drawRectSrc.x = tx
	drawRectSrc.y = args.textureY
	drawRectSrc.w = tsz
	drawRectSrc.h = tsz
	drawRectDst.x = x
	drawRectDst.y = y
	drawRectDst.w = sz
	drawRectDst.h = sz
	renderer:drawRect(drawRectArgs)

	drawRectSrc.x = tx + tsz
	drawRectSrc.w = tw - 2 * tsz
	drawRectDst.x = x + sz
	drawRectDst.w = w - 2 * sz
	renderer:drawRect(drawRectArgs)

	drawRectSrc.x = tx + (tw - tsz)
	drawRectSrc.w = tsz
	drawRectDst.x = x - sz + w
	drawRectDst.w = sz
	renderer:drawRect(drawRectArgs)

	drawRectSrc.x = tx
	drawRectSrc.y = ty + tsz
	drawRectSrc.h = th - 2 * tsz
	drawRectDst.x = x
	drawRectDst.y = y + sz
	drawRectDst.w = sz
	drawRectDst.h = h - sz * 2
	renderer:drawRect(drawRectArgs)

	drawRectSrc.x = tx + (tw - tsz)
	drawRectDst.x = x + w - sz
	renderer:drawRect(drawRectArgs)

	drawRectSrc.x = tx
	drawRectSrc.y = ty + (th - tsz)
	drawRectSrc.w = tsz
	drawRectSrc.h = tsz
	drawRectDst.x = x
	drawRectDst.y = y + h - sz
	drawRectDst.h = sz
	renderer:drawRect(drawRectArgs)

	drawRectSrc.x = tsz
	drawRectSrc.w = tw - 2 * tsz
	drawRectDst.x = x + sz
	drawRectDst.w = w - 2 * sz
	renderer:drawRect(drawRectArgs)

	drawRectSrc.x = tw - tsz
	drawRectSrc.w = tsz
	drawRectDst.x = x + w - sz
	drawRectDst.w = sz
	renderer:drawRect(drawRectArgs)

	return true
end

--- @class dr2c.UI.DrawButton
--- @field x number?
--- @field y number?
--- @field text string?
--- @field alignX number?
--- @field alignY number?
--- @field font DrawArgFont?
--- @field scale number?
--- @field width number?
--- @field height number?
--- @field size number?
--- @field hover boolean?
--- @field held boolean?
--- @field border dr2c.UI.DrawBorder?

--- @param args dr2c.UI.DrawButton
--- @return boolean
function CUIDraw.drawButton(args)
	if not (args.x and args.y and args.width and args.height) then
		return false
	end

	local border = args.border
	if border then
		if args.hover then
			border.color = Color.White
		else
			border.color = Color.gray(192)
		end

		if args.held then -- TODO 这啥？
			border.x = args.x
			border.y = args.y
			border.width = args.width
			border.height = args.height
		else
			border.x = args.x
			border.y = args.y
			border.width = args.width
			border.height = args.height
		end

		CUIDraw.drawBorder(args.border)
	end

	local borderSize = border and border.size or uiBorderTextureSize

	do
		drawRectArgs.texture = uiBorderTexture
		if args.hover then
			drawRectArgs.color = Color.White
		else
			drawRectArgs.color = Color.gray(192)
		end
		drawRectSrc.x = 9
		drawRectSrc.y = 0
		drawRectSrc.w = 1
		drawRectSrc.h = 7
		drawRectDst.x = args.x + borderSize
		drawRectDst.y = args.y + borderSize
		drawRectDst.w = args.width - borderSize * 2
		drawRectDst.h = args.height - borderSize * 2
		renderer:drawRect(drawRectArgs)
	end

	drawTextArgs.font = args.font or uiFont
	drawTextArgs.text = tostring(args.text)
	drawTextArgs.alignX = args.alignX or 0
	drawTextArgs.alignY = args.alignY or 0
	drawTextArgs.x = args.x + args.width * 0.5
	drawTextArgs.y = args.y + args.height * 0.5
	-- drawTextArgs.maxWidth = args.width - borderSize * 2
	drawTextArgs.scale = (args.held and 0.8 or 1) * (args.scale or 1)
	drawTextArgs.color = Color.White
	-- drawTextArgs.backgroundColor = Color.rgba(255, 255, 255, 63)
	drawTextArgs.characterSize = 18
	drawTextArgs.shadow = 2
	drawTextArgs.shadowColor = Color.Green
	renderer:drawText(drawTextArgs)

	return true
end

--- @class dr2c.UI.DrawSelectionBox
--- @field x number
--- @field y number
--- @field width number?
--- @field height number?
--- @field boxWidth number
--- @field boxHeight number
--- @field texture DrawArgTexture?
--- @field textureX number?
--- @field textureY number?
--- @field textureWidth number?
--- @field textureHeight number?
--- @field textureOffsetX number?
--- @field textureOffsetY number?
--- @field time number?
--- @field previousState { x: number, y: number, time: number }?

--- @param args dr2c.UI.DrawSelectionBox
function CUIDraw.drawSelectionBox(args)
	local offsetFactor = (math.sin((args.time or Time.getSystemTime()) * 8) + 1) / 16
	local x = args.x
	local y = args.y
	local bw = args.boxWidth
	local bh = args.boxHeight
	local tx = args.textureX or uiSelectionBoxTextureX
	local ty = args.textureY or uiSelectionBoxTextureY
	local tw = args.textureWidth or uiSelectionBoxTextureWidth
	local th = args.textureHeight or uiSelectionBoxTextureHeight
	local w = args.width or tw
	local h = args.height or th
	local tox = args.textureOffsetX or w / 2
	local toy = args.textureOffsetY or h / 2

	drawRectArgs.texture = args.texture or uiSelectionBoxTexture
	drawRectArgs.color = Color.White

	drawRectSrc.x = tx
	drawRectSrc.y = ty
	drawRectSrc.w = tw
	drawRectSrc.h = th

	-- 绘制顺序：左上、右上、左下、右下

	local px, py
	local prev = args.previousState
	if prev then
		local lerpFactor = Math.clamp(0, ((Time.getSystemTime() - prev.time) * 6) ^ 0.375, 1)
		if lerpFactor >= 1 then
			args.previousState = nil
		end

		px = Math_lerp(prev.x, x, lerpFactor) - tox
		py = Math_lerp(prev.y, y, lerpFactor) - toy
	else
		px = x - tox
		py = y - toy
	end

	drawRectDst.x = px - w * offsetFactor
	drawRectDst.y = py - h * offsetFactor
	drawRectDst.w = w
	drawRectDst.h = h
	renderer:drawRect(drawRectArgs)

	drawRectDst.x = px + w * offsetFactor + w + bw
	drawRectDst.w = -w
	renderer:drawRect(drawRectArgs)

	drawRectDst.x = px - w * offsetFactor
	drawRectDst.y = py + h * offsetFactor + h + bh
	drawRectDst.w = w
	drawRectDst.h = -h
	renderer:drawRect(drawRectArgs)

	drawRectDst.x = px + w * offsetFactor + w + bw
	drawRectDst.w = -w
	renderer:drawRect(drawRectArgs)

	return true
end

--- @param e dr2c.E.CRender
TE.events:add(N_("CRenderUI"), function(e)
	renderer = e.renderer
end, "BeginDraw", "Begin")

return CUIDraw
