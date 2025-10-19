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

local time = 0

time = persist("time", function()
	return time
end)

function SClock.getTime()
	return time
end

TE.events:add(N_("SMessage"), function(e)
	-- TODO 只允许权威客户端

	local serverTime = tonumber(e.content)
	if serverTime then
		time = serverTime

		SNetworkServer.broadcastReliable(GNetworkMessage.Type.Clock, time)
	end
end, "ResponseClockTime", "Receive", GNetworkMessage.Type.Clock)

return SClock
