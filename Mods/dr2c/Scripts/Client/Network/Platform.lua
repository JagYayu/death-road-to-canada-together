--[[
-- @module dr2c.Client.Network.Platform
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local GClient = require("dr2c.Shared.Network.Client")
local GPlatform = require("dr2c.Shared.Network.Platform")

local CNetworkPlatform = {}

function CNetworkPlatform.getType()
	return GPlatform.Type.Standalone
end

TE.events:add(N_("CCollectVerifyAttributes"), function(e)
	e.attributes[#e.attributes + 1] = {
		isPublicOrPrivate = true,
		attribute = GClient.PublicAttribute.Platform,
		value = GPlatform.Type.Standalone,
	}

	e.attributes[#e.attributes + 1] = {
		isPublicOrPrivate = true,
		attribute = GClient.PublicAttribute.PlatformID,
		value = "1516249885",
	}
end, "SendPlatformAndID", "Public")

return CNetworkPlatform
