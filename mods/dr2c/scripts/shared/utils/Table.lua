local _ScriptEngine = require("#ScriptEngine")
local Function = require("dr2c.shared.utils.Function")
local stringBuffer = require("string.buffer")

local ipairs = ipairs
local pairs = pairs
local remove = table.remove
local type = type

--- @class Tudov.Table
local GTable = {}

GTable.empty = setmetatable({}, {
	__newindex = Function.empty,
})

GTable.clear = require("table.clear")

GTable.new = require("table.new")

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
function GTable.copy(tbl, depth)
	depth = depth or math.huge

	return depth > 0 and copyImpl(tbl, depth) or copyFinalImpl(tbl)
end

--- Similar to `Utility.copy`, but is much faster.
--- However, serializing a table which contains functions, userdata or a nested table are not supported.
--- It won't preserve metatables for tables either.
--- @generic T : table
--- @param tbl T
--- @return T
function GTable.fastCopy(tbl)
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
function GTable.flatten(tbl, depth, pairsFunc)
	return flattenImpl(tbl, tonumber(depth) or 1, type(pairsFunc) == "function" and pairsFunc or GTable.sortedPairs)
end

--- @generic T : table
--- @param flattened T
--- @return fun(state: T, index: integer)
--- @return T state
--- @return integer index
function GTable.flattenPairs(flattened)
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
--- @return integer[]
function GTable.getIndexList(tbl)
	local keyList = {}

	for k in ipairs(tbl) do
		keyList[#keyList + 1] = k
	end

	return keyList
end

--- @generic T
--- @param tbl table<T, any>
--- @param keys table? @An alias of return table, to avoid GC in specific situations.
--- @return T[]
function GTable.getKeyList(tbl, keys)
	local keyList = keys or {}

	for k in pairs(tbl) do
		keyList[#keyList + 1] = k
	end

	return keyList
end

--- @generic T
--- @param tbl table<any, T>
--- @return T[]
function GTable.getValueList(tbl)
	local valueList = {}

	for _, v in pairs(tbl) do
		valueList[#valueList + 1] = v
	end

	return valueList
end

--- @generic K, V
--- @param tbl table<K, V>
--- @return K[], V[]
function GTable.getKeyValueLists(tbl)
	local keyList = {}
	local valueList = {}

	for k, v in pairs(tbl) do
		local i = #keyList + 1

		keyList[i] = k
		valueList[i] = v
	end

	return keyList, valueList
end

--- @generic T
--- @param list T[]
--- @param value T
--- @return integer? pos
function GTable.listFastRemoveFirst(list, value)
	for pos, val in ipairs(list) do
		if val == value then
			list[pos] = remove(list, #list)
			return pos
		end
	end
end

--- @generic T
--- @param list T[]
--- @param func fun(value: T, index: integer): boolean
--- @return integer? pos
function GTable.listFastRemoveFirstIf(list, func)
	for pos, val in ipairs(list) do
		if func(val, pos) then
			list[pos] = remove(list, #list)
			return pos
		end
	end
end

--- @generic T
--- @param list T[]
--- @param value T
function GTable.listRemoveFirst(list, value)
	for pos, val in ipairs(list) do
		if val == value then
			return remove(list, pos)
		end
	end
end

--- @generic T
--- @param list T[]
--- @param func fun(value: T, index: integer): boolean
function GTable.listRemoveFirstIf(list, func)
	for pos, val in ipairs(list) do
		if func(val, pos) then
			return remove(list, pos)
		end
	end
end

--- @generic T
--- @param list T[]
--- @return T[]
function GTable.listRemoveDuplicates(list)
	local size = #list

	if size ~= 0 then
		local seen = {}
		local target = 1

		for i = 1, size do
			if not seen[list[i]] then
				seen[list[i]] = true
				list[target] = list[i]
				target = target + 1
			end
		end

		for i = target, size do
			list[i] = nil
		end
	end

	return list
end

--- @generic T
--- @param list T[]
--- @param func fun(value: T, index: integer): boolean
--- @return T[] list
function GTable.listRemoveIf(list, func)
	local l, r = 1, 1

	while #list >= r do
		if func(list[r], r) then
			list[r] = nil
		else
			if l ~= r then
				list[l] = list[r]
				list[r] = nil
			end

			l = l + 1
		end

		r = r + 1
	end

	return list
end

function GTable.reverse(list)
	local len = #list

	for i = 1, math.floor(len / 2) do
		list[len - i + 1] = list[i]
		list[i] = list[len - i + 1]
	end

	return list
end

do
	local function reversedPairsIterator(list, index)
		index = index - 1

		if list[index] then
			return index, list[index]
		end
	end

	function GTable.reversedPairs(list)
		return reversedPairsIterator, list, #list + 1
	end
end

do
	local function sortedPairsIterator(tbl, state)
		local index = state[2]
		index = index + 1
		state[2] = index

		local key = state[1][index]
		if key ~= nil then
			return state, key, tbl[key]
		end
	end

	--- @generic TK, TV
	--- @param tbl table<TK, TV>
	--- @param sort fun(comp: (fun(a: TK, b: TK): boolean)?)?
	--- @param comp (fun(a: TK, b: TK): boolean)?
	--- @return fun(): TK, TV
	--- @return table<TK, TV> tbl
	--- @return { [1]: TK[], [2]: integer } state @readonly
	function GTable.sortedPairs(tbl, sort, comp)
		local keyList = GTable.getKeyList(tbl)

		do
			(sort or table.sort)(keyList, comp)
		end

		return sortedPairsIterator, tbl, {
			keyList,
			0,
		}
	end
end

--- @generic T
--- @param list T[]
--- @return table<T, number>
--- @nodiscard
function GTable.invertIndexValues(list)
	local tbl = GTable.new(0, #list)

	for index, value in ipairs(list) do
		tbl[value] = index
	end

	return tbl
end

local function mergeImpl(dest, src, depth)
	if depth > 0 and type(src) == "table" and type(dest) == "table" then
		for key, val in pairs(src) do
			dest[key] = mergeImpl(dest[key], val, depth - 1)
		end

		return dest
	end

	return GTable.copy(src)
end

--- @generic T : table
--- @param dest T
--- @param src table
--- @param depth number? default: 1
--- @return T tbl
function GTable.merge(dest, src, depth)
	return mergeImpl(dest, src, depth or 1)
end

function GTable.mergeDefaults(dest, src)
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
function GTable.mergeExists(dest, src)
	for k, v in pairs(src) do
		if dest[k] ~= nil then
			dest[k] = v
		end
	end

	return dest
end

--- @generic T
--- @param list T[]
--- @param items T[]
--- @return T[] list
function GTable.append(list, items)
	local len = #list

	for idx, val in ipairs(items) do
		list[len + idx] = val
	end

	return list
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
function GTable.find(tbl, value, pairsFunc)
	return findImpl(tbl, value, pairsFunc or pairs)
end

--- @generic T
--- @param list T[]
--- @param value T
--- @return integer? index
--- @return T? value
--- @nodiscard
function GTable.listFindFirst(list, value)
	return findImpl(list, value, ipairs)
end

--- @generic K, V, Arg...
--- @param tbl table<K, V>
--- @param func fun(value: V, key: K, ...: Arg...): boolean
--- @return K? key
--- @return V? value
--- @nodiscard
local function findFirstIfImpl(tbl, func, funcPairs, ...)
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
function GTable.findFirstIf(tbl, func, pairsFunc, ...)
	return findFirstIfImpl(tbl, func, pairsFunc or pairs, ...)
end

--- @generic T
--- @param l T[]
--- @param r T[]
--- @return T[]
--- @nodiscard
function GTable.listConcat(l, r)
	local ll = #l
	for i = 1, #r do
		l[ll + i] = r[i]
	end
	return l
end

--- @generic T, Arg...
--- @param list table<integer, T>
--- @param func fun(value: T, index: integer, ...: Arg...): boolean
--- @param ... Arg...
--- @return integer? index
--- @return T? value
--- @nodiscard
function GTable.listFindFirstIf(list, func, ...)
	return findFirstIfImpl(list, func, ipairs, ...)
end

--- @generic T, Arg...
--- @param list table<integer, T>
--- @param func fun(value: T, index: integer, ...: Arg...): boolean
--- @param ... Arg...
--- @return integer? index
--- @return T? value
--- @nodiscard
function GTable.listFindLastIf(list, func, ...)
	return findFirstIfImpl(list, func, GTable.reversedPairs, ...)
end

--- @warn This function removes table's hash part.
--- @generic T
--- @param list T[]
--- @return T[]
function GTable.listShrinkToFit(list, length)
	local l = list

	list = GTable.new(#l, 0)
	for i = 1, length do
		list[i] = l[i]
	end

	return list
end

--- @generic T
--- @param list T[]
--- @return { [T]: integer }
function GTable.listToSet(list)
	local set = {}

	for i = 1, #list do
		set[list[i]] = i
	end

	return set
end

assert(type(_G.newproxy) == "function", "`_G.newproxy` is not a function!")

--- @generic T : table
--- @param table T
--- @param metatable metatable
--- @return T
function GTable.setProxyMetatable(table, metatable)
	local instance = newproxy(true)
	--- @diagnostic disable-next-line: invisible
	getmetatable(instance).__gc = metatable.__gc
	metatable[instance] = true
	return setmetatable(table, metatable)
end

--- Lock a table's metatable, which can no longer be accessed by `getmetatable` functions in sandboxed scripts.
--- @param tbl table
--- @return metatable? metatable @nil if metatable not exists or valid.
function GTable.lockMetatable(tbl)
	if type(tbl) == "table" then
		local mt = getmetatable(tbl)
		if type(mt) == "table" then
			_ScriptEngine.markAsLocked(mt)
			return mt
		end
	end
end

local function tableToReadonly(tbl, seen)
	if seen[tbl] or getmetatable(seen[tbl]) ~= nil then
		return tbl
	end

	local index = {}
	local proxy = setmetatable({}, { __index = index, __newindex = Function.newIndexReadonly })
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
--- @param tbl table
--- @return table
function GTable.readonly(tbl)
	return tableToReadonly(tbl, {})
end

--- TODO
--- @param l table
--- @param r table
function GTable.deepEquals(l, r) end

GTable.lockMetatable(GTable.empty)

return GTable
