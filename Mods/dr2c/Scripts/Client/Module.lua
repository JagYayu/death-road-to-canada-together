--[[
-- @module dr2c.Client.Module
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

CModule.eventCRender = TE.events:new(N_("CRender"), {
	"Camera",
	"UI",
	"Debug",
})

CModule.eventCLoad = TE.events:new(N_("CLoad"), {
	"Sprites",
	"ECS",
	"Validate",
	"FastForward",
})

local keyboardEventCommonOrders = {
	"UI",
}

CModule.eventCKeyboardPress = TE.events:new(N_("CKeyboardPress"), keyboardEventCommonOrders)
CModule.eventCKeyboardRelease = TE.events:new(N_("CKeyboardRelease"), keyboardEventCommonOrders)
CModule.eventCKeyboardRepeat = TE.events:new(N_("CKeyboardRepeat"), keyboardEventCommonOrders)

local mouseEventCommonOrders = {
	"UI",
}

CModule.eventCMouseMotion = TE.events:new(N_("CMouseMotion"), mouseEventCommonOrders)
CModule.eventCMousePress = TE.events:new(N_("CMousePress"), mouseEventCommonOrders)
CModule.eventCMouseRelease = TE.events:new(N_("CMouseRelease"), mouseEventCommonOrders)

--- @param e Events.E.TickUpdate
TE.events:add("TickUpdate", function(e)
	--- @class dr2c.E.ClientUpdate : Events.E.TickUpdate
	--- @field networkThrottle boolean?
	local e_ = e

	TE.events:invoke(eventClientUpdate, e_)
end, N_("ClientUpdate"), "Client", "Primary")

--- @param e Events.E.TickRender
TE.events:add("TickRender", function(e)
	--- @class dr2c.E.CRender : Events.E.TickRender
	--- @field renderer TE.Renderer

	--- @cast e dr2c.E.CRender

	window = e.window
	renderer = window.renderer
	e.renderer = renderer

	TE.events:invoke(CModule.eventCRender, e)

	collectgarbage()
end, N_("ClientRender"), "Client", "Primary")

--- @param e Events.E.TickLoad
TE.events:add("TickLoad", function(e)
	--- @class dr2c.E.CLoad : Events.E.TickLoad
	--- @cast e dr2c.E.CLoad
	TE.events:invoke(CModule.eventCLoad, e)
end, N_("ClientContentLoad"), "Client")

--- @param e Events.E.KeyboardPress
TE.events:add("KeyboardPress", function(e)
	--- @class dr2c.E.CKeyboardPress : Events.E.KeyboardPress
	--- @cast e dr2c.E.CKeyboardPress
	TE.events:invoke(CModule.eventCKeyboardPress, e, e.data.scanCode, EEventInvocation.CacheHandlers)
end, N_("ClientKeyboardPress"))

--- @param e Events.E.KeyboardRelease
TE.events:add("KeyboardRelease", function(e)
	--- @class dr2c.E.CKeyboardRelease : Events.E.KeyboardRelease
	--- @cast e dr2c.E.CKeyboardRelease
	TE.events:invoke(CModule.eventCKeyboardRelease, e, e.data.scanCode, EEventInvocation.CacheHandlers)
end, N_("ClientKeyboardRelease"))

--- @param e Events.E.MouseMotion
TE.events:add("MouseMotion", function(e)
	--- @class dr2c.E.MouseMotion : Events.E.MouseMotion
	--- @cast e dr2c.E.MouseMotion
	TE.events:invoke(CModule.eventCMouseMotion, e)
end, N_("ClientKeyboardPress"))

--- @param e Events.E.MousePress
TE.events:add("MousePress", function(e)
	--- @class dr2c.E.MousePress : Events.E.MousePress
	--- @cast e dr2c.E.MousePress
	TE.events:invoke(CModule.eventCMousePress, e)
end, N_("ClientKeyboardPress"))

--- @param e Events.E.MouseRelease
TE.events:add("MouseRelease", function(e)
	--- @class dr2c.E.MouseRelease : Events.E.MouseRelease
	--- @cast e dr2c.E.MouseRelease
	TE.events:invoke(CModule.eventCMouseRelease, e)
end, N_("ClientKeyboardPress"))

return CModule
