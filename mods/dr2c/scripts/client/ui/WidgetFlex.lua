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

--- @class dr2c.CUIWidgetFlex
local CUIWidgetFlex = {}

CUIWidgetFlex.UIWidgetType = CUIWidget.Type.Flex

--- @param widget dr2c.UIWidget
local function addChild(self, widget)
	self.widgets[#self.widgets + 1] = widget

	-- self.cache = nil
end

--- @param index integer
--- @param weight number
local function setWeight(self, index, weight)
	weight = tonumber(weight) or 1

	self.weights[index] = weight
end

--- @param self dr2c.UIWidgetFlex
local function draw(self)
	for _, drawFunction in ipairs(self.widgetsDrawFunctions) do
		drawFunction()
	end
end

--- @param self dr2c.UIWidgetFlex
local function update(self)
	self.widgetsDrawFunctions = {}

	local x = self.rectangle[1]
	local y = self.rectangle[2]
	local w = self.rectangle[3]
	local h = self.rectangle[4]

	if self.vertical then
		local hh = h / #self.widgets

		for i, widget in ipairs(self.widgets) do
			if widget.draw then
				local cy = y + (i - 1) * hh
				local ch = hh

				self.widgetsDrawFunctions[#self.widgetsDrawFunctions + 1] = function()
					local rect = widget.rectangle

					rect[1] = x
					rect[2] = cy
					rect[3] = w
					rect[4] = ch

					widget:draw()
				end
			end
		end
	else
		local ww = w / #self.widgets

		for i, widget in ipairs(self.widgets) do
			if widget.draw then
				local cx = x + (i - 1) * ww
				local cw = ww

				self.widgetsDrawFunctions[#self.widgetsDrawFunctions + 1] = function()
					local rect = widget.rectangle

					rect[1] = cx
					rect[2] = y
					rect[3] = cw
					rect[4] = h

					widget:draw()
				end
			end
		end
	end
end

CUIWidgetFlex.metatable = {
	__index = {
		addChild = addChild,
		draw = draw,
		setWeight = setWeight,
		update = update,
	},
}

--- @param e dr2c.E.CWidget
TE.events:add(CUIWidget.eventCWidget, function(e)
	--- @class dr2c.UIWidgetFlex.Args : dr2c.UIWidget.Args
	--- @field marginX? number
	--- @field marginY? number
	--- @field marginWidth? number
	--- @field marginHeight? number
	--- @field x? number
	--- @field y? number
	--- @field width? number
	--- @field height? number
	--- @field vertical? boolean
	--- @field weights? number[]
	local args = e.args

	--- @class dr2c.UIWidgetFlex : dr2c.UIWidget
	--- @field addChild fun(self: self, widget: dr2c.UIWidget)
	local widget = e.widget

	--- @type table | false
	widget.cache = false

	widget.vertical = not not args.vertical

	--- @type dr2c.UIWidget[]
	widget.widgets = {}

	if type(args.weights) == "table" then
		local weights = Table.copyArray(args.weights)
		for index, value in ipairs(widget.weights) do
			weights[index] = tonumber(value) or 0
		end

		widget.weights = weights
	else
		widget.weights = {}
	end

	widget.widgetsDrawFunctions = {}

	e.widget = setmetatable(widget, CUIWidgetFlex.metatable)

	e.initialized = true
end, "InitializeFlex", "Initialize", CUIWidget.Type.Flex)

return CUIWidgetFlex
