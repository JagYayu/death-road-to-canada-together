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
local eventGameRender = events:new(N_("GRender"), CommonOrders)
local eventGameLoad = events:new(N_("GLoad"), CommonOrders)
local eventGameKeyDown = events:new(N_("GKeyDown"), CommonOrders)
local eventGameKeyUp = events:new(N_("GKeyUp"), CommonOrders)

events:add("TickUpdate", function(e)
	tick = tick + 1

	events:invoke(eventGameUpdate, e, nil, EEventInvocation.None)
end, N_("GameUpdate"))

events:add("TickRender", function(e)
	events:invoke(eventGameRender, e)
end, N_("GameRender"), nil)

events:add("TickLoad", function(e)
	events:invoke(eventGameLoad, e)
end, N_("GameLoad"))

events:add("KeyDown", function(e)
	events:invoke(eventGameKeyDown, e)
end, N_("GameKeyDown"))

events:add("KeyUp", function(e)
	events:invoke(eventGameKeyUp, e)
end, N_("GameGameKeyUp"))

engine:triggerLoadPending()

return GCycle
