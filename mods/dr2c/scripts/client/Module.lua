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

--- @type Window
local window = engine.primaryWindow
--- @type Renderer
local renderer = window.renderer

function CModule.getRenderer()
	return renderer
end

function CModule.getWindow()
	return window
end

local eventClientUpdate = events:new(N_("CUpdate"), {
	"Inputs",
	"Network",
	"Control", -- depreciated
	"Rollback",
	"World",
	"ECS",
	"ClearCaches",
})

events:add(N_("GUpdate"), function(e)
	--- @class dr2c.E.ClientUpdate : Events.E.TickUpdate
	--- @field networkThrottle boolean?
	local e_ = e

	events:invoke(eventClientUpdate, e_)
end, N_("ClientUpdate"), nil, "Main")

CModule.eventCRender = events:new(N_("CRender"), {
	"Camera",
	"UI",
	"Debug",
})

--- @param e Events.E.TickRender
events:add(N_("GRender"), function(e)
	--- @class dr2c.E.CRender : Events.E.TickRender
	--- @field renderer Renderer

	--- @cast e dr2c.E.CRender

	window = e.window
	renderer = window.renderer
	e.renderer = renderer

	events:invoke(CModule.eventCRender, e)
end, N_("ClientRender"), nil, "Main")

CModule.eventCLoad = events:new(N_("CLoad"), {
	"Sprites",
	"ECS",
	"Validate",
	"FastForward",
})

events:add(N_("GLoad"), function(e)
	--- @class dr2c.E.CLoad
	--- @cast e dr2c.E.CLoad
	events:invoke(CModule.eventCLoad, e)
end, N_("ClientContentLoad"), "Client")

local commonOrders = {
	"Hold",
	"Pause",
}

CModule.eventCKeyCodeDown = events:new(N_("CKeyCodeDown"), commonOrders)
CModule.eventCScanCodeDown = events:new(N_("CScanCodeDown"), commonOrders)
CModule.eventCKeyCodeUp = events:new(N_("CKeyCodeUp"), commonOrders)
CModule.eventCScanCodeUp = events:new(N_("CScanCodeUp"), commonOrders)

--- @param e Events.E.KeyDown
events:add(N_("GKeyDown"), function(e)
	events:invoke(CModule.eventCKeyCodeDown, e, e.keyCode)
	events:invoke(CModule.eventCScanCodeDown, e, e.scanCode)
end, N_("ClientKeyDown"))

events:add(N_("GKeyUp"), function(e)
	events:invoke(CModule.eventCKeyCodeUp, e, e.keyCode)
	events:invoke(CModule.eventCScanCodeUp, e, e.scanCode)
end, N_("ClientKeyUp"))

return CModule
