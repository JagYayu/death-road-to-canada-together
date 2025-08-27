local CECS = require "dr2c.client.ecs.ECS"

local CDebugLayerRenderer = {}

local t = CECS.filter({
	"DebugRender",
})

events:add(N_("RenderCamera"), function(e)
	-- CECS.iterateEntities()
end, "RenderDebugObjects", "DebugObjects")

return CDebugLayerRenderer
