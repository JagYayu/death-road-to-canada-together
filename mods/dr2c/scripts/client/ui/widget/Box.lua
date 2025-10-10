--[[
-- @module dr2c.client.ui.widget.Box
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CUIDraw = require("dr2c.Client.UI.Draw")
local CUIWidget = require("dr2c.Client.UI.Widget.Widget")

--- @class dr2c.CUIWidgetBox
local CUIWidgetBox = {}

CUIWidgetBox.UIWidgetType = CUIWidget.Type.Box

--- @param self dr2c.UIWidget
local function draw(self)
	local rect = self.rectangle
	local border = self.border

	border.x = rect[1]
	border.y = rect[2]
	border.width = rect[3]
	border.height = rect[4]

	CUIDraw.drawBorder(border)
end

CUIWidgetBox.metatable = {
	__index = {
		draw = draw,
	},
}

--- @param e dr2c.E.CWidget
TE.events:add(CUIWidget.eventCWidget, function(e)
	e.widget = setmetatable(e.widget, CUIWidgetBox.metatable)

	e.initialized = true
end, "Box", "Initialize", CUIWidget.Type.Box)

return CUIWidgetBox
