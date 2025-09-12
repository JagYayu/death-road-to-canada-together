--[[
-- @module dr2c.client.ui.UI
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Color = require("tudov.Color")

local CClient = require("dr2c.client.Module")
local CUI = {}

--- @type Renderer
local renderer

local time

CUI.eventRenderUI = events:new(N_("CRenderUI"), {
	"Begin",
	"Menu",
	"Test",
	"End",
})

local drawRectArgs = DrawRectArgs()
drawRectArgs.texture = images:getID("mods/dr2c/gfx/UI.png")
drawRectArgs.source = { x = 0, y = 0, w = 0, h = 0 }
local drawRectDst = drawRectArgs.destination
local drawRectSrc = drawRectArgs.source

local drawTextArgs = DrawTextArgs()

--- @param drawTexture DrawArgTexture
function CUI.setTexture(drawTexture)
	drawRectArgs.texture = drawTexture
end

local uiTexture = images:getID("mods/dr2c/gfx/UI.png")
local uiTextureX = 0
local uiTextureY = 0
local uiTextureWidth = 9
local uiTextureHeight = 9
local uiTextureSize = 4
local uiFont = fonts:getID("mods/dr2c/fonts/Galmuri7.ttf")

--- @class dr2c.UI.DrawBorder
--- @field x number? @Frame top left position
--- @field y number? @Frame top left position
--- @field width number? @Frame middle width
--- @field height number? @Frame middle height
--- @field size number? @Corner size (width & height)
--- @field texture DrawArgTexture?
--- @field textureX number?
--- @field textureY number?
--- @field textureWidth number?
--- @field textureHeight number?
--- @field textureSize number?
--- @field color Color?

function CUI.setRenderer() end

--- 画一个通用的矩形边框
--- @param args dr2c.UI.DrawBorder
function CUI.drawBorder(args)
	-- 绘制顺序：左上、上、右上、左、右、左下、下、右下

	local x = args.x
	local y = args.y
	local w = args.width
	local h = args.height
	local sz = args.size
	if not (x or y or w or h) then
		return
	end

	--- @cast x number
	--- @cast y number
	--- @cast w number
	--- @cast h number
	--- @cast sz number

	local tx = args.textureX or uiTextureX
	local ty = args.textureY or uiTextureY
	local tw = args.textureWidth or uiTextureWidth
	local th = args.textureHeight or uiTextureHeight
	local tsz = args.textureSize or uiTextureSize

	drawRectArgs.texture = args.texture or uiTexture
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
	drawRectDst.h = h
	renderer:drawRect(drawRectArgs)

	drawRectSrc.x = tx + (tw - tsz)
	drawRectDst.x = x - sz + w
	renderer:drawRect(drawRectArgs)

	drawRectSrc.x = tx
	drawRectSrc.y = ty + (th - tsz)
	drawRectSrc.w = tsz
	drawRectSrc.h = tsz
	drawRectDst.x = x
	drawRectDst.y = y + sz + h
	drawRectDst.h = sz
	renderer:drawRect(drawRectArgs)

	drawRectSrc.x = tsz
	drawRectSrc.w = tw - 2 * tsz
	drawRectDst.x = x + sz
	drawRectDst.w = w - 2 * sz
	renderer:drawRect(drawRectArgs)

	drawRectSrc.x = tw - tsz
	drawRectSrc.w = tsz
	drawRectDst.x = x - sz + w
	drawRectDst.w = sz
	renderer:drawRect(drawRectArgs)
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
--- @field held boolean?
--- @field border dr2c.UI.DrawBorder?

--- @param args dr2c.UI.DrawButton
function CUI.drawButton(args)
	if not (args.x and args.y) then
		return
	end

	local border = args.border
	if border then
		if args.held then
			border.x = args.x
			border.y = args.y
			border.width = args.width
			border.height = args.height
			border.size = uiTextureSize
		else
			border.x = args.x
			border.y = args.y
			border.width = args.width
			border.height = args.height
			border.size = uiTextureSize
		end

		CUI.drawBorder(args.border)
	end

	local borderSize = border and border.size or uiTextureSize

	do
		drawRectArgs.texture = uiTexture
		drawRectArgs.color = Color.White
		drawRectSrc.x = 9
		drawRectSrc.y = 1
		drawRectSrc.w = 1
		drawRectSrc.h = 6
		drawRectDst.x = args.x + borderSize
		drawRectDst.y = args.y + borderSize
		drawRectDst.w = args.width - borderSize * 2
		drawRectDst.h = args.height - borderSize
		renderer:drawRect(drawRectArgs)
	end

	drawTextArgs.font = args.font or uiFont
	drawTextArgs.text = tostring(args.text)
	drawTextArgs.alignX = args.alignX or 0
	drawTextArgs.alignY = args.alignY or 0
	drawTextArgs.x = args.x + args.width * 0.5 + borderSize
	drawTextArgs.y = args.y + args.height * 0.5 + borderSize
	-- drawTextArgs.maxWidth = args.width - borderSize * 2
	drawTextArgs.scale = (args.held and 0.8 or 1) * (args.scale or 1)
	-- drawTextArgs.color = Color.Red
	-- drawTextArgs.backgroundColor = Color.rgba(255, 255, 255, 63)
	drawTextArgs.characterSize = 18
	drawTextArgs.shadow = 2
	drawTextArgs.shadowColor = Color.Green
	renderer:drawText(drawTextArgs)
end

--- @param e dr2c.E.CRender
events:add(N_("CRender"), function(e)
	time = Time:getSystemTime()
	renderer = e.renderer

	--- @alias dr2c.E.CRenderUI dr2c.E.CRender
	events:invoke(CUI.eventRenderUI, e)
end, N_("RenderUI"), "UI")

return CUI
