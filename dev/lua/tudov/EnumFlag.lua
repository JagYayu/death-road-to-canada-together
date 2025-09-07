--[[
-- @module tudov.EnumFlag
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class EnumFlag
local EnumFlag = {}

local bit_band = bit.band
local bit_bnot = bit.bnot
local bit_bor = bit.bor
local bit_bxor = bit.bxor
local select = select

--- @param a integer
--- @param ... integer
--- @return boolean
function EnumFlag.hasAny(a, ...)
	for i = 1, select("#", ...) do
		local b = select(i, ...)
		if bit_band(a, b) ~= 0 then
			return true
		end
	end

	return false
end

--- Single parameter overload version, slightly faster than `EnumFlag.hasAny`.
--- @see EnumFlag.hasAny
--- @param a integer
--- @param b integer
--- @return boolean
function EnumFlag.hasAny1(a, b)
	return bit_band(a, b) ~= 0
end

--- @param a integer
--- @param ... integer
--- @return boolean
function EnumFlag.hasAll(a, ...)
	for i = 1, select("#", ...) do
		local b = select(i, ...)
		if bit_band(a, b) ~= b then
			return false
		end
	end

	return true
end

--- Single parameter overload version, slightly faster than `EnumFlag.hasAll`.
--- @see EnumFlag.hasAll
--- @param a integer
--- @param b integer
--- @return boolean
function EnumFlag.hasAll1(a, b)
	return bit_band(a, b) == b
end

--- @type fun(a: integer, ...: integer): integer
EnumFlag.mask = bit_bor

--- @param a integer
--- @param ... integer
--- @return integer
function EnumFlag.unmask(a, ...)
	for i = 1, select("#", ...) do
		local b = select(i, ...)
		a = bit_band(a, bit_bnot(b))
	end

	return a
end

--- Single parameter overload version, slightly faster than `EnumFlag.unmask`.
--- @see EnumFlag.unmask
--- @param a integer
--- @param b integer
--- @return integer
function EnumFlag.unmask1(a, b)
	return bit_band(a, bit_bnot(b))
end

--- @param a integer
--- @param ... integer
--- @return integer
function EnumFlag.toggle(a, ...)
	for i = 1, select("#", ...) do
		local b = select(i, ...)
		a = bit_bxor(a, b)
	end

	return a
end

--- Single parameter overload version, slightly faster than `EnumFlag.toggle`.
--- @see EnumFlag.toggle
--- @param a integer
--- @param b integer
--- @return integer
function EnumFlag.toggle1(a, b)
	return bit_bxor(a, b)
end

return EnumFlag
