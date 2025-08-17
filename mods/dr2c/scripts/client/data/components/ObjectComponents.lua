local CComponents = require "dr2c.client.ecs.Components"

local serializable = CComponents.registerSerializable

serializable("GameObject", {
	x = 0,
	y = 0,
})
