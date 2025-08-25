local String = require("tudov.String")
local Enum = require("tudov.Enum")

--- @class dr2c.GMessage
local GMessage = {}

--- @enum dr2c.MessageType
GMessage.Type = Enum.sequence({
	Unknown = 0,
	-- 允许双端发送
	"ClientPublicAttribute",
	-- 允许双端发送
	"ClientPrivateAttribute",
	-- 仅客户端发送
	"ClientPrivateAttributeChanged",
	-- 仅服务器发送
	"ServerAttribute",
})

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
	return Enum.hasKey(GMessage.Type, messageTable[1]) and messageTable[1] or GMessage.Type.Unknown, messageTable[2]
end

--- @param message any
--- @return dr2c.MessageType
--- @return any?
function GMessage.unpack(message)
	local success, messageType, messageContent = pcall(unpackImpl, message)
	if not success then
		messageType = GMessage.Type.Unknown
		messageContent = nil
	end

	return messageType, messageContent
end

return GMessage
