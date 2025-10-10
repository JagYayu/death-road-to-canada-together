local Enum = require("TE.Enum")

--- @class dr2c.GNetworkReason
local GNetworkReason = {}

GNetworkReason.ID = Enum.sequence({
	None = 0,
	NoAuthoritativeClient = 1,
	NoAuthoritativeSnapshot = 2,
})

return GNetworkReason
