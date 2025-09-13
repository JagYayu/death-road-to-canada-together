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
local CUIWidgetBox = require("dr2c.client.ui.WidgetBox")

local CUIWidgetButton = {}

--- @param self dr2c.UIWidgetButton
local function draw(self)
	local rect = self.rectangle

	self.drawButtonArgs.x = rect[1]
	self.drawButtonArgs.y = rect[2]
	self.drawButtonArgs.width = rect[3]
	self.drawButtonArgs.height = rect[4]
	self.drawButtonArgs.border = self.border

	CUI.drawButton(self.drawButtonArgs)
end

CUIWidgetButton.metatable = {
	__index = {
		draw = draw,
	},
}

--- @param e dr2c.E.CWidget
events:add(CUIWidget.eventCWidget, function(e)
	--- @class dr2c.UIWidgetButton.Args : dr2c.UIWidget.Args
	--- @field alignX? number
	--- @field alignY? number
	--- @field label? string
	local args = e.args

	--- @class dr2c.UIWidgetButton : dr2c.UIWidget
	local widget = e.widget

	local label = args.label and tostring(args.label) or nil

	--- @type dr2c.UI.DrawButton
	widget.drawButtonArgs = {
		x = widget.rectangle[1],
		y = widget.rectangle[2],
		width = widget.rectangle[3],
		height = widget.rectangle[4],
		text = label,
		alignX = tonumber(args.alignX) or 0.5,
		alignY = tonumber(args.alignY) or 0.5,
		border = widget.border,
	}

	e.widget = setmetatable(widget, CUIWidgetButton.metatable)

	e.initialized = true
end, "Button", "Initialize", CUIWidget.Type.Button)

return CUIWidgetButton
