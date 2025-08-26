--- @class dr2c.CUpdate
local CUpdate = {}

local eventClientUpdate = events:new(N_("CUpdate"), {
	"Network",
	"WorldTick",
	"ClearCaches",
})

events:add(N_("GUpdate"), function(e)
	--- @class dr2c.E.ClientUpdate : Events.E.TickUpdate
	--- @field networkThrottle boolean?
	local e_ = e

	events:invoke(eventClientUpdate, e_)
end, N_("ClientUpdate"), nil, "Main")
