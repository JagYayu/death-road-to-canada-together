--- @class dr2c.GCycle
local GCycle = {}

local tick = 0
tick = persist("tick", function()
	return tick
end)

function GCycle.getTick()
	return tick
end

local CommonOrders = {
	"Client",
	"Server",
}

local eventGameUpdate = events:new(N_("GUpdate"), CommonOrders)

events:add("TickUpdate", function(e)
	tick = tick + 1
	events:invoke(eventGameUpdate, e, nil, EEventInvocation.None)
end, N_("GameUpdate"))

local eventGameRender = events:new(N_("GRender"), CommonOrders)

events:add("TickRender", function(e)
	events:invoke(eventGameRender, e)
end, N_("GameRender"))

local eventGameLoad = events:new(N_("GLoad"), CommonOrders)

events:add("TickLoad", function(e)
	events:invoke(eventGameLoad, e)
end, N_("GameLoad"))

engine:triggerLoadPending()

return GCycle
