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

local CUI = require("dr2c.client.ui.UI")
local CModule = require("dr2c.client.Module")
local CUIMenu = require("dr2c.client.ui.Menu")
local CUIWidget = require("dr2c.client.ui.Widget")

--- @param e dr2c.E.CMenu
events:add(CUIMenu.eventCMenu, function(e)
	local function newFlexRectangle()
		local windowW, windowH = CModule.getWindow():getSize()

		return {
			windowW * 0.4,
			windowH * 0.65,
			windowW * 0.2,
			windowH * 0.25,
		}
	end

	local widget = CUIWidget.newFlex({
		rectangle = newFlexRectangle(),
		vertical = true,
	})

	local margin = {
		4,
		4,
		4,
		4,
	}

	widget:addChild(CUIWidget.newButton({
		label = "START",
		margin = margin,
		scale = CUI.getScale(),
		border = {
			size = 4 * CUI.getScale(),
		},
	}))

	widget:addChild(CUIWidget.newButton({
		label = "OPTIONS",
		margin = margin,
		scale = CUI.getScale(),
		border = {
			size = 4 * CUI.getScale(),
		},
	}))

	widget:addChild(CUIWidget.newButton({
		label = "EXIT",
		margin = margin,
		scale = CUI.getScale(),
		border = {
			size = 40 * CUI.getScale(),
		},
	}))

	e.menu.canvas:addWidget(widget)

	local update = e.menu.update
	--- @param self dr2c.UIMenu
	function e.menu.update(self)
		widget.rectangle = newFlexRectangle()

		update(self)
	end

	e.initialized = true
end, "InitializeTitleScreen", "Initialize", CUIMenu.Type.TitleScreen)
