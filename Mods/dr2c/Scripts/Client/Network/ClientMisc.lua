--[[
-- @module dr2c.Client.Network.ClientMisc
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local EnumFlag = require("TE.EnumFlag")

local CNetworkClient = require("dr2c.Client.Network.Client")
local GNetworkClient = require("dr2c.Shared.Network.Client")

--- @class dr2c.CNetworkClientMisc
local CNetworkClientMisc = {}

function CNetworkClientMisc.hasPermissionAuthority()
	local permissions = CNetworkClient.getPublicAttribute(GNetworkClient.PublicAttribute.Permissions)
	return permissions and EnumFlag.hasAny(permissions, GNetworkClient.Permission.Authority) or false
end

return CNetworkClientMisc
