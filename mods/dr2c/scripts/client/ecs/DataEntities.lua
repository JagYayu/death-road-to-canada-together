--[[
-- @module dr2c.client.ecs.DataEntities
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CEntities = require("dr2c.client.ecs.Entities")

local entity = CEntities.registerEntity

entity("Character", {
	Controller = {},
	DebugVisual = {},
	DebugVisualRectangle = {},
	GameObject = {},
})

entity("Zombie", {
	DebugVisual = {},
	DebugVisualRectangle = {},
	GameObject = {},
})
