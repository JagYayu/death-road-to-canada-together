--- @class dr2c.CUpdate
local CUpdate = {}

local eventClientUpdate = events:new(N_("CUpdate"), {
	"Network",
	"WorldTick",
	"ClearCaches",
})

events:add("TickUpdate", function(e)
	events:invoke(eventClientUpdate, e)
end, N_("ClientUpdate"), nil, "Main")
