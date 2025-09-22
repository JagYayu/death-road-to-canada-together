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

events:add("TickUpdate", function(e)
	--- @class dr2c.E.SUpdate : Events.E.TickUpdate
	local e_ = e

	events:invoke(SModule.eventSUpdate, e_)
end, N_("ServerUpdate"), "Server", "Primary")
