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
TE.events:add(CUIMenu.eventCMenu, function(e)
	--- @return Rectangle
	local function newFlexRectangle()
		local windowW, windowH = CModule.getWindow():getSize()

		return {
			windowW * 0.4,
			windowH * 0.65,
			windowW * 0.2,
			windowH * 0.25,
		}
	end

	local root = CUIWidget.newFlex({
		rectangle = newFlexRectangle(),
		vertical = true,
	})

	local uiScale = CUI.getScale()

	local commonMargin = {
		uiScale,
		uiScale,
		uiScale,
		uiScale,
	}

	local commonBorder = {
		size = 8 * uiScale,
	}

	root:addChild(CUIWidget.newButton({
		label = "START",
		margin = commonMargin,
		scale = uiScale,
		border = commonBorder,
	}))

	root:addChild(CUIWidget.newButton({
		label = "OPTIONS",
		margin = commonMargin,
		scale = uiScale,
		border = commonBorder,
	}))

	root:addChild(CUIWidget.newButton({
		label = "EXIT",
		margin = commonMargin,
		scale = uiScale,
		border = commonBorder,
	}))

	e.menu.canvas:addWidget(root)

	local update = e.menu.update
	--- @param self dr2c.UIMenu
	function e.menu.update(self)
		root.rectangle = newFlexRectangle()

		-- update(self)
	end

	e.initialized = true
end, "InitializeTitleScreen", "Initialize", CUIMenu.Type.TitleScreen)
