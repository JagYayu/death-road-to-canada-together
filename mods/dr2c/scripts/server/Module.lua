--[[
-- @module dr2c.server.Module
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class dr2c.SModule
local SModule = {}

SModule.eventSUpdate = events:new(N_("SUpdate"), {
	"Network",
})

events:add(N_("GUpdate"), function(e)
	--- @class dr2c.E.SUpdate : dr2c.E.GUpdate
	events:invoke(SModule.eventSUpdate, e)
end, N_("ServerUpdate"), nil, "Main")
