--[[
-- @module dr2c.client.network.Platform
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local GClient = require("dr2c.shared.network.Client")
local GPlatform = require("dr2c.shared.network.Platform")

local CPlatform = {}

function CPlatform.getType()
	return GPlatform.Type.Standalone
end

events:add(N_("CCollectVerifyAttributes"), function(e)
	e.attributes[#e.attributes + 1] = {
		public = true,
		attribute = GClient.PublicAttribute.Platform,
		value = GPlatform.Type.Standalone,
	}

	e.attributes[#e.attributes + 1] = {
		public = true,
		attribute = GClient.PublicAttribute.PlatformID,
		value = "1516249885",
	}
end, "SendPlatformAndID", "Public")

return CPlatform
