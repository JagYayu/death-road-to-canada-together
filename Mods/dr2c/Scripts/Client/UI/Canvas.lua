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

local Table = require("TE.Table")
local Geometry = require("TE.Geometry")
local List = require("TE.List")
local Utility = require("TE.Utility")

local CSystemInput = require("dr2c.Client.System.Input")
local GUtilsAlgorithms = require("dr2c.Shared.Utils.Algorithms")

--- @class dr2c.CUICanvas
local CUICanvas = {}

--- @type dr2c.UICanvas?
local canvasSelected

--- @type integer
local canvasLatestIndex = 0
--- @type table<integer, dr2c.UICanvas>
local canvasMap = setmetatable({}, { __mode = "v" })
--- @type dr2c.UICanvas[]?
local canvasList = nil
--- @type boolean
local isMouseControlEnabled = true
--- @type boolean
local isKeyboardControlEnabled = true

canvasLatestIndex = persist("canvasLatestIndex", function()
	return canvasLatestIndex
end)

canvasMap = persist("canvasMap", function()
	return canvasMap
end)

--- @param l dr2c.UICanvas
--- @param r dr2c.UICanvas
local function compareCanvas(l, r)
	if l.layer ~= r.layer then
		return l.layer > r.layer
	else
		return l.index > r.index
	end
end

local function getCanvasList()
	if not canvasList then
		canvasList = Table.getValueList(canvasMap)
		table.sort(canvasList, compareCanvas)
	end

	return canvasList
end

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
--- @param widget dr2c.UIWidget
local function metatableAddWidget(self, widget)
	local widgets = self.widgets
	if List.findFirst(widgets, widget) then
		return false
	end

	widget.parent = self
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

	--- @type dr2c.UIWidget?
	local widget
	if type(widgetOrID) == "string" or type(widgetOrID) == "number" then
		widget = List.findFirstValueIfV(self.widgets, widgetHasSameID, widgetOrID)
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
	local windowW, windowH = (self.window or TE.engine.primaryWindow):getSize()

	return x > (self.x or 0) * windowW
		and y > (self.y or 0) * windowH
		and x < (self.x or 0) * windowW + (self.w or 1) * windowW
		and y < (self.y or 0) * windowH + (self.h or 1) * windowH
end

