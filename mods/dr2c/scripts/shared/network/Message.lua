local String = require("tudov.String")
local Enum = require("tudov.Enum")

--- @class dr2c.GMessage
local GMessage = {}

--- @enum GMessage.Type
GMessage.Type = Enum.sequence({
	Unknown = 0,
})

--- @param messageType GMessage.Type
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
	return messageTable[1], messageTable[2]
end

--- @param message any
--- @return GMessage.Type
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
