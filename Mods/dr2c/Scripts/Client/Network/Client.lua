--[[
-- @module dr2c.Client.Network.Client
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")
local List = require("TE.List")
local Table = require("TE.Table")

local CNetworkClients = require("dr2c.Client.Network.Clients")
local GNetworkClient = require("dr2c.Shared.Network.Client")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local GUtilsThrottle = require("dr2c.Shared.Utils.Throttle")

local network = TE.network

--- @class dr2c.ClientAttributeSettingRequest
--- @field attribute dr2c.ClientPrivateAttribute
--- @field callback fun(success: boolean, reason?: dr2c.NetworkReasonID)
--- @field expireTime number

--- @class dr2c.CNetworkClient
local CNetworkClient = {}

-- --- @type table<integer, dr2c.ClientAttributeSettingRequest>
-- local attributeSettingRequests = {}
--- @type number
local latencySimulation = 0
--- @type { time: number, func: fun() }[]
local latencySimulatedQueue = {}

--- @type integer
local sessionSlot = 0

latencySimulation = persist("latencySimulation", function()
	return latencySimulation
end)
latencySimulatedQueue = persist("latencySimulatedQueue", function()
	return latencySimulatedQueue
end)

--- @return TE.Network.Client?
function CNetworkClient.getNetworkSession()
	return network:getClient(sessionSlot)
end

--- @return TE.Network.ClientID?
function CNetworkClient.getClientID()
	local session = CNetworkClient.getNetworkSession()
	local clientID = session and session:getSessionID()
	if clientID and clientID ~= 0 then
		return clientID
	end
end

--- @param publicAttribute dr2c.ClientPublicAttribute
--- @return any?
--- @nodiscard
function CNetworkClient.getPublicAttribute(publicAttribute)
	local clientID = CNetworkClient.getClientID()
	if clientID then
		if not CNetworkClients.hasClient(clientID) then
			CNetworkClients.addClient(clientID)
		end

		return CNetworkClients.getPublicAttribute(clientID, publicAttribute)
	end
end

--- @param privateAttribute dr2c.ClientPrivateAttribute
--- @return any?
--- @nodiscard
function CNetworkClient.getPrivateAttribute(privateAttribute)
	local clientID = CNetworkClient.getClientID()
	if clientID then
		return CNetworkClients.getPrivateAttribute(clientID, privateAttribute)
	end
end

--- @param setFunction function
--- @param attribute dr2c.ClientPublicAttribute | dr2c.ClientPrivateAttribute
--- @param value any
--- @return boolean
local function SetAttribute(setFunction, attribute, value)
	local clientID = CNetworkClient.getClientID()
	if not clientID then
		return false
	end

	if setFunction == CNetworkClients.setPublicAttribute then
		local fields = GNetworkMessageFields.SClientSetPublicAttribute
		CNetworkClient.sendReliable(GNetworkMessage.Type.SClientSetPublicAttribute, {
			[fields.attribute] = attribute,
			[fields.value] = value,
		})
	else
		local fields = GNetworkMessageFields.SClientSetPrivateAttribute
		CNetworkClient.sendReliable(GNetworkMessage.Type.SClientSetPrivateAttribute, {
			[fields.attribute] = attribute,
			[fields.value] = value,
		})
	end

	return true
end

--- 修改客户端公开属性
--- @warn 属性修改需经过服务器验证，最好传递callback追踪结果
--- @param publicAttribute dr2c.ClientPublicAttribute
--- @param attributeValue any
--- @return boolean
function CNetworkClient.setPublicAttribute(publicAttribute, attributeValue)
	return SetAttribute(CNetworkClients.setPublicAttribute, publicAttribute, attributeValue)
end

--- 修改客户端公开属性
--- @warn 属性修改需经过服务器验证，最好传递callback追踪结果
--- @param privateAttribute dr2c.ClientPrivateAttribute
--- @param attributeValue any
--- @return boolean
function CNetworkClient.setPrivateAttribute(privateAttribute, attributeValue)
	return SetAttribute(CNetworkClients.setPrivateAttribute, privateAttribute, attributeValue)
end

--- 模拟客户端延迟
--- @param latency? number @单位s
function CNetworkClient.simulateLatency(latency)
	local deltaTime = latencySimulation
	latencySimulation = math.max(0, tonumber(latency))
	deltaTime = latencySimulation - deltaTime

	if deltaTime ~= 0 then
		for _, entry in ipairs(latencySimulatedQueue) do
			entry.time = entry.time + deltaTime
		end
	end
end

--- @param func fun(): boolean
--- @return boolean
local function delaySendFunction(func)
	latencySimulatedQueue[#latencySimulatedQueue + 1] = {
		time = Time.getSystemTime() + latencySimulation,
		func = func,
	}

	return not not CNetworkClient.getNetworkSession()
end

--- @param messageType dr2c.MessageType
--- @param messageContent? any
--- @param messageChannel dr2c.NetworkMessageChannel
--- @return boolean
local function sendReliable(messageType, messageContent, messageChannel)
	local session = CNetworkClient.getNetworkSession()
	if session then
		local messagePacket = GNetworkMessage.pack(messageType, messageContent)

		if log.canTrace() then
			log.trace(("Send reliable message to server: %s"):format(messagePacket))
		end

		---@diagnostic disable-next-line: param-type-mismatch
		session:sendReliable(messagePacket, messageChannel)

		return true
	else
		return false
	end
end

--- @param messageType dr2c.MessageType
--- @param messageContent any?
--- @param messageChannel dr2c.NetworkMessageChannel
--- @return boolean success
local function sendUnreliable(messageType, messageContent, messageChannel)
	local session = CNetworkClient.getNetworkSession()
	if session then
		local data = GNetworkMessage.pack(messageType, messageContent)

		if log.canTrace() then
			log.trace(("Send unreliable message to server: %s"):format(data))
		end

		---@diagnostic disable-next-line: param-type-mismatch
		session:sendUnreliable(data, messageChannel)

		return true
	else
		return false
	end
end

--- @param messageType dr2c.MessageType
--- @param messageContent? any
--- @param messageChannel? dr2c.NetworkMessageChannel
--- @return boolean success
function CNetworkClient.sendReliable(messageType, messageContent, messageChannel)
	messageType = tonumber(messageType) or GNetworkMessage.Type.Heartbeat
	messageChannel = messageChannel or GNetworkMessage.Channel.Main

	if latencySimulation > 0 then
		messageContent = type(messageContent) == "table" and Table.fastCopy(messageContent) or messageContent
		return delaySendFunction(function()
			return sendReliable(messageType, messageContent, messageChannel)
		end)
	else
		return sendReliable(messageType, messageContent, messageChannel)
	end
end

--- @param messageType dr2c.MessageType
--- @param messageContent any?
--- @param messageChannel dr2c.NetworkMessageChannel?
--- @return boolean success
function CNetworkClient.sendUnreliable(messageType, messageContent, messageChannel)
	messageType = tonumber(messageType) or GNetworkMessage.Type.Heartbeat
	messageChannel = messageChannel or GNetworkMessage.Channel.Main

	if latencySimulation > 0 then
		messageContent = type(messageContent) == "table" and Table.fastCopy(messageContent) or messageContent
		return delaySendFunction(function()
			return sendUnreliable(messageType, messageContent, messageChannel)
		end)
	else
		return sendUnreliable(messageType, messageContent, messageChannel)
	end
end

--- @param entry { time: number, func: fun(): boolean }
--- @param _ integer
--- @param time number
--- @return boolean
local function updateLatencySimulatedEntry(entry, _, time)
	if time < entry.time then
		return false
	end

	entry.func()
	return true
end

function CNetworkClient.updateLatencySimulatedQueue()
	List.removeIfV(latencySimulatedQueue, updateLatencySimulatedEntry, Time.getSystemTime())
end

--- comment
function CNetworkClient.getRoomID() end

CNetworkClient.eventCConnect = TE.events:new(N_("CConnect"), {
	"Reset",
	"Initialize",
})

CNetworkClient.eventCDisconnect = TE.events:new(N_("CDisconnect"), {
	"Deinitialize",
	"Reset",
})

CNetworkClient.eventCMessage = TE.events:new(N_("CMessage"), {
	"Overrides",
	"Receive",
	"Request",
	"Response",
	"Effect",
	"Rollback",
}, Enum.eventKeys(GNetworkMessage.Type))

local function invokeEventClientConnect()
	--- @class dr2c.E.ClientConnect
	local e = {}
	TE.events:invoke(CNetworkClient.eventCConnect, e)
end

--- @param e TE.E.ClientConnect
TE.events:add("ClientConnect", function(e)
	invokeEventClientConnect()
end)

local function invokeEventClientDisconnect(clientID)
	--- @class dr2c.E.ClientDisconnect
	local e = {
		clientID = clientID,
	}

	TE.events:invoke(CNetworkClient.eventCDisconnect, e)
end

--- @param e TE.E.ClientConnect
TE.events:add("ClientDisconnect", function(e)
	invokeEventClientDisconnect(e.data.clientID)
end)

--- @param messageType dr2c.MessageType
--- @param messageContent any?
local function invokeEventClientMessage(messageType, messageContent)
	--- @class dr2c.E.CMessage
	--- @field type dr2c.MessageType
	--- @field content any?
	local e = {
		type = messageType,
		content = messageContent,
	}

	TE.events:invoke(CNetworkClient.eventCMessage, e, messageType)
end

--- @param e TE.E.ClientMessage
TE.events:add("ClientMessage", function(e)
	if type(e.data) ~= "userdata" then
		return
	end

	local messageType, messageContent = GNetworkMessage.unpack(e.data.message)

	invokeEventClientMessage(messageType, messageContent)
end, "clientMessage", nil, sessionSlot)

TE.events:add(N_("CUpdate"), CNetworkClient.updateLatencySimulatedQueue, "SendClientDelayedMessages", "Network")

local throttleClientUpdateClientsPrivateAttributeRequests = GUtilsThrottle.newTime(0.5)

--- @param e dr2c.E.CUpdate
TE.events:add(N_("CUpdate"), function(e)
	--- @class dr2c.E.CUpdate : TE.E.TickUpdate
	--- @field clientNetworkThrottle? boolean
	e = e

	e.clientNetworkThrottle = throttleClientUpdateClientsPrivateAttributeRequests()
end, "AssignNetworkThrottle", "Throttle")

return CNetworkClient
