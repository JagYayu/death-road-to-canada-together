--[[
-- @module dr2c.client.render.DebugLayerRenderer
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CECS = require("dr2c.client.ecs.ECS")
local CECSSchema = require("dr2c.client.ecs.ECSSchema")

local CDebugLayerRenderer = {}

local filterTest = CECS.filter({
	"DebugVisual",
	"DebugVisualRectangle",
	"GameObject",
})

--- @param e dr2c.E.CRender
TE.events:add(N_("CRenderCamera"), function(e)
	local renderer = e.renderer

	local CECS_getComponent = CECS.getComponent

	local drawArgs = {
		destination = { 0, 0, 100, 100 },
	}
	local destination = drawArgs.destination

	for index, id, typeID in CECS.iterateEntities(filterTest) do
		local gameObject = CECS_getComponent(id, "GameObject") --- @cast gameObject dr2c.Component.GameObject
		local debugVisualRectangle = CECS_getComponent(id, "DebugVisualRectangle") --- @cast debugVisualRectangle dr2c.Component.DebugVisualRectangle

		local width = debugVisualRectangle.width
		local height = debugVisualRectangle.height

		destination[1] = gameObject.x - width * 0.5
		destination[2] = gameObject.y - height * 0.5
		destination[3] = width
		destination[4] = height

		-- renderer:drawRect(drawArgs)
	end
end, "RenderDebugObjects", "DebugObjects")

return CDebugLayerRenderer
