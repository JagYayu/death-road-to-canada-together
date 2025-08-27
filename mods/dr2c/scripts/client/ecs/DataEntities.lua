local CEntities = require("dr2c.client.ecs.Entities")

local entity = CEntities.registerEntity

entity("Character", {
	GameObject = {},
})

entity("Zombie", {
	GameObject = {},
})
