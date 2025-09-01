local Enum = require("tudov.Enum")

--- @alias dr2c.PlatformType dr2c.GPlatform.Type

local GPlatform = {}

GPlatform.Type = Enum.sequence({
	Standalone = 0,
	ItchIO = 1,
	Steam = 2,
	GOG = 3,
	Epic = 4,
})

--- @return dr2c.PlatformType
function GPlatform.getPlatformType()
	return GPlatform.Type.Standalone
end

return GPlatform
