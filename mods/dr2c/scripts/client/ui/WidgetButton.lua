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

local CUIWidget = require("dr2c.client.ui.Widget")

local metatable = {
	__index = {},
}

--- @param self dr2c.UIWidgetButton
--- @param renderer Renderer
function metatable.__index:draw(renderer)
	local rect = self.rectangle

	local drawRectArgs = DrawRectArgs()
	local destination = drawRectArgs.destination

	destination.x = rect[1]
	destination.y = rect[2]
	destination.w = rect[3]
	destination.h = rect[4]

	renderer:drawRect(drawRectArgs)
end

--- @param e dr2c.E.CWidget
events:add(CUIWidget.eventCWidget, function(e)
	--- @class dr2c.UIWidgetButton : dr2c.UIWidget
	local widget = e.widget

	widget.rectangle = {
		tonumber(e.args.x) or 0,
		tonumber(e.args.y) or 0,
		tonumber(e.args.w) or 0,
		tonumber(e.args.h) or 0,
	}

	e.widget = setmetatable(widget, metatable)

	e.initialized = true
end, "Button", "Initialize", CUIWidget.Type.Button)
