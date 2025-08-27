local CComponents = require "dr2c.client.ecs.Components"

local constant = CComponents.registerConstant
local mutable = CComponents.registerMutable
local serializable = CComponents.registerSerializable
local shared = CComponents.registerShared

serializable("GameObject", {
	x = 0,
	y = 0,
})



-- serializable("")
