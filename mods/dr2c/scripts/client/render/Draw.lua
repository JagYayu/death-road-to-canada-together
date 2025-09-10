--[[
-- @module dr2c.client.render.Draw
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CModule = require("dr2c.client.Module")

--- @class dr2c.CRenderDraw
local CRenderDraw = {}

local draw9GridArgs = DrawRectArgs()

local CModule_getRenderer = CModule.getRenderer

--- @return dr2c.DrawGridDefinition
function CRenderDraw.newGridDef(texture)
	--- @class dr2c.DrawGridDefinition
	return {
		texture = texture,
	}
end

--- @param gridDef dr2c.DrawGridDefinition
--- @param x number
--- @param y number
--- @param w number
--- @param h number
--- @param cs number @corner size
--- @param col number @color
function CRenderDraw.draw9Grid(gridDef, x, y, w, h, cs, col)
	local renderer = CModule_getRenderer()
	if not renderer then
		return
	end

	draw9GridArgs.texture = gridDef[1]
	local dst = draw9GridArgs.destination
	local src = draw9GridArgs.source

	draw9GridArgs.color = col

	-- 绘制顺序：左上、上、右上、左、右、左下、下、右下

	src.x = 0
	src.y = 0
	src.w = 4
	src.h = 4
	dst.x = x
	dst.y = y
	dst.w = cs
	dst.h = cs
	renderer:drawRect(draw9GridArgs)

	src.x = 4
	src.w = 1
	dst.x = x + cs
	dst.w = w
	renderer:drawRect(draw9GridArgs)

	src.x = 5
	src.w = 4
	dst.x = x + cs + w
	dst.w = cs
	renderer:drawRect(draw9GridArgs)

	src.x = 0
	src.y = 4
	src.h = 1
	dst.x = x
	dst.y = y + cs
	dst.w = cs
	dst.h = h
	renderer:drawRect(draw9GridArgs)

	src.x = 5
	dst.x = x + cs + w
	renderer:drawRect(draw9GridArgs)

	src.x = 0
	src.y = 5
	src.w = 4
	src.h = 4
	dst.x = x
	dst.y = y + cs + h
	dst.h = cs
	renderer:drawRect(draw9GridArgs)

	src.x = 4
	src.w = 1
	dst.x = x + cs
	dst.w = w
	renderer:drawRect(draw9GridArgs)

	src.x = 5
	src.w = 4
	dst.x = x + cs + w
	dst.w = cs
	renderer:drawRect(draw9GridArgs)
end

return CRenderDraw
