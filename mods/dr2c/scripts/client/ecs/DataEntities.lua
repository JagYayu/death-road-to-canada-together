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
