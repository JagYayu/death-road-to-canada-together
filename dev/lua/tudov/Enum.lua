local Table = require("tudov.Table")
local String = require("tudov.String")
local inspect = require("inspect")

--- @class Enum.Defaults
--- @field [integer] string
--- @field [string] integer
--- @field [false] integer? @The initial value for enumeration, default: `0`.
--- @field [true] integer? @The value step for enumeration, default: `1`.

--- @class Enum
local Enum = {}

local enumerationsMetadata = {}

local function isEmptyDefaults(defaults)
	local init = defaults[false]
	local step = defaults[true]
	defaults[false] = nil
	defaults[true] = nil

	local isEmpty = next(defaults) == nil

	defaults[false] = init
	defaults[true] = step

	return isEmpty
end

local function metatableNewIndex()
	error("Cannot modify readonly enumeration", 2)
end

local function makeBitFlags(defaults, extend)
	error("NOT IMPLEMENT YET")
end

local function makeIntegers(defaults, extend)
	local metadata = {
		extend = extend,
		step = tonumber(defaults[true]) or 1,
		key2value = {},
		value2keys = {},
		keys = {},
		values = {},
	}
	local initialValue = tonumber(defaults[false]) or 0

	if metadata.step == 0 then
		error("Invalid step value, cannot not be 0", 2)
	elseif metadata.step ~= math.floor(metadata.step) then
		error("Invalid step value, cannot be a fraction", 2)
	end

	--- @param key string
	--- @param value integer
	local function add(key, value)
		metadata.key2value[key] = value

		metadata.value2keys[value] = metadata.value2keys[value] or {}
		table.insert(metadata.value2keys[value], key)

		table.insert(metadata.keys, key)

		table.insert(metadata.values, value)
	end

	if isEmptyDefaults(defaults) then
		add("None", initialValue)
	else
		for key, value in pairs(defaults) do
			if type(key) == "string" then
				if type(value) == "number" then
					add(key, value)
				else
					error("Invalid value type, number expected got " .. type(value), 2)
				end
			elseif type(key) ~= "number" and type(key) ~= "boolean" then
				error("Invalid key type, boolean or number or string expected got " .. type(key), 2)
			end
		end

		Table.listSortAndRemoveDuplications(metadata.values)
		table.sort(metadata.keys)

		local value = initialValue
		for _, key in ipairs(defaults) do
			while metadata.value2keys[value] do
				value = value + 1
			end

			add(key, value)
		end
	end

	setmetatable(metadata.key2value, {
		__index = function(_, k)
			k = tostring(k)

			local might = String.didYouMean(k, metadata.keys)
			if might then
				local fmt =
					"Invalid enum key '%s'. Did you mean '%s'? Or are you try accessing an extended enum during load time?"
				error(fmt:format(k, might), 2)
			else
				error(("Invalid enum key '%s'. Are you try accessing an extended enum during load time?"):format(k), 2)
			end
		end,
	})

	local enum = setmetatable({}, {
		__index = metadata.key2value,
		__newindex = metatableNewIndex,
		__len = function()
			return #metadata.keys
		end,
		__tostring = function()
			return inspect.inspect(metadata.key2value)
		end,
	})

	Table.lockMetatable(enum)
	enumerationsMetadata[enum] = metadata

	return enum
end

local function extendBitFlags(metadata, key)
	-- metadata
end

local function extendSequence(metadata, key) end

function Enum.flags(defaults)
	return makeBitFlags(defaults, extendBitFlags)
end

function Enum.bits(defaults)
	return makeBitFlags(defaults)
end

--- @param defaults Enum.Defaults
--- @return table enum
function Enum.sequence(defaults)
	return makeIntegers(defaults, extendSequence)
end

--- @param defaults Enum.Defaults
--- @return table enum
function Enum.immutable(defaults)
	return makeIntegers(defaults)
end

--- @param enum table
--- @return boolean
function Enum.isValid(enum)
	return not not enumerationsMetadata[enum]
end

local function getMetadata(enum)
	local metadata = enumerationsMetadata[enum]
	if metadata then
		return metadata
	end

	error("Invalid enum", 3)
end

--- @warn Wrap a `pcall` If you are extending bit flag enumerations, because an error will be thrown when the number of flags exceeds 52.
--- @param enum table
--- @param key string
--- @return integer value
function Enum.extend(enum, key)
	local metadata = getMetadata(enum)
	if not metadata.extend then
		error("Enum is not extendable", 3)
	end

	return metadata.extend(metadata, key)
end

--- @param enum table
--- @param key string
--- @return boolean
function Enum.containsKey(enum, key)
	local metadata = getMetadata(enum)
	return not not metadata.key2value[key]
end

--- @param enum table
function Enum.iterateKeyValues(enum)
	local metadata = getMetadata(enum)
	return pairs(metadata.key2value)
end

--- @param enum table
function Enum.iterateKeys(enum)
	local metadata = getMetadata(enum)
	return ipairs(metadata.keys)
end

--- @param enum table
function Enum.iterateValues(enum)
	local metadata = getMetadata(enum)
	return ipairs(metadata.values)
end

--- @param enum table
--- @param value integer
--- @param fallback string?
--- @return string?
function Enum.resolveValue(enum, value, fallback)
	local metadata = getMetadata(enum)
	local keys = metadata.value2keys[value]
	return keys and keys[1] or fallback
end

--- @param enum table
--- @return integer[]
function Enum.eventKeys(enum)
	local metadata = getMetadata(enum)

	local eventKeys = Table.new(#metadata.values, 0)
	for index, value in ipairs(metadata.values) do
		eventKeys[index] = value
	end

	return eventKeys
end

--- @warn Do not modify returned table unless you know what you're doing
--- @return table
function Enum.getAllMetadata()
	return enumerationsMetadata
end

events:add("DebugSnapshot", function(e)
	e.enumerationsMetadata = enumerationsMetadata
end, nil, nil, scriptName)

return Enum
