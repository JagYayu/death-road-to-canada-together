--[[
-- @module dr2c.client.render.Utils
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class dr2c.CRenderUtils
local CRenderUtils = {}

local tonumber = tonumber

--- @param border dr2c.UI.DrawBorder | table
function CRenderUtils.copyDrawBorder(border)
	return {
		x = tonumber(border.x),
		y = tonumber(border.y),
		width = tonumber(border.width),
		height = tonumber(border.height),
		size = tonumber(border.size),
		texture = border.texture,
		textureX = tonumber(border.textureX),
		textureY = tonumber(border.textureY),
		textureWidth = tonumber(border.textureWidth),
		textureHeight = tonumber(border.textureHeight),
		textureSize = tonumber(border.textureSize),
		color = tonumber(border.color),
	}
end

--- @param rect Rectangle | table
--- @param x number?
--- @param y number?
--- @param w number?
--- @param h number?
--- @return Rectangle
function CRenderUtils.copyRectangle(rect, x, y, w, h)
	return {
		tonumber(rect[1]) or x or 0,
		tonumber(rect[2]) or y or 0,
		tonumber(rect[3]) or w or 0,
		tonumber(rect[4]) or h or 0,
	}
end

return CRenderUtils
