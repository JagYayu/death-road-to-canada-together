--[[
-- @module dr2c.client.render.DebugLayer
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("tudov.Enum")

local CDebugLayer = {}

CDebugLayer.Type = Enum.sequence({
	"Circle",
})

return CDebugLayer
