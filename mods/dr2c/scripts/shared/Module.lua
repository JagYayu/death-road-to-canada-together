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

-- local CommonOrders = {
-- 	"Shared",
-- 	"Client",
-- 	"Server",
-- }

-- GModule.eventGUpdate = events:new(N_("GUpdate"), CommonOrders)

-- GModule.eventGRender = events:new(N_("GRender"), CommonOrders)

-- GModule.eventGLoad = events:new(N_("GLoad"), CommonOrders)

-- GModule.eventGKeyboardPress = events:new(N_("GKeyboardPress"), CommonOrders)

-- GModule.eventGKeyUp = events:new(N_("GKeyUp"), CommonOrders)

events:add("TickUpdate", function(e)
	tick = tick + 1

	network:update()

	-- events:invoke(GModule.eventGUpdate, e, nil, EEventInvocation.None)
end, N_("SharedUpdate"), "Shared")

-- events:add("TickRender", function(e)
-- 	events:invoke(GModule.eventGRender, e, nil, EEventInvocation.None)
-- end, N_("GameRender"), nil)

-- events:add("TickLoad", function(e)
-- 	events:invoke(GModule.eventGLoad, e, nil, EEventInvocation.None)
-- end, N_("GameLoad"))

-- events:add("KeyboardPress", function(e)
-- 	events:invoke(GModule.eventGKeyboardPress, e, nil, EEventInvocation.None)
-- end, N_("GameKeyboardPress"))

-- events:add("KeyUp", function(e)
-- 	events:invoke(GModule.eventGKeyUp, e, nil, EEventInvocation.None)
-- end, N_("GameGameKeyUp"))

engine:triggerLoadPending()

return GModule
