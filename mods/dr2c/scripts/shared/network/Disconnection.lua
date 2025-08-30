local Enum = require("tudov.Enum")

--- @alias dr2c.DisconnectionCode dr2c.GDisconnection.Code

--- @class dr2c.GDisconnection
local GDisconnection = {}

GDisconnection.Code = Enum.sequence({
	Unknown = EDisconnectionCode.Unknown,
	ClientClosed = EDisconnectionCode.ClientClosed,
	ServerClosed = EDisconnectionCode.ServerClosed,
	Timeout = 3,
	VerifyFailed = 4,
	Kicked = 5,
	Banned = 6,
})

-- function GDisconnection.getReason()
-- end

return GDisconnection
