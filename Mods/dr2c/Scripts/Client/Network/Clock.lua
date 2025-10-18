--[[
-- @module dr2c.Client.network.Clock
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CClient = require("dr2c.Client.Network.Client")

local GNetworkMessage = require("dr2c.Shared.Network.Message")

--- @class dr2c.CNetworkClock
local CNetworkClock = {}

local clientClockTimeOffset = -Time.getStartupTime()

clientClockTimeOffset = persist("clientClockTimeOffset", function()
	return clientClockTimeOffset
end)

--- @return number time
function CNetworkClock.getTime()
	return Time.getSystemTime() + clientClockTimeOffset
end

function CNetworkClock.sync()
	CClient.sendReliable(GNetworkMessage.Type.Clock)
end

TE.events:add(N_("CMessage"), function(e)
	clientClockTimeOffset = e.content.timeOffset
end, "ReceiveClockTime", "Receive", GNetworkMessage.Type.Clock)

TE.events:add(N_("CConnect"), function(e)
	CNetworkClock.sync()
end, "SyncClockTime", "Initialize")

return CNetworkClock
