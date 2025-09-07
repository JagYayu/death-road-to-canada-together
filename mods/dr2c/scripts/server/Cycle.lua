--[[
-- @module dr2c.server.network.Update
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class dr2c.SCycle
local SCycle = {}

SCycle.eventSUpdate = events:new(N_("SUpdate"), {
	"Network",
})

events:add(N_("GUpdate"), function(e)
	--- @class dr2c.E.SUpdate : dr2c.E.GUpdate
	events:invoke(SCycle.eventSUpdate, e)
end, N_("ServerUpdate"), nil, "Main")
