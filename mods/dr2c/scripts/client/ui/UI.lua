--[[
-- @module dr2c.client.ui.UI
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Color = require("tudov.Color")

local CClient = require("dr2c.client.Module")

local CUI = {}

local scale = 1

function CUI.getScale()
	return scale
end

CUI.eventRenderUI = TE.events:new(N_("CRenderUI"), {
	"Begin",
	"Menu",
	"Test",
	"Exit",
	"End",
})

--- @param e dr2c.E.CRender
TE.events:add(N_("CRender"), function(e)
	local w, h = e.window:getSize()
	scale = math.min(w, h) / 480

	--- @alias dr2c.E.CRenderUI dr2c.E.CRender
	TE.events:invoke(CUI.eventRenderUI, e)
end, N_("RenderUI"), "UI")

return CUI
