local String = require("tudov.String")
local Enum = require("tudov.Enum")

--- @alias dr2c.MessageType dr2c.GMessage.Type

--- @class dr2c.GMessage
local GMessage = {}

GMessage.Type = Enum.sequence({
	Unknown = 0,
	-- Server ==> Client
	ClientConnect = 1,
	-- Server ==> Client
	ClientDisconnect = 2,
	-- Client <-> Server
	ClientPublicAttribute = 3,
	-- Client <-> Server
	ClientPrivateAttribute = 4,
	-- Client ==> Server
	ClientPrivateAttributeChanged = 5,
	-- Server ==> Client
	ServerAttribute = 6,
	-- Server <-> Client
	PlayerInputs = 7,
})

GMessage.Channel = Enum.immutable({
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

--- @type table<dr2c.MessageType, true>
local unessentialMessageTypes = {}

--- @param messageType dr2c.MessageType
function GMessage.markAsUnessential(messageType)
	unessentialMessageTypes[messageType] = true
end

--- @param messageType dr2c.MessageType
--- @return boolean
function GMessage.isUnessential(messageType)
	return unessentialMessageTypes[messageType] or false
end

--- @param messageType dr2c.MessageType
--- @param messageContent any?
--- @return string
function GMessage.pack(messageType, messageContent)
	return String.bufferEncode({
		tonumber(messageType) or GMessage.Type.Unknown,
		messageContent or nil,
	})
end

local function unpackImpl(message)
	local messageTable = String.bufferDecode(message)
	--- @diagnostic disable-next-line: need-check-nil
	return Enum.hasValue(GMessage.Type, messageTable[1]) and messageTable[1] or GMessage.Type.Unknown, messageTable[2]
end

--- @param message string
--- @return dr2c.MessageType
--- @return any?
function GMessage.unpack(message)
	local success, messageType, messageContent = pcall(unpackImpl, message)
	if not success then
		print("Message unpack failed", messageType, message)

		messageType = GMessage.Type.Unknown
		messageContent = nil
	end

	return messageType, messageContent
end

return GMessage
