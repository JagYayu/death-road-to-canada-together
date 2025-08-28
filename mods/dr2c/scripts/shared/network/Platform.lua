local Enum = require("tudov.Enum")

local GPlatform = {}

--- @enum dr2c.PlatformType
GPlatform.Type = Enum.immutable({
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