CUICanvas.metatable = {
	__index = {
		draw = metatableCanvasDraw,
		-- update = metatableCanvasUpdate,
		addWidget = metatableAddWidget,
		selectWidget = metatableSelectWidget,
		contains = metatableContains,
	},
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
	--- @field selectWidget fun(self: self, widgetOrID: dr2c.UIWidget | dr2c.UIWidgetID): boolean
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

	setmetatable(canvas, CUICanvas.metatable)

	canvasMap[canvasLatestIndex] = canvas
	canvasList = nil

	return canvas
end

function CUICanvas.getMouseControlEnabled()
	return isMouseControlEnabled
end

function CUICanvas.getKeyboardControlEnabled()
	return isKeyboardControlEnabled
end

--- @param value boolean
function CUICanvas.setMouseControlEnabled(value)
	isMouseControlEnabled = not not value
end

--- @param value boolean
function CUICanvas.setKeyboardControlEnabled(value)
	isKeyboardControlEnabled = not not value
end

--- @param canvas dr2c.UICanvas
--- @param mouseX number
--- @param mouseY number
--- @param widgets dr2c.UIWidget[]
--- @return dr2c.UIWidget?
local function canvasFindWidgetAtMouse(canvas, mouseX, mouseY, widgets)
	local rectContains = Geometry.luaRectContains

	for _, widget in ipairs(widgets) do
		if widget.children then
			local selection = canvasFindWidgetAtMouse(canvas, mouseX, mouseY, widget.children)
			if selection then
				return selection
			end
		elseif rectContains(widget.rectangle, mouseX, mouseY) then
			return widget
		end
	end
end

--- @param mouseX number
--- @param mouseY number
--- @return dr2c.UICanvas?
--- @return dr2c.UIWidget?
function CUICanvas.mouseFindWidget(mouseX, mouseY)
	for _, canvas in ipairs(getCanvasList()) do
		if canvas.visible and canvas:contains(mouseX, mouseY) then
			local widget = canvasFindWidgetAtMouse(canvas, mouseX, mouseY, canvas.widgets)
			if widget then
				return canvas, widget
			end
		end
	end
end

--- @param mouseX number
--- @param mouseY number
--- @return boolean
function CUICanvas.mouseSelect(mouseX, mouseY)
	local canvas, widget = CUICanvas.mouseFindWidget(mouseX, mouseY)
	if canvas and widget then
		return canvas:selectWidget(widget)
	else
		return false
	end
end

--- @param mouseX number
--- @param mouseY number
--- @return boolean
function CUICanvas.mousePress(mouseX, mouseY)
	local canvas, widget = CUICanvas.mouseFindWidget(mouseX, mouseY)
	if canvas and widget and canvas:selectWidget(widget) and widget.press then
		return not not widget:press()
	else
		return false
	end
end

--- @param widgets dr2c.UIWidget[]
--- @param target dr2c.UIWidget
--- @return boolean
local function releaseWidgets(widgets, target)
	local activated = false

	for _, widget in ipairs(widgets) do
		if widget.children and releaseWidgets(widget.children, target) then
			activated = true
		end

		if widget.release and widget:release(widget == target) then
			activated = true
		end
	end

	return activated
end

--- @param mouseX number
--- @param mouseY number
--- @return boolean
function CUICanvas.mouseRelease(mouseX, mouseY)
	local canvas, widget = CUICanvas.mouseFindWidget(mouseX, mouseY)
	if canvas and widget and canvas:selectWidget(widget) then
		return releaseWidgets(canvas.widgets, widget)
	else
		return false
	end
end

--- @param widget dr2c.UIWidget
--- @param id dr2c.UIWidgetID
--- @return boolean
local function isWidgetIDEquals(widget, _, id)
	return widget.id == id
end

--- @param canvas dr2c.UICanvas
--- @param widgets dr2c.UIWidget[]
--- @param what any
--- @return boolean
local function canvasControlSelectByID(canvas, widgets, what)
	local id = Utility.evaluate(what)
	if id then
		local widget = List.findFirstValueIfV(widgets, isWidgetIDEquals, id)
		if widget then
			return canvas:selectWidget(widget)
		end
	end

	return false
end

--- @param angle number
--- @param canvas dr2c.UICanvas
--- @param selectedWidget dr2c.UIWidget
--- @param widgets dr2c.UIWidget[]
--- @return boolean
local function canvasControlSmartSelect(angle, canvas, selectedWidget, widgets)
	local x = selectedWidget.rectangle[1]
	local y = selectedWidget.rectangle[2]

	local bestWidget = GUtilsAlgorithms.directionalNavigation({
		angle = angle,
		items = widgets,
		--- @param w dr2c.UIWidget
		locate = function(w)
			if not w.selectable or w == selectedWidget or (w.id ~= nil and w.id == selectedWidget.id) then
				return
			end

			local rect = w.rectangle
			return rect[1] - x, rect[2] - y
		end,
	})

	if bestWidget then
		--- @cast bestWidget dr2c.UIWidget
		canvas:selectWidget(bestWidget)

		return true
	else
		return false
	end
end

--- @param key string?
--- @param angle number
local function canvasControl(key, angle)
	for _, canvas in ipairs(getCanvasList()) do
		local widget = canvas and canvas.visible and canvas.selectedWidget
		local widgets = widget and widget.parent and widget.parent.children
		if widgets then
			--- @cast widget dr2c.UIWidget

			if key and canvasControlSelectByID(canvas, widgets, widget[key]) then
				return true
			end

			if canvasControlSmartSelect(angle, canvas, widget, widgets) then
				return true
			end
		end
	end

	return false
end

--- @return boolean
function CUICanvas.selectUp()
	return canvasControl("selectUp", math.pi / 2)
end

--- @return boolean
function CUICanvas.selectDown()
	return canvasControl("selectDown", -math.pi / 2)
end

--- @return boolean
function CUICanvas.selectLeft()
	return canvasControl("selectLeft", math.pi)
end

--- @return boolean
function CUICanvas.selectRight()
	return canvasControl("selectRight", 0)
end

--- @param angle number
--- @return boolean
function CUICanvas.selectDirectional(angle)
	return canvasControl(nil, angle)
end

--- @param e dr2c.E.CMouseMotion
TE.events:add(N_("CMouseMotion"), function(e)
	if isMouseControlEnabled then
		CUICanvas.mouseSelect(e.data.x, e.data.y)
	end
end, "UICanvasHandleMouseControlMotion", "UI")

--- @param e dr2c.E.CMousePress
TE.events:add(N_("CMousePress"), function(e)
	if isMouseControlEnabled then
		if e.data.button == EMouseButton.Left then
			CUICanvas.mousePress(e.data.x, e.data.y)
		end
	end
end, "UICanvasHandleMouseControlRelease", "UI")

--- @param e dr2c.E.CMouseRelease
TE.events:add(N_("CMouseRelease"), function(e)
	if isMouseControlEnabled then
		if e.data.button == EMouseButton.Left then
			CUICanvas.mouseRelease(e.data.x, e.data.y)
		end
	end
end, "UICanvasHandleMouseControlRelease", "UI")

--- @param e Events.E.KeyboardPress
TE.events:add(N_("CKeyboardPress"), function(e)
	if isKeyboardControlEnabled then
		if e.data.scanCode == EScanCode.Up then
			CUICanvas.selectUp()
		elseif e.data.scanCode == EScanCode.Down then
			CUICanvas.selectDown()
		elseif e.data.scanCode == EScanCode.Left then
			CUICanvas.selectLeft()
		elseif e.data.scanCode == EScanCode.Right then
			CUICanvas.selectRight()
		end
	end
end, "UICanvasHandleKeyboardControl", "UI") -- we don't use key filter, we'd let ui controller key bindings editable in future!

--- @param e Events.E.ScriptUnload
TE.events:add("ScriptUnload", function(e)
	--- If a module representing a certain widget type is unloaded,
	--- call this function to remove all corresponding widgets within all canvases.

	local widgetType = e.data.module.UIWidgetType
	if not widgetType then
		return
	end

	local seenSet = {}

	--- @param widget dr2c.UIWidget
	local function removeExpiredWidget(widget)
		if seenSet[widget] or widget.type == widgetType then
			canvasList = nil
			return true
		end

		List.removeIfV(widget.children, removeExpiredWidget)
		return false
	end

	for _, canvas in pairs(canvasMap) do
		List.removeIfV(canvas.widgets, removeExpiredWidget)
	end
end, "ClientUICanvasRemoveTypedWidgets", "Client")

return CUICanvas
