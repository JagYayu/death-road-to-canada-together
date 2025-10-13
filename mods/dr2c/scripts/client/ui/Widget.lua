--[[
-- @module dr2c.Client.UI.widget
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")
local Function = require("TE.Function")
local Table = require("TE.Table")

local CRenderUtils = require("dr2c.Client.Render.Utils")

--- @class dr2c.CUIWidget
local CUIWidget = {}

--- @alias dr2c.UIWidgetID string | integer | nil
--- @alias dr2c.UIWidgetType dr2c.CUIWidget.Type

CUIWidget.Type = Enum.sequence({
	Box = 0,
	Button = 1,
	ColorPicker = 2,
	Dialogue = 3,
	Dropdown = 4,
	Flex = 5,
	Grid = 6,
	Label = 7,
	Textbox = 8,
})

CUIWidget.eventCWidget = TE.events:new(N_("CWidgetNew"), {
	"Initialize",
	"Overrides",
})

--- @param widgetType dr2c.UIWidgetType
--- @param args? dr2c.UIWidget.Args
--- @return dr2c.UIWidget
function CUIWidget.new(widgetType, args)
	widgetType = tonumber(widgetType) or CUIWidget.Type.Box

	--- @class dr2c.UIWidget.Args.Border : dr2c.UI.DrawBorder
	--- @field x nil
	--- @field y nil
	--- @field width nil
	--- @field height nil

	--- @class dr2c.UIWidget.Args
	--- @field id? dr2c.UIWidgetID
	--- @field border? dr2c.UIWidget.Args.Border
	--- @field margin? Rectangle
	--- @field padding? Rectangle
	--- @field rectangle? Rectangle
	--- @field selectable? boolean @default true
	--- @field selectUp? dr2c.UIWidgetID | fun(): dr2c.UIWidgetID?
	--- @field selectDown? dr2c.UIWidgetID | fun(): dr2c.UIWidgetID?
	--- @field selectLeft? dr2c.UIWidgetID | fun(): dr2c.UIWidgetID?
	--- @field selectRight? dr2c.UIWidgetID | fun(): dr2c.UIWidgetID?
	args = args or Table.empty

	--- @class dr2c.UIWidget
	--- @field draw? fun(self: self)
	--- @field press? fun(self: self): boolean?
	--- @field release? fun(self: self, active: boolean)
	--- @field id dr2c.UIWidgetID?
	--- @field parent (dr2c.UIWidget | dr2c.UICanvas)?
	--- @field margin Rectangle
	--- @field padding Rectangle
	--- @field rectangle Rectangle
	--- @field children? dr2c.UIWidget[]
	local widget = {
		type = widgetType,
		id = args.id,
		parent = nil,
		border = CRenderUtils.copyDrawBorder(args.border or Table.empty),
		margin = CRenderUtils.copyRectangle(args.margin or Table.empty),
		padding = CRenderUtils.copyRectangle(args.padding or Table.empty),
		rectangle = CRenderUtils.copyRectangle(args.rectangle or Table.empty),
		selectable = args.selectable ~= false,
		selectUp = args.selectUp,
		selectDown = args.selectDown,
		selectLeft = args.selectLeft,
		selectRight = args.selectRight,
	}

	--- @class dr2c.E.CWidget
	--- @field initialized boolean?
	local e = {
		widget = widget,
		args = args,
		key = widgetType,
	}

	TE.events:invoke(CUIWidget.eventCWidget, e, widgetType)

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

--- @param args dr2c.UIWidgetFlex.Args
--- @return dr2c.UIWidgetFlex
function CUIWidget.newFlex(args)
	local widget = CUIWidget.new(CUIWidget.Type.Flex, args)
	--- @cast widget dr2c.UIWidgetFlex
	return widget
end

return CUIWidget
