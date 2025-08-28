local CComponents = require("dr2c.client.ecs.Components")

local archetypeConstant = CComponents.registerArchetypeConstant
local archetypeSerializable = CComponents.registerArchetypeSerializable
local archetypeTransient = CComponents.registerArchetypeTransient
local entityTransient = CComponents.registerEntityTransient
local entitySerializable = CComponents.registerEntitySerializable

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
--- @field width number
--- @field height number
archetypeConstant("DebugVisualRectangle", {
	width = 16,
	height = 16,
})

--- @class dr2c.Component.GameObject
--- @field x number
--- @field y number
entitySerializable("GameObject", {
	x = 0,
	y = 0,
})
