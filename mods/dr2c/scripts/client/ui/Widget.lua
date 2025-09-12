--[[
-- @module dr2c.client.ui.Widget
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("tudov.Enum")
local Function = require("tudov.Function")
local Table = require("tudov.Table")

--- @class dr2c.CUIWidget
local CUIWidget = {}

--- @alias dr2c.UIWidgetType dr2c.CUIWidget.Type

CUIWidget.Type = Enum.sequence({
	Box = 0,
	Button = 1,
	ColorPicker = 2,
	Container = 3,
	Dialogue = 4,
	Dropdown = 5,
	FlexHorizontal = 6,
	FlexVertical = 7,
	Grid = 8,
	Label = 9,
	Textbox = 10,
})

CUIWidget.eventCWidget = events:new(N_("CWidgetNew"), {
	"Initialize",
	"Overrides",
})

--- @param widgetType dr2c.UIWidgetType
--- @param args table?
--- @return dr2c.UIWidget
function CUIWidget.new(widgetType, args)
	args = args or Table.empty

	--- @class dr2c.UIWidget
	--- @field draw? fun(self: self, renderer: Renderer)
	--- @field margin Rectangle
	--- @field padding Rectangle
	--- @field rect? Rectangle
	local widget = {
		margin = args.margin or { 0, 0, 0, 0 },
		padding = args.padding or { 0, 0, 0, 0 },
		type = widgetType,
	}

	--- @class dr2c.E.CWidget
	--- @field initialized boolean?
	local e = {
		widget = widget,
		args = args,
		key = widgetType,
	}

	events:invoke(CUIWidget.eventCWidget, e, widgetType)

	if not e.initialized then
		error(("Invalid widget type %s"):format(widgetType), 2)
	end

	return widget
end

--- @param args table?
--- @return dr2c.UIWidgetButton
function CUIWidget.newButton(args)
	local widget = CUIWidget.new(CUIWidget.Type.Button, args)
	--- @cast widget dr2c.UIWidgetButton
	return widget
end

--- @param args table?
--- @return dr2c.UIWidgetFlexHorizontal
function CUIWidget.newContainer(args)
	local widget = CUIWidget.new(CUIWidget.Type.Container, args)
	--- @cast widget dr2c.UIWidgetFlexHorizontal
	return widget
end

return CUIWidget
