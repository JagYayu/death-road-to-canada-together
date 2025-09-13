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

local CRenderUtils = require("dr2c.client.render.Utils")

--- @class dr2c.CUIWidget
local CUIWidget = {}

--- @alias dr2c.UIWidgetType dr2c.CUIWidget.Type

CUIWidget.Type = Enum.sequence({
	Box = 0,
	Button = 1,
	ColorPicker = 2,
	Dialogue = 3,
	Dropdown = 4,
	FlexHorizontal = 5,
	FlexVertical = 6,
	Grid = 7,
	Label = 8,
	Textbox = 9,
})

CUIWidget.eventCWidget = events:new(N_("CWidgetNew"), {
	"Initialize",
	"Overrides",
})

--- @param widgetType dr2c.UIWidgetType
--- @param args? dr2c.UIWidget.Args
--- @return dr2c.UIWidget
function CUIWidget.new(widgetType, args)
	widgetType = tonumber(widgetType) or CUIWidget.Type.Box

	--- @class dr2c.UIWidget.Args
	--- @field border? dr2c.UI.DrawBorder
	--- @field margin? Rectangle
	--- @field padding? Rectangle
	--- @field rectangle? Rectangle
	args = args or Table.empty

	--- @class dr2c.UIWidget
	--- @field draw fun(self: self, renderer: Renderer)
	--- @field margin Rectangle
	--- @field padding Rectangle
	--- @field rectangle Rectangle
	local widget = {
		type = widgetType,
		border = CRenderUtils.copyDrawBorder(args.border or Table.empty),
		margin = CRenderUtils.copyRectangle(args.margin or Table.empty),
		padding = CRenderUtils.copyRectangle(args.padding or Table.empty),
		rectangle = CRenderUtils.copyRectangle(args.rectangle or Table.empty),
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

--- @param args dr2c.UIWidgetButton.Args
--- @return dr2c.UIWidgetButton
function CUIWidget.newButton(args)
	local widget = CUIWidget.new(CUIWidget.Type.Button, args)
	--- @cast widget dr2c.UIWidgetButton
	return widget
end

--- @param args dr2c.UIWidgetFlexHorizontal.Args
--- @return dr2c.UIWidgetFlexHorizontal
function CUIWidget.newFlexHorizontal(args)
	local widget = CUIWidget.new(CUIWidget.Type.FlexHorizontal, args)
	--- @cast widget dr2c.UIWidgetFlexHorizontal
	return widget
end

return CUIWidget
