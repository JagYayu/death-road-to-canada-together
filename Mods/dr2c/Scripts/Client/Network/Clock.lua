--[[
-- @module dr2c.Client.Network.Clock
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CClient = require("dr2c.Client.Network.Client")

local CNetworkClientUtils = require("dr2c.Client.Network.ClientUtils")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local GUtilsThrottle = require("dr2c.Shared.Utils.Throttle")

--- @class dr2c.CNetworkClock
local CNetworkClock = {}

local Time_getSystemTime = Time.getSystemTime

--- 最新接收到的服务器时间
local serverTime = Time_getSystemTime()
--- 客户端与服务器时间的偏移，默认0无偏移，假定当前是权威客户端
local timeOffset = 0

serverTime = persist("serverTime", function()
	return serverTime
end)
timeOffset = persist("timeOffset", function()
	return timeOffset
end)

--- @return number
--- @nodiscard
function CNetworkClock.getServerTime()
	return serverTime
end

--- @return number time
--- @nodiscard
function CNetworkClock.getTime()
	return Time_getSystemTime() + timeOffset
end

--- @return number timeOffset
--- @nodiscard
function CNetworkClock.getTimeOffset()
	return timeOffset
end

--- Sync network clock.
--- If client has permission `GNetworkClient.Permission.Authority`, send local clock time to server.
--- Otherwise send request and receive clock time from server later.
--- 同步网络时钟。
--- 如果客户端有`GNetworkClient.Permission.Authority`权限，发送本地时钟给服务器。
--- 否则发送请求并在之后接收时钟。
function CNetworkClock.sync()
	if CNetworkClientUtils.hasPermissionAuthority() then
		CClient.sendReliable(GNetworkMessage.Type.SClock, Time.getSystemTime())
	else
		CClient.sendReliable(GNetworkMessage.Type.SClock)
	end
end

TE.events:add(N_("CMessage"), function(e)
	if CNetworkClientUtils.hasPermissionAuthority() then
		if log.canDebug() then
			log.debug("Received clock time from server, ignoring because of authoritative client.")
		end

		return
	end

	local serverSystemTime = tonumber(e.content)
	if not serverSystemTime then
		if log.canDebug() then
			log.debug(
				("Received clock time from server, ignoring because of invalid content type %s."):format(
					serverSystemTime
				)
			)
		end

		return
	end

	local clientSystemTime = Time.getSystemTime()

	serverTime = serverSystemTime
	timeOffset = clientSystemTime - serverTime

	if log.canTrace() then
		log.trace(("Received network clock from server. serverTime: %s, timeOffset: %s"):format( --
			serverTime,
			timeOffset
		))
	end
end, "ReceiveClockTime", "Receive", GNetworkMessage.Type.CClock)

TE.events:add(N_("CConnect"), function(e)
	CNetworkClock.sync()
end, "SyncClockTime", "Initialize")

local throttle = GUtilsThrottle.newTime(60)

--- @param e dr2c.E.CUpdate
TE.events:add(N_("CUpdate"), function(e)
	if throttle() then
		return
	end

	CNetworkClock.sync()
end, "SyncClockTime", "Network")

return CNetworkClock
