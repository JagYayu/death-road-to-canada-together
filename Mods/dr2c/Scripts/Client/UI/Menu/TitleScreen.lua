--[[
-- @module dr2c.Client.UI.MenuGeneric
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CForthGameMode = require("dr2c.Shared.Forth.GameMode")
local CForthSession = require("dr2c.Client.Forth.Session")
local CModule = require("dr2c.Client.Module")
local CTileMap = require("dr2c.Client.Tile.Map")
local CTileSchema = require("dr2c.Client.Tile.Schema")
local CUI = require("dr2c.Client.UI.UI")
local CUIMenu = require("dr2c.Client.UI.Menu")
local CUIWidget = require("dr2c.Client.UI.Widget")
local CWorldSession = require("dr2c.Client.World.Session")
local GWorldAttribute = require("dr2c.Shared.World.Session")
local GWorldLevel = require("dr2c.Shared.World.Level")

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
		label = "CONNECT",
		margin = commonMargin,
		scale = uiScale,
		border = commonBorder,
		callback = function()
			TE.network:clientConnectRUDP({
				host = "localhost",
				port = 8878,
			})
		end,
	}))

	root:addChild(CUIWidget.newButton({
		label = "HOST",
		margin = commonMargin,
		scale = uiScale,
		border = commonBorder,
		callback = function()
			TE.network:serverHostRUDP({
				host = "localhost",
				port = 8878,
			})
		end,
	}))

	root:addChild(CUIWidget.newButton({
		label = "START FORTH SESSION",
		margin = commonMargin,
		scale = uiScale,
		border = commonBorder,
		callback = function()
			CForthSession.startNew({
				gameMode = CForthGameMode.Type.Normal,
			})

			CUIMenu.closeAll()
		end,
	}))

	root:addChild(CUIWidget.newButton({
		label = "START WORLD SESSION",
		margin = commonMargin,
		scale = uiScale,
		border = commonBorder,
		callback = function()
			local tileTypes = {}
			for i = 1, 8 * 8 do
				tileTypes[i] = "Floor15"
			end

			tileTypes[12] = "Wall1"
			tileTypes[13] = "Wall1"
			tileTypes[14] = "Wall1"

			CWorldSession.start({
				[GWorldAttribute.Attribute.Mode] = GWorldAttribute.Mode.DeathRoad,
				[GWorldAttribute.Attribute.Level] = GWorldLevel.Type.Cabin,
				[GWorldAttribute.Attribute.Scenes] = {
					"main",
				},
				[GWorldAttribute.Attribute.TileMaps] = {
					main = CTileMap.newTileMap(-4, -4, 8, 8, tileTypes),
				},
			})
		end,
	}))

	root:addChild(CUIWidget.newButton({
		label = "EXIT",
		margin = commonMargin,
		scale = uiScale,
		border = commonBorder,
		callback = function()
			local ExitJumpscare = require("dr2c.Client.UI.ExitJumpscare")

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

CUIMenu.update()
