--[[
-- @module dr2c.Shared.Network.RPC
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Function = require("TE.Function")
local List = require("TE.List")
local Math = require("TE.Math")
local Utility = require("TE.Utility")

--- @class dr2c.NetworkRemoteRequestHandle : integer

--- @alias dr2c.NetworkRemoteRequestCallback fun(messageContent?: table, ...): (extendTime: number?)

--- @class dr2c.NetworkRemoteRequest
--- @field handle dr2c.NetworkRemoteRequestHandle
--- @field callback dr2c.NetworkRemoteRequestCallback
--- @field time number

--- @class dr2c.GNetworkRPC
local GNetworkRPC = {}

local modules = {}
--- @type table<dr2c.MessageType, true>
local clientResponsibleMessageTypes = {}
--- @type table<dr2c.MessageType, true>
local serverResponsibleMessageTypes = {}

modules = persist("modules", function()
	return modules
end)
clientResponsibleMessageTypes = persist("clientResponsibleMessageTypes", function()
	return clientResponsibleMessageTypes
end)
serverResponsibleMessageTypes = persist("serverResponsibleMessageTypes", function()
	return serverResponsibleMessageTypes
end)

--- @return dr2c.MNetworkRPC module
function GNetworkRPC.new()
	local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")

	--- @class dr2c.MNetworkRPC
	local MNetworkRPC = {}

	--- @type dr2c.NetworkRemoteRequestHandle
	local currentRequestHandle = RandomDevice.generate(0, Math.intMax)
	--- @type table<dr2c.MessageType, dr2c.NetworkRemoteRequest[]>
	local remotePendingRequestsLists = {}
	--- @type table<dr2c.NetworkRemoteRequestHandle, dr2c.MessageType>
	local requestHandleMessageTypes = {}

	Utility.persistModule(modules, function()
		return { currentRequestHandle, remotePendingRequestsLists, requestHandleMessageTypes }
	end, function(t)
		currentRequestHandle, remotePendingRequestsLists, requestHandleMessageTypes = t[1], t[2], t[3]
	end)

	--- @return dr2c.NetworkRemoteRequestHandle
	function MNetworkRPC.newHandle()
		currentRequestHandle = currentRequestHandle + 1

		if currentRequestHandle >= Math.intMax then
			currentRequestHandle = 0
		end

		return currentRequestHandle
	end

	--- @param requestHandle dr2c.NetworkRemoteRequestHandle
	--- @return boolean
	function MNetworkRPC.hasRequest(requestHandle)
		return not not requestHandleMessageTypes[requestHandle]
	end

	--- @param messageType dr2c.MessageType
	--- @param callback dr2c.NetworkRemoteRequestCallback
	--- @param timeout? number
	--- @return dr2c.NetworkRemoteRequestHandle requestHandle
	function MNetworkRPC.addRequest(messageType, callback, timeout, ...)
		local list = remotePendingRequestsLists[messageType]
		if not list then
			list = {}
			remotePendingRequestsLists[messageType] = list
		end

		local requestHandle = MNetworkRPC.newHandle()
		local request = {
			handle = requestHandle,
			callback = callback,
			time = Time.getSystemTime() + (tonumber(timeout) or 10),
			...,
		}
		list[#list + 1] = request
		requestHandleMessageTypes[requestHandle] = messageType

		if #list > 1000 and log.canWarn() then
			log.warn("Request number is exceed 1000!")
		end

		return requestHandle
	end

	--- @param request dr2c.NetworkRemoteRequest
	--- @param requestHandle dr2c.NetworkRemoteRequestHandle
	--- @return boolean
	local function conditionRemoveRequest(request, _, requestHandle)
		return request.handle == requestHandle
	end

	--- @param requestHandle dr2c.NetworkRemoteRequestHandle
	function MNetworkRPC.removeRequest(requestHandle)
		local messageType = requestHandleMessageTypes[requestHandle]
		if not messageType then
			error(("Request handle %d does not exists"):format(requestHandle), 2)
		end

		local list = remotePendingRequestsLists[messageType]
		assert(list)
		assert(List.fastRemoveFirstIfV(list, conditionRemoveRequest, requestHandle))
		requestHandleMessageTypes[messageType] = nil

		return true
	end

	--- @param messageType dr2c.MessageType
	--- @param messageContent table
	--- @return boolean received
	function MNetworkRPC.receiveMessage(messageType, messageContent)
		local list = remotePendingRequestsLists[messageType]
		if not list then
			return false
		end

		local requestHandle = messageContent[GNetworkMessageFields.requestHandle]
		if not requestHandle then
			return false
		end

		for index, request in ipairs(list) do
			if request.handle == requestHandle then
				request.callback(messageContent, unpack(request))

				list[index] = list[#list]
				list[#list] = nil

				requestHandleMessageTypes[requestHandle] = nil

				break
			end
		end

		if not list[1] then
			remotePendingRequestsLists[messageType] = nil
		end

		return true
	end

	--- @param request dr2c.NetworkRemoteRequest
	--- @param time number
	local function conditionIsRequestExpired(request, _, time)
		if time < request.time then
			return false
		end

		local extendTime = tonumber(request.callback(nil, unpack(request)))
		if extendTime and extendTime > 0 then
			request.time = request.time + extendTime
			return false
		end

		requestHandleMessageTypes[request.handle] = nil
		return true
	end

	function MNetworkRPC.cleanupExpiredRequests()
		local time = Time.getSystemTime()

		for messageType, requests in pairs(remotePendingRequestsLists) do
			List.removeIfV(requests, conditionIsRequestExpired, time)

			if not requests[1] then
				remotePendingRequestsLists[messageType] = nil
			end
		end
	end

	return MNetworkRPC
end

if true then
	--- @param messageType dr2c.MessageType
	--- @return boolean
	--- @nodiscard
	function GNetworkRPC.hasClientResponse(messageType)
		return clientResponsibleMessageTypes[messageType]
	end

	--- @param messageType dr2c.MessageType
	--- @return boolean
	--- @nodiscard
	function GNetworkRPC.hasServerResponse(messageType)
		return serverResponsibleMessageTypes[messageType]
	end
else -- TODO 若进程为纯客户端或专用服务器，则无需验证
	GNetworkRPC.hasClientResponse = Function.alwaysTrue
	GNetworkRPC.hasServerResponse = Function.alwaysTrue
end

-- TODO 下面的都还没完成

--- @param messageType dr2c.MessageType
function GNetworkRPC.registerClientResponse(messageType, handler)
	if clientResponsibleMessageTypes[messageType] then
		error(("Already registered client response for message type %s"):format(messageType), 2)
	end

	--- @param e dr2c.E.CMessage
	TE.events:add(N_("CMessage"), function(e)
		if type(e.content) ~= "table" then
			return
		end
	end, "Response" .. messageType, "Response", messageType)

	clientResponsibleMessageTypes[messageType] = true
end

--- @param messageType dr2c.MessageType
function GNetworkRPC.registerServerResponse(messageType)
	if serverResponsibleMessageTypes[messageType] then
		error(("Already registered server response for message type %s"):format(messageType), 2)
	end

	serverResponsibleMessageTypes[messageType] = true
end

-- ! 上面的都还没完成

return GNetworkRPC
