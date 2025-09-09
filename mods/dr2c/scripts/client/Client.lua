local CClient = {}

--- @type Renderer?
local renderer
--- @type Window?
local window

function CClient.getRenderer()
	return renderer
end

function CClient.getWindow()
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

CClient.eventCRender = events:new(N_("CRender"), {
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

	events:invoke(CClient.eventCRender, e)
end, N_("ClientRender"), nil, "Main")

CClient.eventCLoad = events:new(N_("CLoad"), {
	"Sprites",
	"ECS",
	"Validate",
	"FastForward",
})

events:add(N_("GLoad"), function(e)
	--- @class dr2c.E.CLoad
	--- @cast e dr2c.E.CLoad
	events:invoke(CClient.eventCLoad, e)
end, N_("ClientContentLoad"), "Client")

local commonOrders = {
	"Hold",
	"Pause",
}

CClient.eventCKeyCodeDown = events:new(N_("CKeyCodeDown"), commonOrders)
CClient.eventCScanCodeDown = events:new(N_("CScanCodeDown"), commonOrders)
CClient.eventCKeyCodeUp = events:new(N_("CKeyCodeUp"), commonOrders)
CClient.eventCScanCodeUp = events:new(N_("CScanCodeUp"), commonOrders)

--- @param e Events.E.KeyDown
events:add(N_("GKeyDown"), function(e)
	events:invoke(CClient.eventCKeyCodeDown, e, e.keyCode)
	events:invoke(CClient.eventCScanCodeDown, e, e.scanCode)
end, N_("ClientKeyDown"))

events:add(N_("GKeyUp"), function(e)
	events:invoke(CClient.eventCKeyCodeUp, e, e.keyCode)
	events:invoke(CClient.eventCScanCodeUp, e, e.scanCode)
end, N_("ClientKeyUp"))

return CClient
