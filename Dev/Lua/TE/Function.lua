--[[
-- @module tudov.Function
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class TE.Function
local Function = {}

local type = type

function Function.empty() end

function Function.errorModifyReadonly()
	error("Attempt to modify a readonly table", 2)
end

function Function.generateAlways(value)
	return function()
		return value
	end
end

Function.alwaysTrue = Function.generateAlways(true)
Function.alwaysFalse = Function.generateAlways(false)

function Function.add(a, b)
	return a + b
end

function Function.sub(a, b)
	return a - b
end

function Function.mul(a, b)
	return a * b
end

function Function.div(a, b)
	return a / b
end

function Function.generateCompareAsc(keyList)
	local len = #keyList

	return function(l, r)
		for i = 1, len do
			local key = keyList[i]
			local lv, rv = l[key], r[key]

			if lv ~= rv then
				return lv < rv
			end
		end
	end
end

function Function.generateCompareDesc(keyList)
	local len = #keyList

	return function(l, r)
		for i = 1, len do
			local key = keyList[i]
			local lv, rv = l[key], r[key]

			if lv ~= rv then
				return lv > rv
			end
		end
	end
end

--- @param typeName string
--- @return function
function Function.generateIsType(typeName)
	--- @param value any
	--- @return boolean
	return function(value)
		return type(value) == typeName
	end
end

--- @param typeName string
--- @return function
function Function.generateIsOrNilType(typeName)
	--- @param value any
	--- @return boolean
	return function(value)
		return typeName == nil or type(value) == typeName
	end
end

Function.isTypeBoolean = Function.generateIsType("boolean")
Function.isTypeNumber = Function.generateIsType("number")
Function.isTypeString = Function.generateIsType("string")
Function.isTypeTable = Function.generateIsType("table")
Function.isTypeBooleanOrNil = Function.generateIsOrNilType("boolean")
Function.isTypeNumberOrNil = Function.generateIsOrNilType("number")
Function.isTypeStringOrNil = Function.generateIsOrNilType("string")
Function.isTypeTableOrNil = Function.generateIsOrNilType("table")

--- @param keyValueTypes { [1]: any, [2]: type | dr2c.ValueValidator }[]
--- @return dr2c.ValueValidator
function Function.generateTableValidator(keyValueTypes)
	return function(value)
		if type(value) ~= "table" then
			return false
		end

		for _, entry in ipairs(keyValueTypes) do
			local k = entry[1]
			local v = entry[2]

			if type(v) == "function" then
				if not v(value[k]) then
					return false
				end
			else
				if type(value[k]) ~= v then
					return false
				end
			end
		end

		return true
	end
end

function Function._Enum(Enum)
	Function._Enum = nil

	--- @param value any?
	--- @return boolean?
	function Function.isTypeInteger(value)
		if type(value) == "number" then
			return math.floor(value) == value
		end
	end

	--- @param value any?
	--- @return boolean?
	function Function.isTypeIntegerOrNil(value)
		if value == nil then
			return true
		elseif type(value) == "number" then
			return math.floor(value) == value
		end
	end

	--- @param enum table
	--- @return fun(key: string): boolean
	function Function.generateIsEnumKey(enum)
		if not Enum.isValid(enum) then
			error("Not a valid enumeration", 2)
		end

		--- @param key string
		return function(key)
			return Enum.hasKey(enum, key)
		end
	end

	--- @param enum table
	--- @return fun(value: integer | string): boolean
	function Function.generateIsEnumValue(enum)
		if not Enum.isValid(enum) then
			error("Not a valid enumeration", 2)
		end

		--- @param value integer | string
		return function(value)
			return Enum.hasValue(enum, value)
		end
	end
end

return Function
