--[[
-- @module dr2c.Client.World.Level
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CWorldSession = require("dr2c.Client.World.Session")
local GWorldLevel = require("dr2c.Shared.World.Level")

local CWorldLevel = GWorldLevel.new(CWorldSession)

return CWorldLevel
