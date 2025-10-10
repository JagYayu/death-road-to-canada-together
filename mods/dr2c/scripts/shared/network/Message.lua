--[[
-- @module dr2c.shared.network.Message
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

--- @alias dr2c.NetworkMessageChannel dr2c.GNetworkMessage.Channel

--- @alias dr2c.NetworkMessageType dr2c.GNetworkMessage.Type

--- @class dr2c.GNetworkMessage
local GNetworkMessage = {}

GNetworkMessage.Type = Enum.sequence({
	Unknown = 0,
	--- [Server ==> Client]
	--- Server send all existed client's public attributes to lately verified client.
	Clients = 1,
	--- [Server ==> Client]
	ClientConnect = 2,
	--- [Server ==> Client]
	ClientDisconnect = 3,
	--- [Client <-> Server]
	--- Client ==> Server: A client changed a public attribute, send value to server.
	--- Server ==> Client: Server broadcast a client's modified public attribute to all other clients.
	ClientPublicAttribute = 4,
	--- [Client <-> Server]
	ClientPrivateAttribute = 5,
	--- [Client ==> Server]
	ClientPrivateAttributeChanged = 6,
	--- [Server ==> Client]
	ServerAttribute = 7,
	--- [Client <-> Server]
	Clock = 8,
	--- [Client <-> Server]
	PlayerInputs = 9,
	--- [Client <-> Server]
	SnapshotRequest = 10,
	--- [Server ==> Client]
	SnapshotResponse = 11,
	--- [Client <-> Server]
	Snapshots = 12,
	--- [Client <-> Server]
	--- Client ==> Server: Host client send latest snapshot to server
	Snapshot = 13,
	--- [Client <-> Server]
	WorldSessionStart = 14,
	--- [Client <-> Server]
	WorldSessionRestart = 15,
	--- [Client <-> Server]
	WorldSessionFinish = 16,
	--- [Client <-> Server]
	WorldSessionPause = 17,
	--- [Client <-> Server]
	WorldSessionUnpause = 18,
})

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

--- @param messageType dr2c.NetworkMessageType
--- @param messageContent any?
--- @return string
function GNetworkMessage.pack(messageType, messageContent)
	return String.bufferEncode({
		tonumber(messageType) or GNetworkMessage.Type.Unknown,
		messageContent or nil,
	})
end

--- @param message string
--- @return dr2c.NetworkMessageType
--- @return any?
local function unpackImpl(message)
	local messageTable = String.bufferDecode(message)
	--- @diagnostic disable-next-line: need-check-nil
	local messageValue, messageType = messageTable[1], messageTable[2]

	if Enum.hasValue(GNetworkMessage.Type, messageValue) then
		return messageValue, messageType
	else
		return GNetworkMessage.Type.Unknown, messageType
	end
end

--- @param message string
--- @return dr2c.NetworkMessageType
--- @return any?
function GNetworkMessage.unpack(message)
	local success, messageType, messageContent = pcall(unpackImpl, message)
	if not success then
		if log.canTrace() then
			-- local str = debug.getinfo(1, "n")
			--
			-- print("Message unpack failed", messageType, message)
		end

		messageType = GNetworkMessage.Type.Unknown
		messageContent = nil
	end

	return messageType, messageContent
end

return GNetworkMessage
