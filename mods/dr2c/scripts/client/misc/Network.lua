local Enum = require("tudov.Enum")
local GMessage = require("dr2c.shared.network.Message")

--- @class dr2c.CNetwork
local CNetwork = {}

CNetwork.eventClientMessage = events:new(N_("ClientMessage"), {
	"Clients",
}, Enum.eventKeys(GMessage.Type))

return CNetwork
