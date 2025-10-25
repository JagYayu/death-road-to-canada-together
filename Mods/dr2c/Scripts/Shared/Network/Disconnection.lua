--[[
-- @module dr2c.Shared.Network.Disconnection
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")

--- @class dr2c.GDisconnection
local GDisconnection = {}

--- @alias dr2c.DisconnectionCode dr2c.GDisconnection.Code

GDisconnection.Code = Enum.sequence({
	Unknown = EDisconnectionCode.Unknown,
	ClientClosed = EDisconnectionCode.ClientClosed,
	ServerClosed = EDisconnectionCode.ServerClosed,
	Timeout = 3,
	VerifyFailed = 4,
	Kicked = 5,
	Banned = 6,
})

local disconnectionCode2ReasonID = {}

--- @param disconnectionCode dr2c.DisconnectionCode
--- @param reasonID dr2c.NetworkReasonID
function GDisconnection.setReason(disconnectionCode, reasonID)
	disconnectionCode2ReasonID[disconnectionCode] = reasonID
end

--- @param disconnectionCode dr2c.DisconnectionCode
--- @return dr2c.NetworkReasonID
function GDisconnection.getReason(disconnectionCode)
	return disconnectionCode2ReasonID[disconnectionCode]
end

return GDisconnection
