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

local CSystemInput = require("dr2c.Client.System.Input")

--- @class dr2c.CUICanvas
local CUICanvas = {}

--- @type dr2c.UICanvas?
local canvasSelected

--- @type integer
local canvasLatestIndex = 0
--- @type table<integer, dr2c.UICanvas>
local canvasMap = setmetatable({}, { __mode = "v" })
--- @type boolean
local isMouseControlEnabled = false
--- @type boolean
local isKeyboardControlEnabled = false

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
	-- for _, widget in ipairs(self.widgets) do
	-- 	local update = widget.update
	-- 	if update then
	-- 		update(widget)
	-- 	end
	-- end
end

--- @param self dr2c.UICanvas
--- @param widget dr2c.UIWidget
local function metatableAddWidget(self, widget)
	local widgets = self.widgets
	if List.findFirst(widgets, widget) then
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

	--- @type dr2c.UIWidget?
	local widget
	if type(widgetOrID) == "string" or type(widgetOrID) == "number" then
		widget = List.findFirstValueIf(self.widgets, widgetHasSameID, widgetOrID)
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

	setmetatable(canvas, CUICanvas.metatable)

	canvasMap[canvasLatestIndex] = canvas

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

--- @param l dr2c.UICanvas
--- @param r dr2c.UICanvas
local function compareCanvas(l, r)
	if l.layer ~= r.layer then
		return l.layer > r.layer
	else
		return l.index > r.index
	end
end

--- @return dr2c.UICanvas[]
local function collectCanvasList()
	if next(canvasMap) == nil then
		return Table.empty
	end

	local canvasList = Table.getValueList(canvasMap)
	table.sort(canvasList, compareCanvas)
	return canvasList
end

--- @param canvas dr2c.UICanvas
--- @param mouseX number
--- @param mouseY number
--- @param widgets dr2c.UIWidget[]
--- @return boolean
local function canvasSelectWidgetAtMouse(canvas, mouseX, mouseY, widgets)
	local rectContains = Geometry.luaRectContains

	for _, widget in ipairs(widgets) do
		if widget.widgets then
			if canvasSelectWidgetAtMouse(canvas, mouseX, mouseY, widget.widgets) then
				return true
			end
		elseif rectContains(widget.rectangle, mouseX, mouseY) then
			canvas:selectWidget(widget)

			return true
		end
	end

	return false
end

TE.events:add(N_("CMouseMotion"), function()
	local canvases = collectCanvasList()
	if not canvases[1] then
		return
	end

	local mouseX, mouseY = CSystemInput.getMousePosition()
	for _, canvas in ipairs(canvases) do
		if canvas.visible and canvas:contains(mouseX, mouseY) then
			if canvasSelectWidgetAtMouse(canvas, mouseX, mouseY, canvas.widgets) then
				break
			end
		end
	end
end, "UICanvasHandleMouseControl", "UI")

local function canvasControl(key)
	for _, canvas in ipairs(collectCanvasList()) do
		-- canvas.selectWidget
	end
end

--- @param e Events.E.KeyboardPress
TE.events:add(N_("CKeyboardPress"), function(e)
	if e.data.scanCode == EScanCode.Up then
		canvasControl("selectUp")
	elseif e.data.scanCode == EScanCode.Down then
		canvasControl("selectDown")
	elseif e.data.scanCode == EScanCode.Left then
		canvasControl("selectLeft")
	elseif e.data.scanCode == EScanCode.Right then
		canvasControl("selectRight")
	end
end, "UICanvasHandleKeyboardControl", "UI") -- we don't use key filter, we'd let ui controller key bindings editable in future!

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
			return true
		end

		List.removeIfV(widget.widgets, removeExpiredWidget)
		return false
	end

	for _, canvas in pairs(canvasMap) do
		List.removeIfV(canvas.widgets, removeExpiredWidget)
	end
end, "ClientUICanvasUnloadTypedWidget", "Client")

return CUICanvas
