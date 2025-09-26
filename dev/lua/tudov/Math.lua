--[[
-- @module tudov.Math
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("tudov.Table")

local bit_band = bit.band
local bit_lshift = bit.lshift
local bit_rshift = bit.rshift
local math_atan = math.atan
local math_ceil = math.ceil
local math_deg = math.deg
local math_floor = math.floor
local math_max = math.max
local math_min = math.min

--- Q16.16
--- @class TE.Math
local Math = {}

--- @class TE.Math.Q1616
local Q1616 = {
	shift = 16,
}

local shift = Q1616.shift
local one = bit_lshift(1, shift)

function Q1616.toFixed(x)
	return bit_lshift(x, shift)
end

function Q1616.fromFixed(x)
	return bit_rshift(x, shift)
end

Q1616.pi = Q1616.toFixed(math.pi)
local pi = Q1616.pi

--- @param a number
--- @param b number
--- @return integer value
function Q1616.add(a, b)
	return a + b
end

--- @param a number
--- @param b number
--- @return integer value
function Q1616.sub(a, b)
	return a - b
end

--- @param a number
--- @param b number
--- @return integer value
function Q1616.mul(a, b)
	return bit_rshift(a * b, shift)
end

--- @param a number
--- @param b number
--- @return integer value
function Q1616.div(a, b)
	a = bit_lshift(a, shift) / b
	return a >= 0 and math_floor(a) or math_ceil(a)
end

local seed = math.random(1, 123456789)

--- @param x number
function Q1616.randomseed(x)
	seed = math_floor(x)
end

--- @return integer @Range: 0~9999
function Q1616.random(m, n)
	seed = bit_band((1103515245 * seed + 12345), 0x7fffffff)
	if not m then
		return seed % 1e4
	elseif not n then
		return seed % m
	else
		return m + (seed % (n - m + 1))
	end
end

local sinCache = {}
for deg = 0, 360 do
	local rad = deg * pi / 180
	sinCache[deg] = math_floor(math.sin(rad) * one + 0.5)
end

function Q1616.sin(deg)
	deg = deg % 360
	return sinCache[deg]
end

local Q1616_sin = Q1616.sin

function Q1616.cos(deg)
	return Q1616_sin((deg + 90) % 360)
end

local atanTable = {
	51472,
	30385,
	16054,
	8149,
	4090,
	2045,
	1023,
	512,
	256,
	128,
	64,
	32,
	16,
	8,
	4,
	2,
}
local piDiv2 = Q1616.div(Q1616.pi, 2)

function Q1616.atan2(y, x)
	if x == 0 then
		if y > 0 then
			return piDiv2
		elseif y < 0 then
			return -piDiv2
		else
			return 0
		end
	end

	local angle = 0
	local x1, y1 = x, y

	if x1 < 0 then
		x1 = -x1
		y1 = -y1
		angle = angle + pi
	end

	for i = 1, #atanTable do
		local dx = bit_rshift(x1, (i - 1))
		local dy = bit_rshift(y1, (i - 1))

		if y1 > 0 then
			x1 = x1 + dy
			y1 = y1 - dx
			angle = angle + atanTable[i]
		else
			x1 = x1 - dy
			y1 = y1 + dx
			angle = angle - atanTable[i]
		end
	end

	if angle > pi then
		angle = angle - bit_lshift(pi, 1)
	elseif angle <= -pi then
		angle = angle + bit_lshift(pi, 1)
	end

	return angle
end

Math.Q1616 = Q1616

--- @param l number
--- @param v number
--- @param r number
--- @return number
--- @nodiscard
function Math.clamp(l, v, r)
	if v < l then
		return l
	elseif v > r then
		return r
	else
		return v
	end
end

--- @param a number
--- @param b number
--- @param t number
--- @return number
--- @nodiscard
function Math.lerp(a, b, t)
	return (1 - t) * a + t * b
end

setmetatable(Math, {
	__index = math,
})

return Math
