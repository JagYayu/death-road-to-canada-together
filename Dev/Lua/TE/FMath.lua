--[[
-- @module TE.FMath
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

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
--- 定点数学库，假定
--- @class TE.FMath
local FMath = {}

--- @class TE.FInt : integer

--- @type TE.FInt
FMath.intMax = 2 ^ 53
local intMax = FMath.intMax

--- @type TE.FInt
FMath.intMin = -2 ^ 53
local intMin = FMath.intMin

--- @type TE.FInt
FMath.shift = 16
local shift = FMath.shift

--- @type TE.FInt
FMath.one = bit_lshift(1, shift)
local one = FMath.one

--- @param x integer
--- @return boolean
function FMath.isSafe(x)
	return x >= intMin and x <= intMax
end

--- @param x integer
--- @return boolean
function FMath.isUnsafe(x)
	return not (x >= intMin and x <= intMax)
end

--- @param x number
--- @return TE.FInt
function FMath.toFixed(x)
	return bit_lshift(x, shift)
end

--- @param x TE.FInt
--- @return integer
--- @nodiscard
function FMath.fromFixed(x)
	return bit_rshift(x, shift)
end

FMath.pi = FMath.toFixed(math.pi)
local pi = FMath.pi

--- @param a number
--- @param b number
--- @return TE.FInt value
--- @nodiscard
function FMath.add(a, b)
	return a + b
end

--- @param a number
--- @param b number
--- @return TE.FInt value
--- @nodiscard
function FMath.sub(a, b)
	return a - b
end

--- @param a number
--- @param b number
--- @return TE.FInt value
--- @nodiscard
function FMath.mul(a, b)
	return bit_rshift(a * b, shift)
end

--- @param a number
--- @param b number
--- @return TE.FInt value
--- @nodiscard
function FMath.div(a, b)
	a = bit_lshift(a, shift) / b
	return a >= 0 and math_floor(a) or math_ceil(a)
end

local seed = 123456789

--- @param x integer
function FMath.randomseed(x)
	seed = math_floor(x)
end

--- @return TE.FInt @Range: 0~9999
function FMath.random(m, n)
	seed = bit_band((1103515245 * seed + 12345), 0x7fffffff)
	if not m then
		return seed % 1e4
	elseif not n then
		return seed % m
	else
		return m + (seed % (n - m + 1))
	end
end

local sinTable = {}
for deg = 0, 360 do
	local rad = deg * pi / 180
	sinTable[deg] = math_floor(math.sin(rad) * one + 0.5)
end

--- @param deg TE.FInt
--- @return TE.FInt
--- @nodiscard
function FMath.sin(deg)
	deg = deg % 360
	return sinTable[deg]
end

local Q1616_sin = FMath.sin

--- @param deg TE.FInt
--- @return TE.FInt
--- @nodiscard
function FMath.cos(deg)
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
local piDiv2 = FMath.div(FMath.pi, 2)

--- 返回 `y/x` 的反正切值（用弧度表示）
--- @param y TE.FInt
--- @param x TE.FInt
--- @return TE.FInt
--- @nodiscard
function FMath.atan2(y, x)
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

return FMath
