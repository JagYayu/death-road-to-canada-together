--[[
-- @module dr2c.shared.Module
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class dr2c.GCycle
local GModule = {}

local tick = 0
tick = persist("tick", function()
	return tick
end)

function GModule.getTick()
	return tick
end

local CommonOrders = {
	"Client",
	"Server",
}

GModule.eventGUpdate = events:new(N_("GUpdate"), CommonOrders)

GModule.eventGRender = events:new(N_("GRender"), CommonOrders)

GModule.eventGLoad = events:new(N_("GLoad"), CommonOrders)

GModule.eventGKeyDown = events:new(N_("GKeyDown"), CommonOrders)

GModule.eventGKeyUp = events:new(N_("GKeyUp"), CommonOrders)

events:add("TickUpdate", function(e)
	tick = tick + 1

	network:update()

	events:invoke(GModule.eventGUpdate, e, nil, EEventInvocation.None)

	-- collectgarbage()
end, N_("GameUpdate"))

events:add("TickRender", function(e)
	events:invoke(GModule.eventGRender, e, nil, EEventInvocation.None)
end, N_("GameRender"), nil)

events:add("TickLoad", function(e)
	events:invoke(GModule.eventGLoad, e, nil, EEventInvocation.None)
end, N_("GameLoad"))

events:add("KeyDown", function(e)
	events:invoke(GModule.eventGKeyDown, e, nil, EEventInvocation.None)
end, N_("GameKeyDown"))

events:add("KeyUp", function(e)
	events:invoke(GModule.eventGKeyUp, e, nil, EEventInvocation.None)
end, N_("GameGameKeyUp"))

engine:triggerLoadPending()

return GModule
