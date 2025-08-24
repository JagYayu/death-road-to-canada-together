local Function = {}

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

Function.isTypeBoolean = Function.generateIsType("boolean")

Function.isTypeNumber = Function.generateIsType("number")

Function.isTypeString = Function.generateIsType("string")

Function.isTypeTable = Function.generateIsType("table")

--- @param value any?
--- @return boolean?
function Function.isTypeInteger(value)
	if type(value) == "number" then
		return math.floor(value) == value
	end
end

return Function
