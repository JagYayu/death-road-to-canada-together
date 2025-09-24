--[[
-- @module dr2c.client.Module
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class dr2c.CModule
local CModule = {}

--- @type TE.Window
local window = TE.engine.primaryWindow
--- @type TE.Renderer
local renderer = window.renderer

function CModule.getRenderer()
	return renderer
end

function CModule.getWindow()
	return window
end

local eventClientUpdate = TE.events:new(N_("CUpdate"), {
	"Inputs",
	"Network",
	"Control", -- depreciated
	"Rollback",
	"World",
	"ECS",
	"ClearCaches",
})

TE.events:add("TickUpdate", function(e)
	--- @class dr2c.E.ClientUpdate : Events.E.TickUpdate
	--- @field networkThrottle boolean?
	local e_ = e

	TE.events:invoke(eventClientUpdate, e_)
end, N_("ClientUpdate"), "Client", "Primary")

CModule.eventCRender = TE.events:new(N_("CRender"), {
	"Camera",
	"UI",
	"Debug",
})

--- @param e Events.E.TickRender
TE.events:add("TickRender", function(e)
	--- @class dr2c.E.CRender : Events.E.TickRender
	--- @field renderer TE.Renderer

	--- @cast e dr2c.E.CRender

	window = e.window
	renderer = window.renderer
	e.renderer = renderer

	TE.events:invoke(CModule.eventCRender, e)
end, N_("ClientRender"), "Client", "Primary")

CModule.eventCLoad = TE.events:new(N_("CLoad"), {
	"Sprites",
	"ECS",
	"Validate",
	"FastForward",
})

TE.events:add("TickLoad", function(e)
	--- @class dr2c.E.CLoad
	--- @cast e dr2c.E.CLoad
	TE.events:invoke(CModule.eventCLoad, e)
end, N_("ClientContentLoad"), "Client")

-- local keyboardEventCommonOrders = {
-- 	"Hold",
-- 	"Pause",
-- }

-- CModule.eventCKeyCodeDown = TE.events:new(N_("CKeyCodeDown"), keyboardEventCommonOrders)
-- CModule.eventCScanCodeDown = TE.events:new(N_("CScanCodeDown"), keyboardEventCommonOrders)
-- CModule.eventCKeyCodeUp = TE.events:new(N_("CKeyCodeUp"), keyboardEventCommonOrders)
-- CModule.eventCScanCodeUp = TE.events:new(N_("CScanCodeUp"), keyboardEventCommonOrders)

-- --- @param e Events.E.KeyboardPress
-- TE.events:add(N_("GKeyboardPress"), function(e)
-- 	TE.events:invoke(CModule.eventCKeyCodeDown, e, e.keyCode)
-- 	TE.events:invoke(CModule.eventCScanCodeDown, e, e.scanCode)
-- end, N_("ClientKeyboardPress"))

-- TE.events:add(N_("GKeyUp"), function(e)
-- 	TE.events:invoke(CModule.eventCKeyCodeUp, e, e.keyCode)
-- 	TE.events:invoke(CModule.eventCScanCodeUp, e, e.scanCode)
-- end, N_("ClientKeyUp"))

-- TE.events:add("MouseMove", function(e)

-- end, N_"ClientMouseMove", "", key?, sequence?)

return CModule
