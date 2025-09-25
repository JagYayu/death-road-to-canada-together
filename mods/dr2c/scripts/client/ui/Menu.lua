--[[
-- @module dr2c.client.ui.Menu
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("tudov.Enum")
local Table = require("tudov.Table")

local CUIDraw = require("dr2c.client.ui.Draw")
local CUICanvas = require("dr2c.client.ui.Canvas")

--- @class dr2c.MenuEntry

--- @class dr2c.CUIMenu
local CUIMenu = {}

local defaultSelectionBoxImageID = TE.images:getID("gfx/misc/dr2c_particles")

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

function CUIMenu.getAll()
	return menuStack
end

--- @return dr2c.UIWidget?
function CUIMenu.getSelectedWidget()
	return selectedWidget
end

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

	-- CUIDraw.setTexture(drawTexture)

	-- widget.rectangle[1]
	-- widget.rectangle[2]
end

CUIMenu.eventCMenu = TE.events:new(N_("CMenu"), {
	"Initialize",
	"Overrides",
})

--- @param self dr2c.UIMenu
local function metatableMenuDraw(self)
	local canvas = self.canvas

	canvas:draw()

	-- self.selectionBox

	local widget = self.canvas.selectedWidget
	if widget then
		-- widget.rectangle[1]
		-- widget.rectangle[2]
	end
end

--- @param self dr2c.UIMenu
local function metatableMenuUpdate(self)
	-- self.canvas:update()
end

-- CUIMenu.metatable = {
-- 	__index = {
-- 		draw = metatableMenuDraw,
-- 		update = metatableMenuUpdate,
-- 	},
-- }

--- @param selectionBox dr2c.UIMenu.SelectionBox | table
--- @return dr2c.UIMenu.SelectionBox
local function copyMenuSelectionBox(selectionBox)
	--- @class dr2c.UIMenu.SelectionBox

	return {
		texture = selectionBox.texture or defaultSelectionBoxImageID,
		x = tonumber(selectionBox.x) or 0,
		y = tonumber(selectionBox.y) or 0,
		w = tonumber(selectionBox.w) or 0,
		h = tonumber(selectionBox.h) or 0,
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
	--- @field selectionBox dr2c.UIMenu.SelectionBox
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

function CUIMenu.update()
	for i, menu in ipairs(menuStack) do
		menuStack[i] = createMenu(menu.type, menu.args)
	end
end

function CUIMenu.close()
	menuStack[#menuStack] = nil
end

function CUIMenu.closeAll()
	menuStack = {}
end

TE.events:add(N_("CRenderUI"), CUIMenu.draw, "RenderMenu", "Menu")

local testOnce = true

TE.events:add(N_("CUpdate"), function(e)
	if testOnce then
		CUIMenu.closeAll()
		CUIMenu.open(CUIMenu.Type.TitleScreen)

		testOnce = false
		print("test once")
	end

	-- CUIMenu.update()
end, "Test")

return CUIMenu
