local GMessage = require("dr2c.shared.network.Message")
local GServer = require("dr2c.shared.network.Server")

--- @class dr2c.CServer
local CServer = {}

local serverAttributes = {}

function CServer.getAttribute(attribute)
	return serverAttributes[attribute]
end

events:add(N_("CMessage"), function(e)
	local content = e.content
	if type(content) ~= "table" then
		return
	end

	local attribute = content.attribute
	local value = content.value
	if type(attribute) ~= "string" then
		return
	end

	if not GServer.validateAttribute(attribute, value) then
		return
	end

	serverAttributes[attribute] = value
end, "ClientReceiveServerAttribute", "Receive", GMessage.Type.ServerAttribute)

return CServer
