local Enum = {}

local enumerations = {}

local function makeBits(defaults, extendable)

end

local function makeInts(defaults, extendable)
	local enumMap = {}

	for value, key in ipairs(defaults) do
		enumMap[tostring(key)] = value
	end

	return setmetatable({}, {
		__index = function(_, k)
			local value = enumMap[k]
			if not value then
				error("Invalid enum " .. k, 2)
			end

			return value
		end,
	})
end

function Enum.bitmask(defaults)
	return makeBits(defaults, true)
end

function Enum.flags(defaults)
	return makeBits(defaults, false)
end

function Enum.sequence(defaults)
	return makeInts(defaults, true)
end

--- @param defaults string[]
--- @return nil
function Enum.immutable(defaults)
	return makeInts(defaults, false)
end

function Enum.extend()
end

function Enum.getAll()
end

return Enum
