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

events:add("TickUpdate", function(e)
	tick = tick + 1

	network:update()

	-- events:invoke(GModule.eventGUpdate, e, nil, EEventInvocation.None)
end, N_("SharedUpdate"), "Shared")

engine:triggerLoadPending()

return GModule
