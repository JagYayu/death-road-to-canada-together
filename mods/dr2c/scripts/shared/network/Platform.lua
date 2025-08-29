local Enum = require("tudov.Enum")

local GPlatform = {}

GPlatform.Type = Enum.sequence({
	Standalone = 0,
	ItchIO = 1,
	Steam = 2,
	GOG = 3,
	Epic = 4,
})

--- @return Enum.Defaults
function GPlatform.getPlatformType()
	return GPlatform.Type.Standalone
end

return GPlatform
