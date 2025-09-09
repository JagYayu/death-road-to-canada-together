--[[
-- @module tudov.Color
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class Color
local Color = {}

local bit_band = bit.band
local bit_bor = bit.bor
local bit_lshift = bit.lshift
local bit_rshift = bit.rshift

--- @param r UInt8
--- @param g UInt8
--- @param b UInt8
function Color.rgb(r, g, b)
	return bit_bor(
		0x000000FF, -- bit_lshift(bit_band(255, 255), 0)
		bit_lshift(bit_band(b, 255), 8),
		bit_lshift(bit_band(g, 255), 16),
		bit_lshift(bit_band(r, 255), 24)
	)
end

--- @param r UInt8
--- @param g UInt8
--- @param b UInt8
--- @param a UInt8
function Color.rgba(r, g, b, a)
	return bit_bor(
		bit_band(a, 255),
		bit_lshift(bit_band(b, 255), 8),
		bit_lshift(bit_band(g, 255), 16),
		bit_lshift(bit_band(r, 255), 24)
	)
end

function Color.rgbF(r, g, b)
	error("not implement yet")
end

function Color.rgbaF(r, g, b, a)
	error("not implement yet")
end

function Color.hsv()
	error("not implement yet")
end

function Color.hsva()
	error("not implement yet")
end

function Color.getR(c)
	error("not implement yet")
end

function Color.getG(c)
	error("not implement yet")
end

function Color.getB(c)
	error("not implement yet")
end

function Color.getA(c)
	error("not implement yet")
end

function Color.getH(c)
	error("not implement yet")
end

function Color.getS(c)
	error("not implement yet")
end

function Color.getV(c)
	error("not implement yet")
end

function Color.setR(c)
	error("not implement yet")
end

function Color.setG(c)
	error("not implement yet")
end

function Color.setB(c)
	error("not implement yet")
end

function Color.setA(c)
	error("not implement yet")
end

function Color.setH(c)
	error("not implement yet")
end

function Color.setS(c)
	error("not implement yet")
end

function Color.setV(c)
	error("not implement yet")
end

local TransparentWhite = Color.rgba(255, 255, 255, 0)

--- Slightly faster than `Color.rgba(255, 255, 255, a)`
--- @param a UInt8
--- @return integer
function Color.opacity(a)
	return bit_bor(TransparentWhite, a)
end

Color.Black = Color.rgb(0, 0, 0)
Color.Blue = Color.rgb(0, 0, 255)
Color.Cyan = Color.rgb(0, 255, 255)
Color.Green = Color.rgb(0, 255, 0)
Color.Magenta = Color.rgb(255, 0, 255)
Color.Red = Color.rgb(255, 0, 0)
Color.Transparent = Color.rgba(0, 0, 0, 0)
Color.TransparentBlack = Color.rgba(0, 0, 0, 0)
Color.TransparentBlue = Color.rgba(0, 0, 255, 0)
Color.TransparentCyan = Color.rgba(0, 255, 255, 0)
Color.TransparentGreen = Color.rgba(0, 255, 0, 0)
Color.TransparentMagenta = Color.rgba(255, 0, 255, 0)
Color.TransparentRed = Color.rgba(255, 0, 0, 0)
Color.TransparentWhite = TransparentWhite
Color.TransparentYellow = Color.rgba(255, 255, 0, 0)
Color.White = Color.rgb(255, 255, 255)
Color.Yellow = Color.rgb(255, 255, 0)

return Color
