--[[
-- @module dr2c.Shared.network.Room
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")

--- @alias dr2c.NetworkRoomAttribute dr2c.GNetworkRoom.Attribute

--- @class dr2c.NetworkRoomID : integer

--- @class dr2c.GNetworkRoom
local GNetworkRoom = {}

GNetworkRoom.Attribute = Enum.sequence({
	ID = 0,
	Name = 1,
})

return GNetworkRoom
