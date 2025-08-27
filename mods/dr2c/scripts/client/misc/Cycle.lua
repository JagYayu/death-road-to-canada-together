local eventClientUpdate = events:new(N_("CUpdate"), {
	"Network",
	"Control",
	"WorldTick",
	"ECS",
	"ClearCaches",
})

events:add(N_("GUpdate"), function(e)
	--- @class dr2c.E.ClientUpdate : Events.E.TickUpdate
	--- @field networkThrottle boolean?
	local e_ = e

	events:invoke(eventClientUpdate, e_)
end, N_("ClientUpdate"), nil, "Main")

local eventClientRender = events:new(N_("CRender"), {
	"Camera",
	"UI",
})

events:add(N_("GRender"), function(e)
	events:invoke(eventClientRender, e)
end, N_("ClientRender"), nil, "Main")

local eventClientLoad = events:new(N_("CLoad"), {
	"Sprites",
	"ECS",
	"Validate",
	"FastForward",
})

events:add(N_("GLoad"), function(e)
	--- @class dr2c.E.CLoad
	--- @cast e dr2c.E.CLoad
	events:invoke(eventClientLoad, e)
end, N_("ClientContentLoad"), "Client")

local eventClientKeyDown = events:new(N_("CKeyDown"), {
	"Hold",
})

events:add(N_("GKeyDown"), function(e)
	events:invoke(eventClientKeyDown, e)
end, N_("ClientKeyDown"))

local eventClientKeyUp = events:new(N_("CKeyUp"), {
	"Hold",
})

events:add(N_("GKeyUp"), function(e)
	events:invoke(eventClientKeyUp, e)
end, N_("ClientKeyUp"))
