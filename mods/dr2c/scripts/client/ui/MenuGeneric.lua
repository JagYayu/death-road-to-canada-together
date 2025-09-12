local CUIMenu = require("dr2c.client.ui.Menu")
local CUIWidget = require("dr2c.client.ui.Widget")

--- @param e dr2c.E.CMenu
events:add(CUIMenu.eventCMenu, function(e)
	e.menu.widgets[#e.menu.widgets + 1] = CUIWidget.newButton({
		x = 400,
		y = 400,
		w = 200,
		h = 40,
		label = "START",
		alignX = 0.5,
		alignY = 0.5,
	})

	e.initialized = true
end, "InitializeGeneric", "Initialize", CUIMenu.Type.Generic)
