local Enum = require("tudov.Enum")
--- @class dr2c.MenuEntry

--- @class dr2c.CUIMenu
local CUIMenu = {}

--- @alias dr2c.MenuType dr2c.CUIMenu.Type

CUIMenu.Type = Enum.sequence({
	Generic = 1,
})

local menuStack = {}

menuStack = persist("menuStack", function()
	return menuStack
end)

function CUIMenu.getAll()
	return menuStack
end

CUIMenu.eventCMenu = events:new(N_("CMenu"), {
	"Initialize",
	"Overrides",
})

local menuMetatable = {
	__index = {
		--- @param self dr2c.Menu
		draw = function(self, ...)
			for _, widget in ipairs(self.widgets) do
				local draw = widget.draw
				if draw then
					draw(widget, ...)
				end
			end
		end,
	},
}

--- @param menuType dr2c.MenuType
--- @param menuArgs table?
--- @return dr2c.Menu?
function CUIMenu.open(menuType, menuArgs)
	--- @class dr2c.Menu
	--- @field animationTime number?
	--- @field content string
	--- @field title string?
	--- @field type dr2c.MenuType
	--- @field widgets dr2c.UIWidget[]
	local menu = {
		type = menuType,
		widgets = {},
	}

	--- @class dr2c.E.CMenu
	--- @field initialized boolean?
	local e = {
		menu = menu,
		args = menuArgs,
	}
	events:invoke(CUIMenu.eventCMenu, e, menuType)

	if e.initialized then
		menuStack[#menuStack + 1] = setmetatable(menu, menuMetatable)

		return menu
	end
end

function CUIMenu.close()
	menuStack[#menuStack] = nil
end

function CUIMenu.closeAll()
	menuStack = {}
end

events:add(N_("CRenderUI"), function(e)
	local menu = menuStack[#menuStack]
	if menu then
		menu:draw(e.renderer)
	end
end, "RenderMenu", "Menu")

local testOnce = true

events:add(N_("CUpdate"), function(e)
	if testOnce then
		CUIMenu.closeAll()
		CUIMenu.open(CUIMenu.Type.Generic)

		testOnce = false
		print("test once")
	end
end, "Test")

return CUIMenu
