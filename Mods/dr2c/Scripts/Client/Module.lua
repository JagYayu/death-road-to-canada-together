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
	"Throttle",
	"Test",
	"Inputs",
	"Network",
	"World",
	"Snapshot",
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
	"Tile",
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

--- @param e TE.E.TickUpdate
TE.events:add("TickUpdate", function(e)
	--- @class dr2c.E.CUpdate : TE.E.TickUpdate
	--- @field networkThrottle? boolean
	local e_ = e

	TE.events:invoke(eventClientUpdate, e_)
end, N_("ClientUpdate"), "Client", "Primary")

--- @param e TE.E.TickRender
TE.events:add("TickRender", function(e)
	--- @class dr2c.E.CRender : TE.E.TickRender
	--- @field renderer TE.Renderer

	--- @cast e dr2c.E.CRender

	window = e.window
	renderer = window.renderer
	e.renderer = renderer

	TE.events:invoke(CModule.eventCRender, e)

	collectgarbage()
end, N_("ClientRender"), "Client", "Primary")

--- @param e TE.E.TickLoad
TE.events:add("TickLoad", function(e)
	--- @class dr2c.E.CLoad : TE.E.TickLoad
	--- @cast e dr2c.E.CLoad
	TE.events:invoke(CModule.eventCLoad, e)
end, N_("ClientContentLoad"), "Client")

--- @param e TE.E.KeyboardPress
TE.events:add("KeyboardPress", function(e)
	--- @class dr2c.E.CKeyboardPress : TE.E.KeyboardPress
	--- @cast e dr2c.E.CKeyboardPress
	TE.events:invoke(CModule.eventCKeyboardPress, e, e.data.scanCode, EEventInvocation.CacheHandlers)
end, N_("ClientKeyboardPress"))

--- @param e TE.E.KeyboardRelease
TE.events:add("KeyboardRelease", function(e)
	--- @class dr2c.E.CKeyboardRelease : TE.E.KeyboardRelease
	--- @cast e dr2c.E.CKeyboardRelease
	TE.events:invoke(CModule.eventCKeyboardRelease, e, e.data.scanCode, EEventInvocation.CacheHandlers)
end, N_("ClientKeyboardRelease"))

--- @param e TE.E.MouseMotion
TE.events:add("MouseMotion", function(e)
	--- @class dr2c.E.MouseMotion : TE.E.MouseMotion
	--- @cast e dr2c.E.MouseMotion
	TE.events:invoke(CModule.eventCMouseMotion, e)
end, N_("ClientKeyboardPress"))

--- @param e TE.E.MousePress
TE.events:add("MousePress", function(e)
	--- @class dr2c.E.MousePress : TE.E.MousePress
	--- @cast e dr2c.E.MousePress
	TE.events:invoke(CModule.eventCMousePress, e)
end, N_("ClientKeyboardPress"))

--- @param e TE.E.MouseRelease
TE.events:add("MouseRelease", function(e)
	--- @class dr2c.E.MouseRelease : TE.E.MouseRelease
	--- @cast e dr2c.E.MouseRelease
	TE.events:invoke(CModule.eventCMouseRelease, e)
end, N_("ClientKeyboardPress"))

return CModule
