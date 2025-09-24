--[[
-- @module dr2c.server.network.Clock
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local GMessage = require("dr2c.shared.network.Message")

local SServer = require("dr2c.server.network.Server")

--- @class dr2c.SClock
local SClock = {}

local serverClockTimeOffset = -Time.getSystemTime()

serverClockTimeOffset = persist("serverClockTimeOffset", function()
	return serverClockTimeOffset
end)

function SClock.getTime()
	return Time.getSystemTime() + serverClockTimeOffset
end

TE.events:add(N_("SMessage"), function(e)
	SServer.broadcastReliable(GMessage.Type.Clock, {
		timeOffset = serverClockTimeOffset,
	})
end, "ResponseClockTime", "Receive", GMessage.Type.Clock)

return SClock
