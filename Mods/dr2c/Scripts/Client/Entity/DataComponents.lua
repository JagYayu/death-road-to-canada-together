--[[
-- @module dr2c.Client.Entity.DataComponents
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CComponents = require("dr2c.Client.Entity.Components")

local archetypeConstant = CComponents.registerArchetypeConstant
local archetypeSerializable = CComponents.registerArchetypeSerializable
local archetypeTransient = CComponents.registerArchetypeTransient
local entityTransient = CComponents.registerEntityTransient
local entitySerializable = CComponents.registerEntitySerializable

archetypeConstant("Character", {})

--- @class dr2c.Component.CharacterBody
entityTransient("CharacterBody", {
	color = -1,
	sprite = "",
	spriteIndex = 1,
}, {
	"Character",
})

--- @class dr2c.Component.CharacterHead
entityTransient("CharacterHead", {
	color = -1,
	sprite = "",
	spriteIndex = 1,
}, {
	"Character",
})

--- @class dr2c.Component.Controller
--- @field x number
--- @field y number
entitySerializable("Controller", {
	playerID = 0,
})

--- @class dr2c.Component.DebugVisual
--- @field active boolean
entityTransient("DebugVisual", {
	active = true,
})

--- @class dr2c.Component.DebugVisualRectangle
--- @field height number
--- @field width number
archetypeConstant("DebugVisualRectangle", {
	height = 16,
	width = 16,
})

--- @class dr2c.Component.GameObject
--- @field x number
--- @field y number
entitySerializable("GameObject", {
	room = 0,
	x = 0,
	y = 0,
})

--- @class dr2c.Component.Move
entitySerializable("Move", {
	direction = 0,
	speed = 0,
})
