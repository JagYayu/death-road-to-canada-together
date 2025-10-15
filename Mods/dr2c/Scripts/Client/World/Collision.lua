--[[
-- @module dr2c.Client.World.Collision
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")

--- @class dr2c.CWorldCollision
local CWorldCollision = {}

--- @alias dr2c.CWorldCollisionFlag dr2c.CWorldCollision.Flag

--- @enum dr2c.CWorldCollision.Flag
CWorldCollision.Flag = { -- TODO use `Enum.flags` in future for extendability.
	Object = bit.lshift(1, 0),
}

return CWorldCollision
