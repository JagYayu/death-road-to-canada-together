--[[
-- @module dr2c.Client.Render.DebugLayerRenderer
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CEntityECS = require("dr2c.Client.Entity.ECS")
local CEntityECSSchema = require("dr2c.Client.Entity.ECSSchema")

local CDebugLayerRenderer = {}

local filterTest = CEntityECS.filter({
	"DebugVisual",
	"DebugVisualRectangle",
	"GameObject",
})

local drawRectArgs

--- @param e dr2c.E.CRender
TE.events:add(N_("CRenderCamera"), function(e)
	local renderer = e.renderer

	local getComponent = CEntityECS.getComponent

	drawRectArgs = drawRectArgs or DrawRectArgs()
	local drawRectArgsDest = drawRectArgs.destination

	for index, id, typeID in CEntityECS.iterateEntities(filterTest) do
		local gameObject = getComponent(id, "GameObject") --- @cast gameObject dr2c.Component.GameObject
		local debugVisualRectangle = getComponent(id, "DebugVisualRectangle") --- @cast debugVisualRectangle dr2c.Component.DebugVisualRectangle

		local width = debugVisualRectangle.width
		local height = debugVisualRectangle.height

		drawRectArgsDest.x = gameObject.x - width * 0.5
		drawRectArgsDest.y = gameObject.y - height * 0.5
		drawRectArgsDest.w = width
		drawRectArgsDest.h = height

		renderer:drawRect(drawRectArgs)
	end
end, "RenderDebugObjects", "DebugObjects")

return CDebugLayerRenderer
