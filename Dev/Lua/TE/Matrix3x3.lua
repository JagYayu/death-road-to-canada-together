--[[
-- @module TE.Matrix3x3
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class TE.Matrix3x3
--- @field [1] number
--- @field [2] number
--- @field [3] number
--- @field [4] number
--- @field [5] number
--- @field [6] number
--- @field [7] number
--- @field [8] number
--- @field [9] number
local Matrix3x3 = {
	1, 0, 0,
	0, 1, 0,
	0, 0, 1,
}

local math_cos = math.cos
local math_sin = math.sin

--- @type metatable
local metatable = {
	__eq = function(a, b)
		return a[1] == b[1] and a[2] == b[2] and a[3] == b[3] and a[4] == b[4] and a[5] == b[5] and a[6] == b[6] and a[7] == b[7] and a[8] == b[8] and a[9] == b[9]
	end,
	__index = Matrix3x3,
	__mul = function(a, b)
		return a:clone():multiplyMatrix(b)
	end,
}

--- @param m11 number?
--- @param m12 number?
--- @param m13 number?
--- @param m21 number?
--- @param m22 number?
--- @param m23 number?
--- @param m31 number?
--- @param m32 number?
--- @param m33 number?
--- @return TE.Matrix3x3
function Matrix3x3.new(m11, m12, m13, m21, m22, m23, m31, m32, m33)
	return setmetatable({
		m11 or Matrix3x3[1], m12 or Matrix3x3[2], m13 or Matrix3x3[3],
		m21 or Matrix3x3[4], m22 or Matrix3x3[5], m23 or Matrix3x3[6],
		m31 or Matrix3x3[7], m32 or Matrix3x3[8], m33 or Matrix3x3[9],
	}, metatable)
end

--- @return TE.Matrix3x3
function Matrix3x3:clone()
	return Matrix3x3.new(self[1], self[2], self[3], self[4], self[5], self[6], self[7], self[8], self[9])
end

--- @param dst TE.Matrix3x3
--- @return self
function Matrix3x3:copyTo(dst)
	self[1] = dst[1]
	self[2] = dst[2]
	self[3] = dst[3]
	self[4] = dst[4]
	self[5] = dst[5]
	self[6] = dst[6]
	self[7] = dst[7]
	self[8] = dst[8]
	self[9] = dst[9]
	return self
end

--- @return self
function Matrix3x3:reset()
	self[1], self[2], self[3] = Matrix3x3[1], Matrix3x3[2], Matrix3x3[3]
	self[4], self[5], self[6] = Matrix3x3[4], Matrix3x3[5], Matrix3x3[6]
	self[7], self[8], self[9] = Matrix3x3[7], Matrix3x3[8], Matrix3x3[9]
	return self
end

--- @param tx number
--- @param ty number
--- @return self
function Matrix3x3:translate(tx, ty)
	self[3] = self[1] * tx + self[2] * ty + self[3]
	self[6] = self[4] * tx + self[5] * ty + self[6]
	self[9] = self[7] * tx + self[8] * ty + self[9]
	return self
end

--- @param sx number
--- @param sy number
--- @return TE.Matrix3x3
function Matrix3x3:scale(sx, sy)
	self[1] = self[1] * sx
	self[2] = self[2] * sy
	self[3] = self[3]
	self[4] = self[4] * sx
	self[5] = self[5] * sy
	self[6] = self[6]
	self[7] = self[7] * sx
	self[8] = self[8] * sy
	self[9] = self[9]
	return self
end

--- @param radians number
--- @return self
function Matrix3x3:rotate(radians)
	local c = math_cos(radians)
	local s = math_sin(radians)
	self[1] = self[1] * c + self[2] * -s
	self[2] = self[1] * s + self[2] * c
	self[4] = self[4] * c + self[5] * -s
	self[5] = self[4] * s + self[5] * c
	self[7] = self[7] * c + self[8] * -s
	self[8] = self[7] * s + self[8] * c
	return self
end

--- @return self
--- @return (number | 0)
function Matrix3x3:invert()
	local m1, m2, m3 = self[1], self[2], self[3]
	local m4, m5, m6 = self[4], self[5], self[6]
	local m7, m8, m9 = self[7], self[8], self[9]
	local d = m1 * (m5 * m9 - m6 * m8) - m2 * (m4 * m9 - m6 * m7) + m3 * (m4 * m8 - m5 * m7)
	if d == 0 then
		return self, d
	end

	d = 1 / d
	self[1] = (m5 * m9 - m6 * m8) * d
	self[2] = -(m2 * m9 - m3 * m8) * d
	self[3] = (m2 * m6 - m3 * m5) * d
	self[4] = -(m4 * m9 - m6 * m7) * d
	self[5] = (m1 * m9 - m3 * m7) * d
	self[6] = -(m1 * m6 - m3 * m4) * d
	self[7] = (m4 * m8 - m5 * m7) * d
	self[8] = -(m1 * m8 - m2 * m7) * d
	self[9] = (m1 * m5 - m2 * m4) * d
	return self, d
end

--- @param r TE.Matrix3x3
function Matrix3x3:multiplyMatrix(r)
	local l = self
	self[1] = l[1] * r[1] + l[2] * r[4] + l[3] * r[7]
	self[2] = l[1] * r[2] + l[2] * r[5] + l[3] * r[8]
	self[3] = l[1] * r[3] + l[2] * r[6] + l[3] * r[9]
	self[4] = l[4] * r[1] + l[5] * r[4] + l[6] * r[7]
	self[5] = l[4] * r[2] + l[5] * r[5] + l[6] * r[8]
	self[6] = l[4] * r[3] + l[5] * r[6] + l[6] * r[9]
	self[7] = l[7] * r[1] + l[8] * r[4] + l[9] * r[7]
	self[8] = l[7] * r[2] + l[8] * r[5] + l[9] * r[8]
	self[9] = l[7] * r[3] + l[8] * r[6] + l[9] * r[9]
	return self
end

--- @param x number
--- @param y number
--- @return number x
--- @return number y
function Matrix3x3:multiplyVector(x, y)
	return x * self[1] + y * self[2] + self[3], x * self[4] + y * self[5] + self[6]
end

return Matrix3x3
