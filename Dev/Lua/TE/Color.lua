--[[
-- @module TE.Color
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class TE.Color
local Color = {}

--- @class Color : integer

local bit_band = bit.band
local bit_bor = bit.bor
local bit_lshift = bit.lshift
local bit_rshift = bit.rshift

--- @param r UInt8
--- @param g UInt8
--- @param b UInt8
function Color.rgb(r, g, b)
	return bit_bor(
		0x000000FF,
		bit_lshift(bit_band(b, 255), 8),
		bit_lshift(bit_band(g, 255), 16),
		bit_lshift(bit_band(r, 255), 24)
	)
end

local Color_rgb = Color.rgb

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

local Color_rgba = Color.rgba

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

--- @param c Color
--- @return integer r
--- @nodiscard
function Color.getR(c)
	return bit_band(bit_rshift(c, 24), 255)
end

--- @param c Color
--- @return integer g
--- @nodiscard
function Color.getG(c)
	return bit_band(bit_rshift(c, 16), 255)
end

--- @param c Color
--- @return integer b
--- @nodiscard
function Color.getB(c)
	return bit_band(bit_rshift(c, 8), 255)
end

--- @param c Color
--- @return integer a
--- @nodiscard
function Color.getA(c)
	return bit_band(c, 255)
end

--- @param c Color
--- @return integer r
--- @return integer g
--- @return integer b
--- @nodiscard
function Color.getRGB(c)
	return bit_band(bit_rshift(c, 24), 255), bit_band(bit_rshift(c, 16), 255), bit_band(bit_rshift(c, 8), 255)
end

--- @param c Color
--- @return integer r
--- @return integer g
--- @return integer b
--- @return integer a
--- @nodiscard
function Color.getRGBA(c)
	return bit_band(bit_rshift(c, 24), 255),
		bit_band(bit_rshift(c, 16), 255),
		bit_band(bit_rshift(c, 8), 255),
		bit_band(c, 255)
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

local Color_getRGBA = Color.getRGBA

--- @param c Color
--- @param f number
--- @return number c
--- @nodiscard
function Color.darken(c, f)
	local r, g, b, a = Color_getRGBA(c)
	return Color_rgba(r * f, g * f, b * f, a)
end

function Color.fade(c, f)
	-- TODO
end

--- @param v UInt8?
--- @return number c
--- @nodiscard
function Color.gray(v)
	v = v or 127
	return Color_rgb(v, v, v)
end

--- @param f number? @default=0.5
--- @return number c
--- @nodiscard
function Color.grayF(f)
	f = (f or 0.5) * 255
	return Color_rgb(f, f, f)
end

local TransparentWhite = Color_rgba(255, 255, 255, 0)

--- Slightly faster than `Color.rgba(255, 255, 255, a)`
--- @param a UInt8
--- @return integer c
--- @nodiscard
function Color.opacity(a)
	return bit_bor(TransparentWhite, a)
end

--- @param a number
--- @return integer c
--- @nodiscard
function Color.opacityF(a)
	return bit_bor(TransparentWhite, bit_band(a * 255))
end

Color.Black = Color.rgb(0, 0, 0)
Color.Blue = Color.rgb(0, 0, 255)
Color.Cyan = Color.rgb(0, 255, 255)
Color.Green = Color.rgb(0, 255, 0)
Color.Magenta = Color.rgb(255, 0, 255)
Color.Red = Color.rgb(255, 0, 0)
Color.Transparent = Color_rgba(0, 0, 0, 0)
Color.TransparentBlack = Color_rgba(0, 0, 0, 0)
Color.TransparentBlue = Color_rgba(0, 0, 255, 0)
Color.TransparentCyan = Color_rgba(0, 255, 255, 0)
Color.TransparentGreen = Color_rgba(0, 255, 0, 0)
Color.TransparentMagenta = Color_rgba(255, 0, 255, 0)
Color.TransparentRed = Color_rgba(255, 0, 0, 0)
Color.TransparentWhite = TransparentWhite
Color.TransparentYellow = Color_rgba(255, 255, 0, 0)
Color.White = Color.rgb(255, 255, 255)
Color.Yellow = Color.rgb(255, 255, 0)

return Color
