local GClient = require("dr2c.shared.network.Client")
local GPlatform = require("dr2c.shared.network.Platform")

local CPlatform = {}

function CPlatform.getType()
	return GPlatform.Type.None
end

events:add(N_("CCollectVerifyAttributes"), function(e)
	e.public[#e.public + 1] = {
		attribute = GClient.PublicAttribute.Platform,
		value = GPlatform.Type.None,
	}

	e.public[#e.public + 1] = {
		attribute = GClient.PublicAttribute.PlatformID,
		value = "1516249885",
	}
end, "SendPlatformAndID", "Public")

return CPlatform
