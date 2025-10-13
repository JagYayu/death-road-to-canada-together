--[[
-- @module dr2c.Server.Network.ServerAuthoritativeMessage
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("TE.Table")
local Enum = require("TE.Enum")
local List = require("TE.List")

local SNetworkClients = require("dr2c.Server.Network.Clients")
local SNetworkServer = require("dr2c.Server.Network.Server")

--- @class dr2c.SNetworkServerAuthoritativeMessage
local SNetworkServerAuthoritativeMessage = {}

local serverRegisteredAuthoritativeRequestsMap = {}

serverRegisteredAuthoritativeRequestsMap = persist("serverRegisteredAuthoritativeRequestsMap", function()
	return serverRegisteredAuthoritativeRequestsMap
end)

--- @param messageType dr2c.NetworkMessageType
--- @param messageContent table?
--- @return boolean success
local function sendReliableToAuthoritativeClient(messageType, messageContent)
	local clientID = SNetworkClients.getAuthoritativeClient()
	if not clientID then
		return false
	end

	return SNetworkServer.sendReliable(clientID, messageType, messageContent)
end

local requestMetatable = {
	--- @param self dr2c.ServerAuthoritativeRequest
	__call = function(self)
		sendReliableToAuthoritativeClient(self.messageType, {
			requestID = self.requestID,
			extras = self.args,
		})
	end,
}

--- @param messageType dr2c.NetworkMessageType
--- @param onReceived fun(response: any?): table?
--- @param args table
--- @return fun(contentExtras: any?)
function SNetworkServerAuthoritativeMessage.register(messageType, onReceived, args)
	if serverRegisteredAuthoritativeRequestsMap[messageType] then
		error(("Message type %s has already been registered!"):format(messageType), 2)
	elseif type(onReceived) ~= "function" then
		error(("Bad argument to #2 'onSuccess': function expected, got %s"):format(type(onReceived)), 2)
	end

	local latestRequestID = 0
	local requests = {}
	serverRegisteredAuthoritativeRequestsMap[messageType] = requests

	--- @param e dr2c.E.SMessage
	TE.events:add(N_("SMessage"), function(e)
		if not SNetworkClients.isAuthoritativeClient(e.clientID) then
			return
		end

		for _, request in ipairs(requests) do
			if request.requestID == e.content.requestID then
				e.broadcast = onReceived(e.content.response)

				break
			end
		end
	end, "ReceiveAuthoritativeMessage" .. messageType, "Receive", messageType)

	args = args or Table.empty
	local times = tonumber(args.times) or 4
	local interval = tonumber(args.interval) or 0.25

	--- @param requestArgs any?
	return function(requestArgs)
		local requestID = latestRequestID + 1
		latestRequestID = requestID

		--- @class dr2c.ServerAuthoritativeRequest
		--- @overload fun()
		local request = setmetatable({
			requestID = requestID,
			messageType = messageType,
			args = requestArgs,
			onTimeout = args.onTimeout,
			times = times - 1,
			interval = interval,
			time = Time.getSystemTime() + interval,
		}, requestMetatable)

		if times > 1 then
			requests[#requests + 1] = request
		end

		request()
	end
end

--- @param request dr2c.ServerAuthoritativeRequest
--- @param time number
local function serverAuthoritativeRequestHandle(request, _, time)
	if time < request.time then
		return false
	elseif request.times <= 0 then
		if request.onTimeout then
			request.onTimeout(request)
		end

		return true
	end

	request.time = time + request.interval
	request.times = request.times - 1
	request()

	return false
end

--- @param e dr2c.E.SUpdate
TE.events:add(N_("SUpdate"), function(e)
	for _, requests in pairs(serverRegisteredAuthoritativeRequestsMap) do
		if requests[1] then
			List.removeIfV(requests, serverAuthoritativeRequestHandle, Time.getSystemTime())
		end
	end
end, "UpdateServerAuthoritativeRequests", "Network", nil, 1)
