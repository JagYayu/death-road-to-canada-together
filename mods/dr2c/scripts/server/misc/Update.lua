--- @class dr2c.CUpdate
local CUpdate = {}

local eventServerUpdate = events:new(N_("SUpdate"), {
	"Network",
})

events:add(N_("GUpdate"), function(e)
	events:invoke(eventServerUpdate, e)
end, N_("ServerUpdate"), nil, "Main")
