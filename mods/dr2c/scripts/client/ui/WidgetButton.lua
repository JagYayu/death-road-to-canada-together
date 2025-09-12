--[[
-- @module dr2c.client.ui.WidgetButton
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("tudov.Table")
local Color = require("tudov.Color")

local CUI = require("dr2c.client.ui.UI")
local CUIWidget = require("dr2c.client.ui.Widget")

local metatable = {
	__index = {},
}

--- @param self dr2c.UIWidgetButton
function metatable.__index:draw()
	local rect = self.rectangle

	self.drawButtonArgs.x = rect[1]
	self.drawButtonArgs.y = rect[2]
	self.drawButtonArgs.width = rect[3]
	self.drawButtonArgs.height = rect[4]
	self.drawButtonArgs.border = self.border

	CUI.drawButton(self.drawButtonArgs)
end

--- @param e dr2c.E.CWidget
events:add(CUIWidget.eventCWidget, function(e)
	local args = e.args

	--- @class dr2c.UIWidgetButton : dr2c.UIWidget
	local widget = e.widget

	--- @type dr2c.UI.DrawBorder
	widget.border = {
		x = 0,
		y = 0,
		width = 0,
		height = 0,
		size = args.borderSize,
		texture = args.borderTexture,
		textureX = args.borderTextureX,
		textureY = args.borderTextureY,
		textureWidth = args.borderTextureWidth,
		textureHeight = args.borderTextureHeight,
		textureSize = args.borderTextureSize,
		color = args.borderColor,
	}

	local rectangle = {
		tonumber(args.x) or 0,
		tonumber(args.y) or 0,
		tonumber(args.w) or 0,
		tonumber(args.h) or 0,
	}

	local label = args.label and tostring(args.label) or nil

	--- @type dr2c.UI.DrawButton
	widget.drawButtonArgs = {
		x = rectangle[1],
		y = rectangle[2],
		width = rectangle[3],
		height = rectangle[4],
		text = label,
		alignX = tonumber(args.alignX),
		alignY = tonumber(args.alignY),
		border = widget.border,
	}

	--- @type Rectangle
	widget.rectangle = rectangle

	e.widget = setmetatable(widget, metatable)

	e.initialized = true
end, "Button", "Initialize", CUIWidget.Type.Button)
