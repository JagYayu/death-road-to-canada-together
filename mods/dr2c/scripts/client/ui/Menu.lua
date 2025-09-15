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

--- @class dr2c.MenuEntry

--- @class dr2c.CUIMenu
local CUIMenu = {}

--- @alias dr2c.MenuType dr2c.CUIMenu.Type

CUIMenu.Type = Enum.sequence({
	TitleScreen = 1,
	Generic = 2,
})

--- @type dr2c.Menu[]
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
	if menu then
		menu:draw()
	end
end

function CUIMenu.update()
	for _, menu in Table.reversedPairs(menuStack) do
		if menu.update then
			menu:update()
		end
	end
end

CUIMenu.eventCMenu = events:new(N_("CMenu"), {
	"Initialize",
	"Overrides",
})

--- @param self dr2c.Menu
local function metatableMenuDraw(self)
	selectedWidget = nil
	local mouseX
	local mouseY
	for _, widget in ipairs(self.widgets) do
		-- widget.rectangle
		if widget.selectable then
			--
		end
	end

	for _, widget in ipairs(self.widgets) do
		local draw = widget.draw
		if draw then
			draw(widget)
		end
	end
end

--- @param self dr2c.Menu
local function metatableMenuUpdate(self)
	for _, widget in ipairs(self.widgets) do
		local update = widget.update
		if update then
			update(widget)
		end
	end
end

CUIMenu.metatable = {
	__index = {
		draw = metatableMenuDraw,
		update = metatableMenuUpdate,
	},
}

--- @param menuType dr2c.MenuType
--- @param menuArgs table?
--- @return dr2c.Menu?
function CUIMenu.open(menuType, menuArgs)
	--- @class dr2c.Menu : table
	--- @field draw? fun(self: self)
	--- @field update fun(self: self)
	--- @field animationTime number?
	--- @field content string
	--- @field title string?
	--- @field type dr2c.MenuType
	--- @field widgets dr2c.UIWidget[]
	local menu = {
		type = menuType,
		widgets = {},
	}

	setmetatable(menu, CUIMenu.metatable)

	--- @class dr2c.E.CMenu
	--- @field initialized boolean?
	local e = {
		menu = menu,
		args = menuArgs,
	}
	events:invoke(CUIMenu.eventCMenu, e, menuType)

	if e.initialized then
		menu:update()

		menuStack[#menuStack + 1] = menu

		return menu
	end
end

function CUIMenu.close()
	menuStack[#menuStack] = nil
end

function CUIMenu.closeAll()
	menuStack = {}
end

events:add(N_("CRenderUI"), CUIMenu.draw, "RenderMenu", "Menu")

local testOnce = true

events:add(N_("CUpdate"), function(e)
	if testOnce then
		CUIMenu.closeAll()
		CUIMenu.open(CUIMenu.Type.TitleScreen)

		testOnce = false
		print("test once")
	end

	CUIMenu.update()
end, "Test")

return CUIMenu
