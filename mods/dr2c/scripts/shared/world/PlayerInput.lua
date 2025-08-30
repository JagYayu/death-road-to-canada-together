local Enum = require("tudov.Enum")

--- @alias dr2c.PlayerInputID dr2c.GPlayerInput.ID
--- @alias dr2c.PlayerInputType dr2c.GPlayerInput.Type

--- @class dr2c.PlayerInput : integer
local GPlayerInput = {}

GPlayerInput.InputQueueMaxSize = 8

GPlayerInput.ID = Enum.sequence({
	Move = 1,
})

GPlayerInput.Type = Enum.immutable({
	Discrete = 0,
	--- Serve as a predictive value for the client.
	Continuous = 1,
	Intent = 2,
})

local playerInputID2Type = {
	[GPlayerInput.ID.Move] = GPlayerInput.Type.Continuous,
}

--- @param playerInputID dr2c.PlayerInputID
--- @return dr2c.PlayerInputType
function GPlayerInput.getType(playerInputID)
	return playerInputID2Type[playerInputID] or GPlayerInput.Type.Discrete
end

return GPlayerInput
