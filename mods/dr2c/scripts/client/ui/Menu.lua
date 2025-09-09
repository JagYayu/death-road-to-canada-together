--- @class dr2c.MenuEntry

--- @class dr2c.Menu
--- @field animationTime number?
--- @field content string
--- @field widgets dr2c.MenuEntry[]
--- @field name string
--- @field title string?

--- @class dr2c.CUIMenu
local CUIMenu = {}

local menuStack = {}

menuStack = persist("menuStack", function()
	return menuStack
end)

function CUIMenu.getAll()
	return menuStack
end

CUIMenu.eventCMenu = events:new(N_("CMenu"), {})

--- @param menuName string
--- @param menuArgs table?
--- @return table
function CUIMenu.open(menuName, menuArgs)
	menuName = tostring(menuName)

	local menu = {
		name = menuName,
	}

	local e = {
		menu = menu,
		args = menuArgs,
	}
	events:invoke(CUIMenu.eventCMenu, e, menuName)

	menuStack[#menuStack + 1] = menu

	return menu
end

function CUIMenu.close()
	menuStack[#menuStack] = nil
end

function CUIMenu.closeAll()
	menuStack = {}
end

--- @param menu dr2c.Menu
function CUIMenu.draw(menu)
	--
end

events:add(N_("CRenderUI"), function(e)
	local menu = menuStack[#menuStack]
	if menu then
		CUIMenu.draw(menu)
	end
end, "RenderMenu", "Menu")

return CUIMenu
