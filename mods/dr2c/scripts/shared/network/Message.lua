local String = require("tudov.String")
local Enum = require("tudov.Enum")

--- @class dr2c.GMessage
local GMessage = {}

GMessage.Type = Enum.sequence({
	Unknown = 0,
	-- Server ==> Client
	"ClientConnect",
	-- Server ==> Client
	"ClientDisconnect",
	-- Client <-> Server
	"ClientPublicAttribute",
	-- Client <-> Server
	"ClientPrivateAttribute",
	-- Client ==> Server
	"ClientPrivateAttributeChanged",
	-- Server ==> Client
	"ServerAttribute",
})

GMessage.Channel = Enum.immutable({
	Main = 0,
	_1 = 1,
	_2 = 2,
	_3 = 3,
	_4 = 4,
	_5 = 5,
	_6 = 6,
	_7 = 7,
})

--- @type table<dr2c.GMessage.Type, true>
local unessentialMessageTypes = {}

--- @param messageType dr2c.GMessage.Type
function GMessage.markAsUnessential(messageType)
	unessentialMessageTypes[messageType] = true
end

--- @param messageType dr2c.GMessage.Type
--- @return boolean
function GMessage.isUnessential(messageType)
	return unessentialMessageTypes[messageType] or false
end

--- @param messageType dr2c.GMessage.Type
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
--- @return dr2c.GMessage.Type
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
