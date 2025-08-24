local Enum = require("tudov.Enum")

--- @class dr2c.CPlayers
local CPlayers = {}

CPlayers.Attributes = Enum.sequence({
	ID = 0,
	ClientID = 1,
})

function CPlayers.getClientID(playerID)
	--
end

function CPlayers.isLocalPlayer(playerID)
	--
end

return CPlayers
