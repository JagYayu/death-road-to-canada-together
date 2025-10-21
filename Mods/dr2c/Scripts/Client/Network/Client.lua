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

local String = require("TE.String")
local Enum = require("TE.Enum")
local EnumFlag = require("TE.EnumFlag")
local List = require("TE.List")
local Table = require("TE.Table")

local CNetworkClients = require("dr2c.Client.Network.Clients")
local GNetworkClient = require("dr2c.Shared.Network.Client")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local GNetworkPlatform = require("dr2c.Client.Network.Platform")
local GUtilsThrottle = require("dr2c.Shared.Utils.Throttle")

local network = TE.network

--- @class dr2c.CNetworkClient
local CNetworkClient = {}

--- @type table<dr2c.NetworkClientPrivateAttribute, any?>
local clientPrivateAttributes = {}
--- @type number
local latencySimulation = 0
--- @type { time: number, func: fun() }[]
local latencySimulatedQueue = {}

--- @type integer
local sessionSlot = 0

clientPrivateAttributes = persist("clientPrivateAttributes", function()
	return clientPrivateAttributes
end)
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
	return session and session:getSessionID()
end

--- @param publicAttribute dr2c.NetworkClientPublicAttribute
--- @return any?
function CNetworkClient.getPublicAttribute(publicAttribute)
	local clientID = CNetworkClient.getClientID()
	if clientID then
		return CNetworkClients.getPublicAttribute(clientID, publicAttribute)
	end
end

--- @param privateAttribute dr2c.NetworkClientPrivateAttribute
--- @return any?
function CNetworkClient.getPrivateAttribute(privateAttribute)
	return clientPrivateAttributes[privateAttribute]
end

function CNetworkClient.hasPermissionAuthority()
	local permissions = CNetworkClient.getPublicAttribute(GNetworkClient.PublicAttribute.Permissions)
	return permissions and EnumFlag.hasAny(permissions, GNetworkClient.Permission.Authority) or false
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

--- @param messageType dr2c.NetworkMessageType
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

--- @param messageType dr2c.NetworkMessageType
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

--- @param messageType dr2c.NetworkMessageType
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

--- @param messageType dr2c.NetworkMessageType
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
	if time >= entry.time then
		entry.func()
		return true
	else
		return false
	end
end

function CNetworkClient.updateLatencySimulatedQueue()
	List.removeIfV(latencySimulatedQueue, updateLatencySimulatedEntry, Time.getSystemTime())
end

CNetworkClient.eventCCollectVerifyAttributes = TE.events:new(N_("CCollectVerifyAttributes"), {
	"Public",
	"Private",
	"State",
})

TE.events:add(CNetworkClient.eventCCollectVerifyAttributes, function(e)
	e.attributes[#e.attributes + 1] = {
		type = GNetworkClient.PublicAttribute,
		attribute = GNetworkClient.PublicAttribute.State,
		value = GNetworkClient.State.Verified,
	}
end, "SendState", "State")

local function invokeEventClientCollectVerifyAttributes()
	local attributes = {}

	--- @class dr2c.E.ClientCollectVerifyAttributes
	local e = {
		attributes = attributes,
	}

	TE.events:invoke(CNetworkClient.eventCCollectVerifyAttributes, e)

	return attributes
end

function CNetworkClient.sendVerifyingAttributes()
	local entries = invokeEventClientCollectVerifyAttributes()

	for _, entry in ipairs(entries) do
		if entry.type == GNetworkClient.PublicAttribute then
			local fields = GNetworkMessageFields.ClientPublicAttribute
			CNetworkClient.sendReliable(GNetworkMessage.Type.ClientPublicAttribute, {
				[fields.attribute] = entry.attribute,
				[fields.value] = entry.value,
			})
		elseif entry.type == GNetworkClient.PrivateAttribute then
			local fields = GNetworkMessageFields.ClientPrivateAttribute
			CNetworkClient.sendReliable(GNetworkMessage.Type.ClientPrivateAttribute, {
				[fields.attribute] = entry.attribute,
				[fields.value] = entry.value,
			})
		end
	end

	return entries
end

CNetworkClient.eventCConnect = TE.events:new(N_("CConnect"), {
	"Reset",
	"Initialize",
})

CNetworkClient.eventCDisconnect = TE.events:new(N_("CDisconnect"), {
	"Reset",
})

CNetworkClient.eventCMessage = TE.events:new(N_("CMessage"), {
	"Overrides",
	"Receive",
	"PlayerInputBuffer",
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

--- @param messageType dr2c.NetworkMessageType
--- @param messageContent any?
local function invokeEventClientMessage(messageType, messageContent)
	--- @class dr2c.E.CMessage
	--- @field type dr2c.NetworkMessageType
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
	e.networkThrottle = throttleClientUpdateClientsPrivateAttributeRequests()
end, "AssignNetworkThrottle", "Throttle")

local sendClientVerifyingAttributesTime = 0

--- @param e dr2c.E.CUpdate
TE.events:add(N_("CUpdate"), function(e)
	if e.networkThrottle then
		return
	end

	local clientID = CNetworkClient.getClientID()
	if not clientID then
		return
	end

	local time = Time.getSystemTime()
	if time < sendClientVerifyingAttributesTime then
		return
	end

	local state = CNetworkClients.getPublicAttribute(clientID, GNetworkClient.PublicAttribute.State)
	if state ~= GNetworkClient.State.Verifying then
		return
	end

	local attributes = CNetworkClient.sendVerifyingAttributes()
	if log.canTrace() then
		log.trace("Collected verify attributes: ", attributes)
	end

	sendClientVerifyingAttributesTime = time + 5
end, "SendClientVerifyingAttributes", "Network")

return CNetworkClient
