--[[
-- @module dr2c.shared.network.Platform
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("tudov.Enum")

--- @alias dr2c.PlatformType dr2c.GNetworkPlatform.Type

--- @class dr2c.GNetworkPlatform
local GNetworkPlatform = {}

GNetworkPlatform.Type = Enum.sequence({
	Standalone = 0,
	ItchIO = 1,
	Steam = 2,
	GOG = 3,
	Epic = 4,
})

--- @return dr2c.PlatformType
function GNetworkPlatform.getPlatformType()
	return GNetworkPlatform.Type.Standalone
end

return GNetworkPlatform
