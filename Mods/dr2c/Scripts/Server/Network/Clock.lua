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
local SNetworkClients = require("dr2c.Server.Network.Clients")

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

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	if SNetworkClients.getHostClient() == e.clientID then
		if log.canInfo() then
			log.info("Received network clock time from client %d, ignoring because its not the host client.")
		end

		return
	end

	local serverTime = tonumber(e.content)
	if not serverTime then
		return
	end

	time = serverTime

	if log.canInfo() then
		log.info(("Received network clock time from host client %d, broadcasting to all clients."):format(e.clientID))
	end

	SNetworkServer.broadcastReliable(GNetworkMessage.Type.CClock, time)
end, "ResponseClockTime", "Receive", GNetworkMessage.Type.SClock)

return SClock
