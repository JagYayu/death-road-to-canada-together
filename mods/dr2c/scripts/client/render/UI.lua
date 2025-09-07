--[[
-- @module dr2c.client.render.UI
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local eventRenderUI = events:new(N_("CRenderUI"), {
	"Begin",
	"Test",
	"End",
})

--- @type RenderTarget
local renderTarget

local imageID = fonts:getID("gfx/cars/cars_unique_110x96.png")

--- @param e dr2c.E.ClientRender
events:add(N_("CRender"), function(e)
	local renderer = e.renderer
	if not renderTarget then
		renderTarget = renderer:newRenderTarget(1920, 1080)
	else
		renderTarget:resizeToFit()
	end

	renderer:beginTarget(renderTarget)
	renderer:clear()

	local scale = 1

	renderTarget:setCameraTargetScale(scale, scale)

	events:invoke(eventRenderUI, e)

	local width, height = e.window:getSize()
	renderer:draw({
		renderTarget = renderer:endTarget(),
		destination = {
			0,
			0,
			width,
			height,
		},
	})
end, N_("RenderUI"), "UI")
