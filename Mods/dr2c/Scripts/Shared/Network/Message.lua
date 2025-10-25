--[[
-- @module dr2c.Shared.Network.Message
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")
local String = require("TE.String")

local Enum_hasValue = Enum.hasValue
local String_bufferDecode = String.bufferDecode
local String_bufferEncode = String.bufferEncode
local pcall = pcall
local tonumber = tonumber

--- @alias dr2c.NetworkMessageChannel dr2c.GNetworkMessage.Channel

--- @alias dr2c.MessageType dr2c.GNetworkMessage.Type

--- @class dr2c.GNetworkMessage
local GNetworkMessage = {}

GNetworkMessage.Type = Enum.sequence({
	Heartbeat = 0,

	--- Server send all existed client's public attributes to lately verified client.
	--- 服务器发送所有其他客户端的最新客户端公开属性给刚过验证的客户端。
	CClients = 1,
	CClientConnect = 2,
	CClientDisconnect = 3,

	--- 服务器广播一个客户端修改后的客户端公开属性给所有客户端，或发送一个失败原因给该客户端。
	CClientSetPublicAttribute = 4,
	--- 一个客户端尝试修改了一个客户端公开属性，将值发送给服务器。
	SClientSetPublicAttribute = 5,
	CClientSetPrivateAttribute = 6,
	--- 一个客户端尝试修改了一个客户端私有属性，将值发送给服务器。
	SClientSetPrivateAttribute = 7,
	--- RPC——客户端尝试获取一个客户端私有属性，服务器下发给所有其他客户端
	ClientGetPrivateAttribute = 8,
	--- 服务器广播修改后的服务器属性给所有客户端，或者发送一个失败原因给修改者客户端。
	CServerAttribute = 10,
	--- 一个客户端尝试修改了服务器的属性，将值发送给服务器。
	SServerAttribute = 11,
	--- 服务器将所有属性发送给刚通过验证的客户端。
	CServerAttributes = 12,
	--- 服务器广播修改后的网络时钟。
	CClock = 13,
	--- 客户端修改服务器网络时钟。
	SClock = 14,
	CRooms = 15,
	CRoomAttribute = 16,
	SRoomAttribute = 17,
	CCreateRoom = 18,
	SCreateRoom = 19,
	CDestroyRoom = 20,
	SDestroyRoom = 21,

	ForthSessionAttributes = 101,
	ForthSessionStart = 102,
	ForthSessionFinish = 103,

	CWorldSessionAttributes = 201,
	CWorldSessionAttribute = 202,
	SWorldSessionAttribute = 203,
	CWorldSessionStart = 204,
	SWorldSessionStart = 205,
	CWorldSessionRestart = 206,
	SWorldSessionRestart = 207,
	CWorldSessionFinish = 208,
	SWorldSessionFinish = 209,
	CWorldSessionPause = 210,
	SWorldSessionPause = 211,
	CWorldSessionUnpause = 212,
	SWorldSessionUnpause = 213,

	--- 服务器向客户端发送玩家输入缓存
	CPlayerInputBuffers = 301,
	--- 服务器向客户端发送玩家输入
	CPlayerInputs = 302,
	--- 客户端向服务器发送玩家输入
	SPlayerInputs = 303,
	--- [RPC]客户端向服务器请求玩家输入缓冲
	ClientPlayerInputBuffersRequest = 304,
	--- [RPC]客户端向服务器请求快照
	ClientSnapshotRequest = 305,
	--- [RPC]服务器向权威客户端请求快照
	ServerSnapshotRequest = 306,
})

local GNetworkMessage_Type = GNetworkMessage.Type
local GNetworkMessage_Type_Heartbeat = GNetworkMessage_Type.Heartbeat

GNetworkMessage.Channel = Enum.immutable({
	Main = 0,
	Chat = 1,

	-- Placeholders *may* be deprecated in the future.
	-- You cannot have a channel that value is greater than 7

	--- placeholder 1
	_1 = 1,
	--- placeholder 2
	_2 = 2,
	--- placeholder 3
	_3 = 3,
	--- placeholder 4
	_4 = 4,
	--- placeholder 5
	_5 = 5,
	--- placeholder 6
	_6 = 6,
	--- placeholder 7
	_7 = 7,
})

--- @type table<dr2c.MessageType, true>
local unessentialMessageTypes = {}

--- 将消息类型标记为*非必要的*。
--- 非必要的消息无需经过事件`SMessage`手动广播/发送，直接由引擎转发。
--- @param messageType dr2c.MessageType
function GNetworkMessage.markAsUnessential(messageType)
	unessentialMessageTypes[messageType] = true
end

---
--- @param messageType dr2c.MessageType
--- @return boolean
function GNetworkMessage.isEssential(messageType)
	return not unessentialMessageTypes[messageType]
end

--- @param messageType dr2c.MessageType
--- @return boolean
function GNetworkMessage.isUnessential(messageType)
	return not not unessentialMessageTypes[messageType]
end

local encodingTable = { 0, nil }

--- @param messageType dr2c.MessageType
--- @param messageContent? any
--- @return string messagePacket
function GNetworkMessage.pack(messageType, messageContent)
	encodingTable[1] = tonumber(messageType) or GNetworkMessage_Type_Heartbeat
	encodingTable[2] = messageContent
	return String_bufferEncode(encodingTable)
end

--- @param message string
--- @return dr2c.MessageType
--- @return any?
--- @nodiscard
local function unpackImpl(message)
	local messageTable = String_bufferDecode(message)
	--- @diagnostic disable-next-line: need-check-nil
	local messageValue, messageType = messageTable[1], messageTable[2]

	if Enum_hasValue(GNetworkMessage_Type, messageValue) then
		return messageValue, messageType
	else
		return GNetworkMessage_Type_Heartbeat, messageType
	end
end

--- @param messagePacket? string
--- @return dr2c.MessageType
--- @return any?
--- @nodiscard
function GNetworkMessage.unpack(messagePacket)
	if messagePacket then
		local success, messageType, messageContent = pcall(unpackImpl, messagePacket)
		if not success then
			error("Message unpack failed: " .. messageType)
		end

		return messageType, messageContent
	else
		return GNetworkMessage_Type_Heartbeat, nil
	end
end

return GNetworkMessage
