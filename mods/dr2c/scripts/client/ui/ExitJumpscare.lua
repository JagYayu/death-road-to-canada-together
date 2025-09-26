--[[
-- @module dr2c.client.ui.ExitJumpscare
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Color = require("tudov.Color")

local CRenderSprites = require("dr2c.client.render.Sprites")

--- @class dr2c.CExitJumpscare
local CExitJumpscare = {}

local state

state = persist("state", function()
	return state
end)

function CExitJumpscare.trigger()
	if state then
		return
	end

	CRenderSprites.registerSprites(sprites)

	state = {
		time = Time.getSystemTime(),
		texture = "",
		sound = "",
	}
end

--- @param e dr2c.E.CRenderUI
TE.events:add(N_("CRenderUI"), function(e)
	if not state then
		return
	end

	local time = Time.getSystemTime() - state.time

	local drawRectArgs = DrawRectArgs()
	drawRectArgs.destination.x = 0
	drawRectArgs.destination.y = 0
	drawRectArgs.destination.w, drawRectArgs.destination.h = e.window:getSize()
	drawRectArgs.color = Color.rgba(0, 0, 0, math.min(time * 192, 255))

	e.renderer:drawRect(drawRectArgs)

	if time > 1.75 then
		-- TE.engine:quit()
	elseif time > 1 then
		if state.sound then
			-- TODO play scary sound :)

			state.sound = nil
		end

		local size = (time - 1)
	end
end, "UpdateAndRenderExitJumpscare", "Exit")

return CExitJumpscare
