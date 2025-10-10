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

local CUI = require("dr2c.Client.UI.UI")
local CModule = require("dr2c.Client.Module")
local CUIMenu = require("dr2c.Client.UI.Menu.Menu")
local CUIWidget = require("dr2c.Client.UI.Widget.Widget")

--- @param e dr2c.E.CMenu
TE.events:add(CUIMenu.eventCMenu, function(e)
	--- @return Rectangle
	local function newFlexRectangle()
		local windowW, windowH = CModule.getWindow():getSize()

		return {
			windowW * 0.4,
			windowH * 0.6,
			windowW * 0.2,
			windowH * 0.3,
		}
	end

	local root = CUIWidget.newFlex({
		rectangle = newFlexRectangle(),
		vertical = true,
	})

	local uiScale = CUI.getScale()

	local commonMargin = {
		0,
		0,
		0,
		0,
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
		callback = function()
			local ExitJumpscare = require("dr2c.client.ui.ExitJumpscare")

			ExitJumpscare.trigger()
		end,
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
