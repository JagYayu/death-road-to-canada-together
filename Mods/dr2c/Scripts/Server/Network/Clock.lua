--[[
-- @module dr2c.Server.Network.Clock
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local GNetworkMessage = require("dr2c.Shared.Network.Message")

local SNetworkServer = require("dr2c.Server.Network.Server")

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
	SNetworkServer.broadcastReliable(GNetworkMessage.Type.Clock, {
		timeOffset = serverClockTimeOffset,
	})
end, "ResponseClockTime", "Receive", GNetworkMessage.Type.Clock)

return SClock
