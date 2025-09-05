--[[
-- @module dr2c.client.network.Clock
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CClient = require("dr2c.client.network.Client")

local GMessage = require("dr2c.shared.network.Message")

--- @class dr2c.CClock
local CClock = {}

local clientClockTimeOffset = -Time.getStartupTime()

clientClockTimeOffset = persist("clientClockTimeOffset", function()
	return clientClockTimeOffset
end)

--- @return number time
function CClock.getTime()
	return Time.getSystemTime() + clientClockTimeOffset
end

function CClock.sync()
	CClient.sendReliable(GMessage.Type.Clock)
end

events:add(N_("CMessage"), function(e)
	clientClockTimeOffset = e.content.timeOffset
end, "ReceiveClockTime", "Receive", GMessage.Type.Clock)

events:add(N_("CConnect"), function(e)
	CClock.sync()
end, "SyncClockTime", "Initialize")

return CClock
