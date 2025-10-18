--[[
-- @module TE.Math
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local math_abs = math.abs
local math_atan2 = math.atan2
local math_cos = math.cos
local math_min = math.min
local math_sin = math.sin
local math_sqrt = math.sqrt
local math_pi = math.pi

--- Q16.16
--- @class TE.FMath
local Math = {}

Math.intMax = 2 ^ 53

Math.intMin = -2 ^ 53

Math.tau = 2 * math_pi

Math.nan = 0 / 0

local tau = Math.tau

local deg2rad = math_pi / 180
local rad2deg = 180 / math_pi

--- @param rad number
--- @return number
function Math.rad2deg(rad)
	return rad * rad2deg
end

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

--- @param deg number
--- @return number
function Math.deg2rad(deg)
	return deg * deg2rad
end

--- @param x number
--- @param y number
--- @return number
function Math.dist(x, y)
	return math_sqrt(x * x + y * y)
end

--- @param a number
--- @param b number
--- @param t number
--- @return number
--- @nodiscard
function Math.lerp(a, b, t)
	return (1 - t) * a + t * b
end

--- @param x number
--- @param y number
--- @return number
function Math.sqDist(x, y)
	return x * x + y * y
end

--- @param ang1 number
--- @param ang2 number
--- @return number @[0, π]
function Math.angDiff(ang1, ang2)
	local norm1 = ang1 % tau
	local norm2 = ang2 % tau

	local diff1 = math_abs(norm1 - norm2)
	local diff2 = tau - diff1

	return math_min(diff1, diff2)
end

--#region Vector2D

--- Get vector square length.
Math.vecSqLen = Math.sqDist

--- Get vector length.
Math.vecSqLen = Math.dist

--- @param x1 number
--- @param y1 number
--- @param x2 number
--- @param y2 number
--- @return number
--- @return number
function Math.vecAdd(x1, y1, x2, y2)
	return x1 + x2, y1 + y2
end

--- Get a vector's angle (radius).
--- @param x number
--- @param y number
--- @return number
function Math.vecAng(x, y)
	return math_atan2(y, x)
end

--- Get angle (radius) between 2 vectors.
--- @param x1 number
--- @param y1 number
--- @param x2 number
--- @param y2 number
--- @return number radius
function Math.vecAngBet(x1, y1, x2, y2)
	if (x1 == 0 and y1 == 0) or (x2 == 0 and y2 == 0) then
		return Math.nan
	end

	local cross = x1 * y2 - y1 * x2
	local dot = x1 * x2 + y1 * y2
	return math_atan2(cross, dot)
end

--- Similar to `Math.vecAngBet`, but result angle is between a vector and a direction.
--- @param x number
--- @param y number
--- @param ang number
--- @return number
function Math.vecAngBetDir(x, y, ang)
	if x == 0 and y == 0 then
		return Math.nan
	end

	local y2 = math_sin(ang)
	local x2 = math_cos(ang)
	local cross = x * y2 - y * x2
	local dot = x * x2 + y * y2
	return math_atan2(cross, dot)
end

--- @param x1 number
--- @param y1 number
--- @param x2 number
--- @param y2 number
--- @return number
function Math.vecCross(x1, y1, x2, y2)
	return x1 * y2 - y1 * x2
end

--- @param x1 number
--- @param y1 number
--- @param x2 number
--- @param y2 number
--- @return number
function Math.vecDist(x1, y1, x2, y2)
	local dx, dy = x2 - x1, y2 - y1
	return math_sqrt(dx * dx + dy * dy)
end

--- @param x1 number
--- @param y1 number
--- @param x2 number
--- @param y2 number
--- @return number
function Math.vecDistSq(x1, y1, x2, y2)
	local dx, dy = x2 - x1, y2 - y1
	return dx * dx + dy * dy
end

--- @param x1 number
--- @param y1 number
--- @param x2 number
--- @param y2 number
--- @return number
function Math.vecDot(x1, y1, x2, y2)
	return x1 * x2 + y1 * y2
end

--- @param x1 number
--- @param y1 number
--- @param x2 number
--- @param y2 number
--- @param t number
--- @return number
--- @return number
function Math.vecLerp(x1, y1, x2, y2, t)
	return x1 + (x2 - x1) * t, y1 + (y2 - y1) * t
end

--- Get a normalized vector.
--- @param x number
--- @param y number
--- @return number x
--- @return number y
function Math.vecNorm(x, y)
	local len = math_sqrt(x * x + y * y)
	if len == 0 then
		return 0, 0
	end

	return x / len, y / len
end

--- @param x1 number
--- @param y1 number
--- @param x2 number
--- @param y2 number
--- @return number
--- @return number
function Math.vecProj(x1, y1, x2, y2)
	local lenSq = x2 * x2 + y2 * y2
	if lenSq == 0 then
		return 0, 0
	end

	local scale = (x1 * x2 + y1 * y2) / lenSq
	return x2 * scale, y2 * scale
end

--- Rotate a vector around the origin (0, 0)
function Math.vecRot(x, y, rad)
	local c, s = math_cos(rad), math_sin(rad)
	return x * c - y * s, x * s + y * c
end

--- Get the reflection vector of (x, y) relative to the normal vector (nx, ny)
function Math.vecRfl(x, y, nx, ny)
	local dot2 = 2 * (x * nx + y * ny)
	return x - dot2 * nx, y - dot2 * ny
end

--- @param x1 number
--- @param y1 number
--- @param x2 number
--- @param y2 number
--- @return number
--- @return number
function Math.vecSub(x1, y1, x2, y2)
	return x1 - x2, y1 - y2
end

--#endregion

return Math
