--[[
-- @module tudov.Table
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local ScriptEngine = require("ScriptEngine")
local Function = require("TE.Function")
local stringBuffer = require("string.buffer")
local table_clear = require("table.clear")
local table_new = require("table.new")

local getmetatable = getmetatable
local ipairs = ipairs
local math_floor = math.floor
local math_min = math.min
local newproxy = newproxy
local pairs = pairs
local remove = table.remove
local setmetatable = setmetatable
local type = type

assert(type(_G.newproxy) == "function", "`_G.newproxy` is not a function!")

--- @class TE.Table
local Table = {}

Table.clear = table_clear
Table.new = table_new

Table.empty = setmetatable({}, {
	__newindex = Function.empty,
})

--- @diagnostic disable-next-line: deprecated
Table.foreach = table.foreach

--- @diagnostic disable-next-line: deprecated
Table.foreachi = table.foreachi

--- @generic T: table, K, V
--- @param tbl table
--- @return table
local function copyFinalImpl(tbl)
	local result = {}

	for k, v in pairs(tbl) do
		result[k] = v
	end

	return setmetatable(result, getmetatable(tbl))
end

--- @generic T: table, K, V
--- @param tbl table
--- @param depth number
--- @return table
local function copyImpl(tbl, depth)
	local result = {}

	for k, v in pairs(tbl) do
		result[k] = v

		if type(v) == "table" then
			if depth > 0 then
				result[k] = copyImpl(v, depth - 1)
			else
				result[k] = copyFinalImpl(v)
			end
		end
	end

	return setmetatable(result, getmetatable(tbl))
end

--- Fully copy a table.
--- @generic T : table
--- @param tbl T
--- @param depth number? default: math.huge
--- @return T
function Table.copy(tbl, depth)
	depth = tonumber(depth) or math.huge

	return depth > 0 and copyImpl(tbl, depth) or copyFinalImpl(tbl)
end

--- Similar to `Utility.copy`, but is much faster.
--- However, serializing a table which contains functions, userdata or a nested table are not supported.
--- It won't preserve metatables for tables either.
--- @generic T : table
--- @param tbl T
--- @return T
function Table.fastCopy(tbl)
	if type(tbl) ~= "table" then
		error("Type mismatch", 2)
	end

	tbl = stringBuffer.decode(stringBuffer.encode(tbl))

	if type(tbl) ~= "table" then
		error("Failed to fast copy", 2)
	end

	return tbl
end

local flattenImpl
do
	local function flattenImplInsert(t, v, d, pf)
		t[#t + 1] = type(v) == "table" and flattenImpl(v, d - 1, pf) or v
	end

	flattenImpl = function(t, d, pf)
		if d <= 0 then
			return t
		end

		local res = {}
		for k, v in pf(t) do
			flattenImplInsert(res, k, d, pf)
			flattenImplInsert(res, v, d, pf)
		end
		return res
	end
end

--- Returns a list of the table’s key-value pairs.
--- e.g. { b = 42, a = 51, c = 12 } => { "b", 42, "a", 51, "c", 12 }
--- Use `Table.flattenPairs` to quickly iterate returned table.
--- @generic TKey, TValue
--- @param tbl table<TKey, TValue>
--- @param depth integer? @default: math.huge
--- @return (TKey | TValue)[]
function Table.flatten(tbl, depth, pairsFunc)
	return flattenImpl(tbl, tonumber(depth) or 1, type(pairsFunc) == "function" and pairsFunc or Table.sortedPairs)
end

--- @generic T : table
--- @param flattened T
--- @return fun(state: T, index: integer)
--- @return T state
--- @return integer index
function Table.flattenPairs(flattened)
	local function it(state, index)
		index = index + 2
		local value = state[index]

		if value ~= nil then
			return index, state[index - 1], value
		end
	end

	return it, flattened, 0
end

--- @param tbl any[]
--- @return integer[] list
function Table.getIndexList(tbl)
	local keyList = {}

	for k in ipairs(tbl) do
		keyList[#keyList + 1] = k
	end

	return keyList
end

--- @generic T
--- @param tbl table<T, any>
--- @param keys? (table | integer) @An alias of return table, to avoid GC in specific situations.
--- @return T[] list
function Table.getKeyList(tbl, keys)
	local keysType = type(keys)
	local keyList = keysType == "number" and table_new(keys, 0) --
		or keysType == "table" and keys
		or {}

	for k in pairs(tbl) do
		keyList[#keyList + 1] = k
	end

	return keyList
end

--- @generic T
--- @param tbl table<any, T>
--- @param values (T[] | integer)? @table: fill result values to this table; integer: preallocate this size for result table.
--- @return T[]
function Table.getValueList(tbl, values)
	local valuesType = type(values)
	local valueList = valuesType == "number" and table_new(values, 0) --
		or valuesType == "table" and values --
		or {}

	for _, v in pairs(tbl) do
		valueList[#valueList + 1] = v
	end

	return valueList
end

--- @generic K, V
--- @param tbl table<K, V>
--- @param keys (table | integer)?
--- @param values (table | integer)?
--- @return K[], V[]
function Table.getKeyValueLists(tbl, keys, values)
	local keysType = type(keys)
	local keyList = keysType == "number" and table_new(keys, 0) --
		or keysType == "table" and keys
		or {}
	local valuesType = type(values)
	local valueList = valuesType == "number" and table_new(values, 0) --
		or valuesType == "table" and values
		or {}

	for k, v in pairs(tbl) do
		local i = #keyList + 1

		keyList[i] = k
		valueList[i] = v
	end

	return keyList, valueList
end

--- @generic TK, TV
--- @param tbl table<TK, TV>
--- @param state { [1]: TK[], [2]: integer }
--- @return { [1]: TK[], [2]: integer }? state
--- @return TK key
--- @return TV value
local function sortedPairsIterator(tbl, state)
	local index = state[2]
	index = index + 1
	state[2] = index

	local key = state[1][index]
	if key ~= nil then
		return state, key, tbl[key]
	else
		return nil, nil, nil
	end
end

local sortedPairsEmptyState = { {}, 0 }

--- @generic TK, TV
--- @param tbl table<TK, TV>
--- @param sort fun(comp: (fun(a: TK, b: TK): boolean)?)?
--- @param comp (fun(a: TK, b: TK): boolean)?
--- @return fun(): TK, TV
--- @return table<TK, TV> tbl
--- @return { [1]: TK[], [2]: integer } state @readonly
function Table.sortedPairs(tbl, sort, comp)
	local keyList = Table.getKeyList(tbl)
	if keyList[1] then
		do
			(sort or table.sort)(keyList, comp)
		end

		return sortedPairsIterator, tbl, {
			keyList,
			0,
		}
	else
		return sortedPairsIterator, tbl, sortedPairsEmptyState
	end
end

local function mergeImpl(dest, src, depth)
	if depth > 0 and type(src) == "table" and type(dest) == "table" then
		for key, val in pairs(src) do
			dest[key] = mergeImpl(dest[key], val, depth - 1)
		end

		return dest
	end

	return Table.copy(src)
end

--- @generic T : table
--- @param dest T
--- @param src table
--- @param depth number? default: 1
--- @return T tbl
function Table.merge(dest, src, depth)
	return mergeImpl(dest, src, depth or 1)
end

function Table.mergeDefaults(dest, src)
	for k, v in pairs(src) do
		if dest[k] == nil then
			dest[k] = v
		end
	end

	return dest
end

--- @generic T : table
--- @param dest T
--- @param src table
--- @return T
function Table.mergeExists(dest, src)
	for k, v in pairs(src) do
		if dest[k] ~= nil then
			dest[k] = v
		end
	end

	return dest
end

--- @generic TK, TV
--- @param tbl table<TK, TV>
--- @param val TV
--- @return TK? key
--- @return TV? value
--- @nodiscard
local function findImpl(tbl, val, pairsFunc)
	for k, v in pairsFunc(tbl) do
		if v == val then
			return k, v
		end
	end
end

--- @generic TK, TV
--- @param tbl table<TK, TV>
--- @param value TV
--- @return TK? key
--- @return TV? value
--- @nodiscard
function Table.find(tbl, value, pairsFunc)
	return findImpl(tbl, value, pairsFunc or pairs)
end

--- @generic K, V, TArgs...
--- @param tbl table<K, V>
--- @param func fun(value: V, key: K, ...: TArgs...): boolean
--- @return K? key
--- @return V? value
--- @nodiscard
local function findFirstIfImpl(tbl, func, funcPairs)
	for k, v in funcPairs(tbl) do
		if func(v, k) then
			return k, v
		end
	end
end

--- @generic TK, TV
--- @param tbl table<TK, TV>
--- @param func fun(value: TV, key: TK): boolean
--- @return TK? key
--- @return TV? value
--- @nodiscard
function Table.findFirstIf(tbl, func, pairsFunc)
	return findFirstIfImpl(tbl, func, pairsFunc or pairs)
end

--- @generic K, V, TArgs...
--- @param tbl table<K, V>
--- @param func fun(value: V, key: K, ...: TArgs...): boolean
--- @return K? key
--- @return V? value
--- @nodiscard
local function findFirstIfVImpl(tbl, func, funcPairs, ...)
	for k, v in funcPairs(tbl) do
		if func(v, k, ...) then
			return k, v
		end
	end
end

--- @generic TK, TV
--- @param tbl table<TK, TV>
--- @param func fun(value: TV, key: TK): boolean
--- @param ... any
--- @return TK? key
--- @return TV? value
--- @nodiscard
function Table.findFirstIfV(tbl, func, pairsFunc, ...)
	return findFirstIfVImpl(tbl, func, pairsFunc or pairs, ...)
end

--- @generic T : table
--- @param table T
--- @param metatable metatable
--- @return T
function Table.setProxyMetatable(table, metatable)
	local instance = newproxy(true)

	--- @diagnostic disable-next-line: invisible
	getmetatable(instance).__gc = metatable.__gc
	metatable[instance] = true

	return setmetatable(table, metatable)
end

--- Lock a table's metatable, which can no longer be accessed by `getmetatable` functions in sandboxed scripts.
--- @param tbl table
--- @return metatable? metatable @nil if metatable not exists or valid.
function Table.lockMetatable(tbl)
	if type(tbl) == "table" then
		local mt = getmetatable(tbl)
		if type(mt) == "table" then
			ScriptEngine.lockMetatable(mt)
			return mt
		end
	end
end

local function tableToReadonly(tbl, seen)
	if seen[tbl] or getmetatable(seen[tbl]) ~= nil then
		return tbl
	end

	local index = {}
	local proxy = setmetatable({}, { __index = index, __newindex = Function.errorModifyReadonly })
	seen[tbl] = proxy

	for k, v in pairs(tbl) do
		if type(k) == "table" then
			k = tableToReadonly(k, seen)
		end

		if type(v) == "table" then
			v = tableToReadonly(v, seen)
		end

		index[k] = v
	end

	return proxy
end

--- Create a readonly proxy table, which is readonly to original table.
--- @generic T : table
--- @param tbl T
--- @return T
function Table.readonly(tbl)
	return tableToReadonly(tbl, {})
end

--- @param l any
--- @param r any
--- @param visited table<any, true>
--- @return boolean
local function deepEqualsImpl(l, r, visited)
	if l == r then
		return true
	end

	do
		local lt = type(l)
		if lt ~= "table" then
			return l == r
		elseif lt ~= type(r) then
			return false
		end
	end

	if visited[l] and visited[l][r] then
		return true
	end

	visited[l] = visited[l] or {}
	visited[l][r] = true

	for k, v in pairs(l) do
		if not deepEqualsImpl(v, r[k], visited) then
			return false
		end
	end

	for k in pairs(r) do
		if l[k] == nil then
			return false
		end
	end

	return true
end

--- @param l table
--- @param r table
--- @return boolean
function Table.deepEquals(l, r)
	return deepEqualsImpl(l, r, {})
end

Table.lockMetatable(Table.empty)

return Table
