--[[
-- @module dr2c.shared.Cycle
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

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

GCycle.eventGUpdate = events:new(N_("GUpdate"), CommonOrders)

GCycle.eventGRender = events:new(N_("GRender"), CommonOrders)

GCycle.eventGLoad = events:new(N_("GLoad"), CommonOrders)

GCycle.eventGKeyDown = events:new(N_("GKeyDown"), CommonOrders)

GCycle.eventGKeyUp = events:new(N_("GKeyUp"), CommonOrders)

events:add("TickUpdate", function(e)
	tick = tick + 1

	network:update()

	events:invoke(GCycle.eventGUpdate, e, nil, EEventInvocation.None)

	-- collectgarbage()
end, N_("GameUpdate"))

events:add("TickRender", function(e)
	events:invoke(GCycle.eventGRender, e, nil, EEventInvocation.None)
end, N_("GameRender"), nil)

events:add("TickLoad", function(e)
	events:invoke(GCycle.eventGLoad, e, nil, EEventInvocation.None)
end, N_("GameLoad"))

events:add("KeyDown", function(e)
	events:invoke(GCycle.eventGKeyDown, e, nil, EEventInvocation.None)
end, N_("GameKeyDown"))

events:add("KeyUp", function(e)
	events:invoke(GCycle.eventGKeyUp, e, nil, EEventInvocation.None)
end, N_("GameGameKeyUp"))

engine:triggerLoadPending()

return GCycle
