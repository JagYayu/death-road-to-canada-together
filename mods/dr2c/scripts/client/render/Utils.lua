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

local Geometry = require("tudov.Geometry")

local tonumber = tonumber

--- @class dr2c.CRenderUtils
local CRenderUtils = {}

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

CRenderUtils.copyRectangle = Geometry.copyLuaRect

return CRenderUtils
