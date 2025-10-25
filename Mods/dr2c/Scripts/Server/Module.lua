--[[
-- @module dr2c.Server.Module
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

SModule.eventSUpdate = TE.events:new(N_("SUpdate"), {
	"Network",
	"Cleanup",
})

TE.events:add("TickUpdate", function(e)
	--- @class dr2c.E.SUpdate : TE.E.TickUpdate
	local e_ = e

	TE.events:invoke(SModule.eventSUpdate, e_)
end, N_("ServerUpdate"), "Server", "Primary")
