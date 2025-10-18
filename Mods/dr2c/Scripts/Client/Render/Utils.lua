--[[
-- @module dr2c.Client.Render.Utils
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Geometry = require("TE.Geometry")

local tonumber = tonumber

--- @class dr2c.CRenderUtils
local CRenderUtils = {}

--- @param border dr2c.UI.DrawBorder | table
--- @return dr2c.UI.DrawBorder
function CRenderUtils.copyDrawBorder(border)
	return {
		x = tonumber(border.x) or 0,
		y = tonumber(border.y) or 0,
		width = tonumber(border.width) or 0,
		height = tonumber(border.height) or 0,
		size = tonumber(border.size) or 1,
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
