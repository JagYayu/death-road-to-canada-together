--[[
-- @module dr2c.client.ui.MenuGeneric
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CModule = require("dr2c.client.Module")
local CUIMenu = require("dr2c.client.ui.Menu")
local CUIWidget = require("dr2c.client.ui.Widget")

--- @param e dr2c.E.CMenu
events:add(CUIMenu.eventCMenu, function(e)
	local windowW, windowH = CModule.getWindow():getSize()

	local flex = CUIWidget.newFlex({
		rectangle = {
			windowW * 0.4,
			windowH * 0.65,
			windowW * 0.2,
			windowH * 0.25,
		},
	})

	flex:addChild(CUIWidget.newButton({
		label = "START",
	}))
	e.menu.widgets[#e.menu.widgets + 1] = flex

	e.initialized = true
end, "InitializeGeneric", "Initialize", CUIMenu.Type.Generic)
