--[[
-- @module dr2c.Client.network.Server
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local GMessage = require("dr2c.Shared.Network.Message")
local GServer = require("dr2c.Shared.Network.Server")

--- @class dr2c.CServer
local CServer = {}

local serverAttributes = {}

--- @param attribute dr2c.ServerAttribute
--- @return any?
function CServer.getAttribute(attribute)
	return serverAttributes[attribute]
end

TE.events:add(N_("CMessage"), function(e)
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
