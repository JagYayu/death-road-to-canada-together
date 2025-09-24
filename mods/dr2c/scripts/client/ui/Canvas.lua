--[[
	-- @module dr2c.client.ui.Canvas
	-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("tudov.Table")
local Geometry = require("tudov.Geometry")
local Enum = require("tudov.Enum")

local CSystemInput = require("dr2c.client.system.Input")
local CUIWidget = require("dr2c.client.ui.Widget")

--- @class dr2c.CUICanvas
local CUICanvas = {}

--- @type dr2c.UICanvas?
local canvasSelected
local canvasCount = 0

--- @type integer
local canvasLatestIndex = 0
--- @type table<integer, dr2c.UICanvas>
local canvasMap = setmetatable({}, { __mode = "k" })

canvasLatestIndex = persist("canvasLatestIndex", function()
	return canvasLatestIndex
end)

canvasMap = persist("canvasMap", function()
	return canvasMap
end)

function CUICanvas.getSelectedCanvas()
	return canvasSelected
end

--- @param self dr2c.UICanvas
local function metatableCanvasDraw(self)
	for _, widget in ipairs(self.widgets) do
		local draw = widget.draw
		if draw then
			draw(widget)
		end
	end
end

--- @param self dr2c.UICanvas
local function metatableCanvasUpdate(self)
	for _, widget in ipairs(self.widgets) do
		local update = widget.update
		if update then
			update(widget)
		end
	end
end

--- @param self dr2c.UICanvas
--- @param widget dr2c.UIWidget
local function metatableAddWidget(self, widget)
	local widgets = self.widgets
	if Table.listFindFirst(widgets, widget) then
		return false
	end

	widgets[#widgets + 1] = widget
	return true
end

--- @param widget dr2c.UIWidget
--- @param widgetID dr2c.UIWidgetID
local function widgetHasSameID(widget, _, widgetID)
	return widget.id == widgetID
end

--- @param self dr2c.UICanvas
--- @param widgetOrID dr2c.UIWidget | dr2c.UIWidgetID
local function metatableSelectWidget(self, widgetOrID)
	local type = type

	local widget
	if type(widgetOrID) == "string" or type(widgetOrID) == "number" then
		local _
		_, widget = Table.listFindFirstIf(self.widgets, widgetHasSameID, widgetOrID)
	elseif type(widgetOrID) == "table" then
		widget = widgetOrID
	end

	if widget then
		self.selectedWidget = widget

		return true
	else
		return false
	end
end

--- @param self dr2c.UICanvas
--- @param x number
--- @param y number
local function metatableContains(self, x, y)
	local windowW, windowH = (self.window or engine.primaryWindow):getSize()

	return x > (self.x or 0) * windowW
		and y > (self.y or 0) * windowH
		and x < (self.x or 0) * windowW + (self.w or 1) * windowW
		and y < (self.y or 0) * windowH + (self.h or 1) * windowH
end

CUICanvas.metatable = {
	__index = {
		draw = metatableCanvasDraw,
		update = metatableCanvasUpdate,
		addWidget = metatableAddWidget,
		selectWidget = metatableSelectWidget,
		contains = metatableContains,
	},
	__gc = function()
		canvasCount = canvasCount - 1
	end,
}

--- @param args table?
--- @return dr2c.UICanvas
function CUICanvas.new(args)
	args = args or Table.empty

	canvasLatestIndex = canvasLatestIndex + 1

	--- @class dr2c.UICanvas
	--- @field draw fun(self: self)
	--- @field update fun(self: self)
	--- @field addWidget fun(self: self, widget: dr2c.UIWidget)
	--- @field selectWidget fun(self: self, widgetOrID: dr2c.UIWidget | dr2c.UIWidgetID)
	--- @field contains fun(self: self, x: number, y: number)
	--- @field x number? @Relative value of window size (0 ~ 1)
	--- @field y number? @Relative value of window size (0 ~ 1)
	--- @field w number? @Relative value of window size (0 ~ 1)
	--- @field h number? @Relative value of window size (0 ~ 1)
	--- @field visible boolean
	--- @field layer number
	--- @field index integer
	--- @field widgets dr2c.UIWidget[]
	--- @field selectedWidget dr2c.UIWidget?
	--- @field window TE.Window?
	local canvas = {
		x = tonumber(args.x),
		y = tonumber(args.y),
		w = tonumber(args.w),
		h = tonumber(args.h),
		visible = args.visible ~= false,
		layer = tonumber(args.layer) or 0,
		index = canvasLatestIndex,
		widgets = {},
		selectedWidget = nil,
		window = args.window or TE.engine.primaryWindow,
	}

	Table.setProxyMetatable(canvas, CUICanvas.metatable)

	canvasCount = canvasCount + 1
	canvasMap[canvasLatestIndex] = canvas

	return canvas
end

--- @param l dr2c.UICanvas
--- @param r dr2c.UICanvas
local function compareCanvas(l, r)
	if l.layer ~= r.layer then
		return l.layer > r.layer
	else
		return l.index > r.index
	end
end

--- @param canvas dr2c.UICanvas
--- @param mouseX number
--- @param mouseY number
--- @return boolean
local function canvasSelectWidgetAtMouse(canvas, mouseX, mouseY)
	local rectContains = Geometry.luaRectContains

	for _, widget in ipairs(canvas.widgets) do
		if rectContains(widget.rectangle, mouseX, mouseY) then
			canvas:selectWidget(widget)

			return true
		end
	end

	return false
end

local function handleCanvasesSelection(canvases)
	local mouseX, mouseY = CSystemInput.getMousePosition()

	for _, canvas in ipairs(canvases) do
		if canvas.visible and canvas:contains(mouseX, mouseY) then
			if canvasSelectWidgetAtMouse(canvas, mouseX, mouseY) then
				break
			end
		end
	end
end

TE.events:add(N_("CUpdate"), function(e)
	if not next(canvasMap) then
		return
	end

	--- @type dr2c.UICanvas[]
	local canvasList = Table.getValueList(canvasMap, canvasCount)

	table.sort(canvasList, compareCanvas)

	handleCanvasesSelection(canvasList)
end, "UICanvasHandleInput", "Inputs", nil, 1)

TE.events:add("ScriptUnload", function(e)
	--- If a module representing a certain widget type is unloaded, call this function to remove all corresponding widgets within all canvases.
	--- However it may result in errors if some widget module don't have `UIWidgetType` field.

	local widgetType = e.data.module.UIWidgetType
	if not (widgetType and Enum.hasValue(CUIWidget.Type, widgetType)) then
		return
	end

	--- @param widget dr2c.UIWidget
	local function func(widget, seenSet)
		if widget.type == widgetType then
			return true
		end

		Table.listRemoveIf(widget.widgets, func, seenSet)
		return false
	end

	for _, canvas in pairs(canvasMap) do
		Table.listRemoveIf(canvas.widgets, func, {})
	end
end, "ClientUICanvasUnloadTypedWidget", "Client")

return CUICanvas
