--[[
-- @module dr2c.Client.World.FastForward
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CWorldRollback = require("dr2c.Client.World.Rollback")
local CWorldTick = require("dr2c.Client.World.Tick")

local CWorldRollback_isApplied = CWorldRollback.isActivated
local CWorldTick_isOnLatestTick = CWorldTick.isOnLatestTick

--- @see dr2c.CWorldFastForwardRenderer
--- @class dr2c.CWorldFastForward
local CWorldFastForward = {}

--- @return boolean
function CWorldFastForward.isActive()
	return CWorldRollback_isApplied() and not CWorldTick_isOnLatestTick()
end

return CWorldFastForward
