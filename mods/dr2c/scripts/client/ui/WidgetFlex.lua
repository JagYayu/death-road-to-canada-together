--[[
-- @module dr2c.client.ui.WidgetFlexHorizontal
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

--- @param widget dr2c.UIWidget
function metatable.__index:addChild(widget)
	self.children[#self.children + 1] = widget

	-- self.cache = nil
end

--- @param index integer
--- @param weight number
function metatable.__index:setWeight(index, weight)
	weight = tonumber(weight) or 1

	self.weights[index] = weight
end

--- @param self dr2c.UIWidgetFlexHorizontal
--- @param renderer Renderer
function metatable.__index:draw(renderer)
	local x = self.rect[1]
	local y = self.rect[2]
	local w = self.rect[3]
	local h = self.rect[4]

	for i, child in ipairs(self.children) do
		if child.draw then
			local rect = child.rect
			if not rect then
				rect = { 0, 0, 0, 0 }
				child.rect = rect
			end

			rect[1] = x + (i - 1) * w
			rect[2] = y
			rect[3] = w
			rect[4] = h

			child:draw(renderer)
		end
	end
end

--- @param e dr2c.E.CWidget
events:add(CUIWidget.eventCWidget, function(e)
	--- @class dr2c.UIWidgetFlexHorizontal : dr2c.UIWidget
	--- @field addChild fun(self: self)
	local widget = e.widget

	--- @type table | false
	widget.cache = false

	--- @type dr2c.UIWidget[]
	widget.children = {}

	--- @type Rectangle
	widget.margin = {
		tonumber(e.args.marginX) or 0,
		tonumber(e.args.marginY) or 0,
		tonumber(e.args.marginW) or 0,
		tonumber(e.args.marginH) or 0,
	}

	--- @type Rectangle
	widget.rect = {
		tonumber(e.args.x) or 0,
		tonumber(e.args.y) or 0,
		tonumber(e.args.w) or 0,
		tonumber(e.args.h) or 0,
	}

	widget.weights = {}

	e.widget = setmetatable(widget, metatable)

	e.initialized = true
end, "Container", "Initialize", CUIWidget.Type.FlexHorizontal)
