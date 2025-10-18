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

local String = require("TE.String")
local Enum = require("TE.Enum")
local stringBuffer = require("string.buffer")

local Enum_hasValue = Enum.hasValue
local String_bufferDecode = String.bufferDecode
local String_bufferEncode = String.bufferEncode
local pcall = pcall
local tonumber = tonumber

--- @alias dr2c.NetworkMessageChannel dr2c.GNetworkMessage.Channel

--- @alias dr2c.NetworkMessageType dr2c.GNetworkMessage.Type

--- @class dr2c.GNetworkMessage
local GNetworkMessage = {}

GNetworkMessage.Type = Enum.sequence({
	Heartbeat = 0,

	--- {en}
	--- Server --> Client: Server send all existed client's public attributes to lately verified client.
	--- {zn_CN}
	--- 服务器 --> 客户端: 服务器给刚过验证的客户端发送所有其他客户端的最新公开属性。
	Clients = 1,
	--- Server --> Client
	ClientConnect = 2,
	--- {en}
	--- Server --> Client
	ClientDisconnect = 3,
	--- {en}
	--- Client --> Server: A client attempt to change a public attribute, send value to server.
	--- Server --> Client: Server broadcast a client's modified public attribute to all other clients.
	--- {zh_CN}
	--- 客户端 --> 服务器: 一个客户端尝试修改了一个公开属性，将值发送给服务器。
	--- 服务器 --> 客户端: 服务器发送一个客户端修改后的公开属性给所有其他客户端。
	ClientPublicAttribute = 4,
	--- [Client <-> Server]
	ClientPrivateAttribute = 5,
	--- [Client --> Server]
	ClientPrivateAttributeChanged = 6,
	--- Server --> Client
	ServerAttribute = 7,
	--- [Client <-> Server]
	Clock = 8,

	--- [Client <-> Server]
	PlayerInputs = 101,
	--- [Client <-> Server]
	SnapshotRequest = 102,
	--- Server --> Client
	SnapshotResponse = 103,
	--- [Client <-> Server]
	Snapshots = 104,
	--- [Client <-> Server]
	--- Client --> Server: Host client send latest snapshot to server
	Snapshot = 105,

	--- [Client <-> Server]
	WorldSessionStart = 201,
	--- [Client <-> Server]
	WorldSessionRestart = 202,
	--- [Client <-> Server]
	WorldSessionFinish = 203,
	--- [Client <-> Server]
	WorldSessionPause = 204,
	--- [Client <-> Server]
	WorldSessionUnpause = 205,

	--- [Client <-> Server]
	ForthSessionStart = 301,
	--- [Client <-> Server]
	ForthSessionFinish = 302,
})

local GNetworkMessage_Type = GNetworkMessage.Type
local GNetworkMessage_Type_Heartbeat = GNetworkMessage_Type.Heartbeat

GNetworkMessage.Channel = Enum.immutable({
	Main = 0,
	Gameplay = 1,

	-- Placeholders may be deprecated in the future.

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

--- @type table<dr2c.NetworkMessageType, true>
local unessentialMessageTypes = {}

--- @param messageType dr2c.NetworkMessageType
function GNetworkMessage.markAsUnessential(messageType)
	unessentialMessageTypes[messageType] = true
end

--- @param messageType dr2c.NetworkMessageType
--- @return boolean
function GNetworkMessage.isUnessential(messageType)
	return unessentialMessageTypes[messageType] or false
end

local encodingTable = { 0, nil }

--- @param messageType dr2c.NetworkMessageType
--- @param messageContent? any
--- @return string
function GNetworkMessage.pack(messageType, messageContent)
	encodingTable[1] = tonumber(messageType) or GNetworkMessage_Type_Heartbeat
	encodingTable[2] = messageContent
	return String_bufferEncode(encodingTable)
end

--- @param message string
--- @return dr2c.NetworkMessageType
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

--- @param message? string
--- @return dr2c.NetworkMessageType
--- @return any?
--- @nodiscard
function GNetworkMessage.unpack(message)
	if message then
		local success, messageType, messageContent = pcall(unpackImpl, message)
		if not success then
			error("Message unpack failed: " .. messageType)
		end

		return messageType, messageContent
	else
		return GNetworkMessage_Type_Heartbeat, nil
	end
end

return GNetworkMessage
