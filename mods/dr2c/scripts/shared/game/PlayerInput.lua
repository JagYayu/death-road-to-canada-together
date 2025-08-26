local Enum = require("tudov.Enum")

--- @class dr2c.PlayerInput : integer
local GPlayerInput = {}

GPlayerInput.ID = Enum.sequence({
	"Move",
})

return GPlayerInput
