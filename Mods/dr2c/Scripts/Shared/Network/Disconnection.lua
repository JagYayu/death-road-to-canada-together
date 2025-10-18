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

--- @alias dr2c.DisconnectionCode dr2c.GDisconnection.Code

--- @class dr2c.GDisconnection
local GDisconnection = {}

GDisconnection.Code = Enum.sequence({
	Unknown = EDisconnectionCode.Unknown,
	ClientClosed = EDisconnectionCode.ClientClosed,
	ServerClosed = EDisconnectionCode.ServerClosed,
	Timeout = 3,
	VerifyFailed = 4,
	Kicked = 5,
	Banned = 6,
})

-- function GDisconnection.getReason()
-- end

return GDisconnection
