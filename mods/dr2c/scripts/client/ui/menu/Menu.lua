--[[
-- @module dr2c.Client.UI.Menu
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")
local Table = require("TE.Table")

local CUI = require("dr2c.Client.UI.UI")
local CUIDraw = require("dr2c.Client.UI.Draw")
local CUICanvas = require("dr2c.Client.UI.Canvas")
local CWorldSession = require("dr2c.Client.World.Session")
local GWorldSession = require("dr2c.Shared.World.Session")

--- @class dr2c.MenuEntry

--- @class dr2c.CUIMenu
local CUIMenu = {}

--- @alias dr2c.MenuType dr2c.CUIMenu.Type

CUIMenu.Type = Enum.sequence({
	TitleScreen = 1,
	Generic = 2,
})

--- @type dr2c.UIMenu[]
local menuStack = {}

--- @type dr2c.UIWidget?
local selectedWidget

menuStack = persist("menuStack", function()
	return menuStack
end)

function CUIMenu.isOpened()
	return not not menuStack[1]
end

function CUIMenu.getAll()
	return menuStack
end

--- @deprecated
--- @return dr2c.UIWidget?
function CUIMenu.getSelectedWidget()
	return selectedWidget
end

--- @type { [1]: dr2c.UIWidget }
local previousSelectedWidget = setmetatable({}, { __mode = "v" })

function CUIMenu.draw()
	local menu = menuStack[#menuStack]
	if not menu then
		return
	end

	local canvas = menu.canvas
	canvas:draw()

	local widget = canvas.selectedWidget
	if not widget then
		return
	end

	local selectionBox = menu.selectionBox
	local rectangle = widget.rectangle
	selectionBox.x = rectangle[1]
	selectionBox.y = rectangle[2]
	selectionBox.boxWidth = rectangle[3]
	selectionBox.boxHeight = rectangle[4]

	if previousSelectedWidget[1] ~= widget then
		local prevWidget = previousSelectedWidget[1]
		if prevWidget then
			selectionBox.previousState = {
				x = prevWidget.rectangle[1],
				y = prevWidget.rectangle[2],
				time = Time.getSystemTime(),
			}
		end
		previousSelectedWidget[1] = widget
	end

	CUIDraw.drawSelectionBox(menu.selectionBox)
end

CUIMenu.eventCMenu = TE.events:new(N_("CMenu"), {
	"Initialize",
	"Overrides",
})

--- @param selectionBox dr2c.UI.DrawSelectionBox | table
--- @return dr2c.UI.DrawSelectionBox
local function copyMenuSelectionBox(selectionBox)
	local scale = CUI.getScale()

	--- @type dr2c.UI.DrawSelectionBox
	return {
		x = tonumber(selectionBox.x) or 0,
		y = tonumber(selectionBox.y) or 0,
		width = 32 * scale,
		height = 32 * scale,
		boxWidth = 0,
		boxHeight = 0,
		-- TODO 还缺几个字段复制
	}
end

--- @param menuType dr2c.MenuType
--- @param menuArgs table?
--- @return dr2c.UIMenu?
local function createMenu(menuType, menuArgs)
	--- @class dr2c.UIMenu
	--- @field draw fun(self: self)
	--- @field update nil
	--- @field canvas dr2c.UICanvas
	--- @field type dr2c.MenuType
	--- @field args table?
	--- @field selectionBox dr2c.UI.DrawSelectionBox
	--- @field animationTime number?
	--- @field content string
	--- @field title string?
	local menu = {
		canvas = CUICanvas.new(),
		type = menuType,
		args = type(menuArgs) == "table" and Table.copy(menuArgs) or nil,
		selectionBox = copyMenuSelectionBox(menuArgs and menuArgs.selectionBox or Table.empty),
	}

	--- @class dr2c.E.CMenu
	--- @field initialized boolean?
	local e = {
		menu = menu,
		args = menuArgs,
	}
	TE.events:invoke(CUIMenu.eventCMenu, e, menuType)

	if e.initialized then
		return menu
	end
end

--- @param menuType dr2c.MenuType
--- @param menuArgs table?
--- @return dr2c.UIMenu?
function CUIMenu.open(menuType, menuArgs)
	local menu = createMenu(menuType, menuArgs)

	menuStack[#menuStack + 1] = menu

	return menu
end

local updatePending = true

function CUIMenu.update()
	updatePending = true
end

function CUIMenu.updateImmediately()
	for i, menu in ipairs(menuStack) do
		menuStack[i] = createMenu(menu.type, menu.args)
	end

	updatePending = false
end

function CUIMenu.close()
	menuStack[#menuStack] = nil
end

function CUIMenu.closeAll()
	menuStack = {}
end

TE.events:add(N_("CRenderUI"), CUIMenu.draw, "RenderMenu", "Menu")

TE.events:add(N_("CUpdate"), function(e)
	if
		not CUIMenu.isOpened() --
		and CWorldSession.getState() == GWorldSession.State.Inactive
	then
		CUIMenu.open(CUIMenu.Type.TitleScreen)
	end

	if updatePending then
		CUIMenu.updateImmediately()
	end
end, "UpdateMenu")

--- @param e Events.E.ScriptUnload
TE.events:add("ScriptUnload", function(e)
	if e.data.module.UIWidgetType then
		CUIMenu.update()
	end
end, "ClientUIMenuUpdateIfRemoveTypedWidget", "Client")

-- TE.events:add()
-- CUIMenu.update()

return CUIMenu
