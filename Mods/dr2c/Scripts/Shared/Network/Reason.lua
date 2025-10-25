local Enum = require("TE.Enum")

--- @class dr2c.GNetworkReason
local GNetworkReason = {}

--- @alias dr2c.NetworkReasonID dr2c.GNetworkReason.ID

GNetworkReason.ID = Enum.sequence({
	None = 0,
	NoResponseFromServer = 1,
	NoAuthoritativeClient = 2,
	NoAuthoritativeSnapshot = 3,
})

return GNetworkReason
