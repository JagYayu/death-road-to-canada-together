local Enum = require("tudov.Enum")

local GDisconnection = {}

GDisconnection.Code = Enum.sequence({
	Unknown = EDisconnectionCode.Unknown,
	ClientClosed = EDisconnectionCode.ClientClosed,
	ServerClosed = EDisconnectionCode.ServerClosed,
	"Timeout",
	"VerifyFailed",
	"Kicked",
	"Banned",
})

-- function GDisconnection.getReason()
-- end

return GDisconnection
