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

--- 画一个通用的矩形框
--- @param x number @Frame top left position
--- @param y number @Frame top left position
--- @param size number @Corner size
--- @param width number @Frame width
--- @param height number @Frame height
function CUI.drawFrame(x, y, size, width, height, color)
	
end

function CUI.addText(x, y, text)
	local rect = CClient.getRenderer():drawDebugText(x, y, text)
end

--- @param x number
--- @param y number
--- @param text string?
--- @param size number?
--- @param width number?
--- @param height number?
function CUI.drawButton(x, y, text, size, width, height, color)
	size = size or 8

	drawTextArgs.text = tostring(text)
	drawTextArgs.font = 2
	drawTextArgs.x = x
	drawTextArgs.y = y

	local txtX, txtY, txtW, txtH = renderer:drawText(drawTextArgs)

	CUI.drawFrame(txtX - size, txtY - size, size, txtW, txtH, color)
end

--- @param e dr2c.E.CRender
events:add(N_("CRender"), function(e)
	renderer = e.renderer

	CUI.drawButton(100, 100, "你好 hello world", nil, 168, 64, Color.White)

	--- @alias dr2c.E.CRenderUI dr2c.E.CRender
	events:invoke(CUI.eventRenderUI, e)
end, N_("RenderUI"), "UI")

return CUI
