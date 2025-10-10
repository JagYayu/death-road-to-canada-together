--[[
-- @module dr2c.client.ui.widget.Button
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("TE.Table")
local Color = require("TE.Color")

local CRenderUtils = require("dr2c.Client.Render.Utils")
local CUIDraw = require("dr2c.Client.UI.Draw")
local CUIWidget = require("dr2c.Client.UI.Widget.Widget")

--- @class dr2c.CUIWidgetButton
local CUIWidgetButton = {}

CUIWidgetButton.UIWidgetType = CUIWidget.Type.Button

--- @param self dr2c.UIWidgetButton
local function draw(self)
	local args = self.drawButtonArgs
	local margin = self.margin
	local rect = self.rectangle

	args.x = rect[1] + margin[1]
	args.y = rect[2] + margin[2]
	args.width = rect[3] - (margin[1] + margin[3])
	args.height = rect[4] - (margin[2] + margin[4])
	args.border = self.border

	CUIDraw.drawButton(args)
end

--- @param self dr2c.UIWidgetButton
local function update(self)
	-- args
end

CUIWidgetButton.metatable = {
	__index = {
		draw = draw,
		update = update,
	},
}

--- @param e dr2c.E.CWidget
TE.events:add(CUIWidget.eventCWidget, function(e)
	--- @class dr2c.UIWidgetButton.Args : dr2c.UIWidget.Args
	--- @field alignX? number
	--- @field alignY? number
	--- @field label? string
	--- @field scale? number
	local args = e.args

	--- @class dr2c.UIWidgetButton : dr2c.UIWidget
	local widget = e.widget

	local label = args.label and tostring(args.label) or nil
	local scale = tonumber(args.scale) or 1

	--- @type dr2c.UI.DrawButton
	widget.drawButtonArgs = {
		x = widget.rectangle[1],
		y = widget.rectangle[2],
		width = widget.rectangle[3],
		height = widget.rectangle[4],
		text = label,
		scale = scale,
		alignX = tonumber(args.alignX) or 0.5,
		alignY = tonumber(args.alignY) or 0.5,
		border = CRenderUtils.copyDrawBorder(args.border or Table.empty),
	}

	e.widget = setmetatable(widget, CUIWidgetButton.metatable)

	e.initialized = true
end, "Button", "Initialize", CUIWidgetButton.UIWidgetType)

return CUIWidgetButton
