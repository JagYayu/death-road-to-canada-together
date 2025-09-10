local CUIMenu = require("dr2c.client.ui.Menu")
local CUIWidget = require("dr2c.client.ui.Widget")

local CUIMenuGeneric = {}

--- @param e dr2c.E.CMenu
events:add(CUIMenu.eventCMenu, function(e)
	--
	e.menu.widgets[#e.menu.widgets + 1] = CUIWidget.newButton({
		x = 100,
		y = 100,
		w = 800,
		h = 20,
	})

	

	e.initialized = true
end, "InitializeGeneric", "Initialize", CUIMenu.Type.Generic)
