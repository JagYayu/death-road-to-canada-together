--[[
-- @module TE.List
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("TE.Table")
local table_new = require("table.new")

local Table_find = Table.find
local Table_findFirstIf = Table.findFirstIf
local Table_findFirstIfV = Table.findFirstIfV
local ipairs = ipairs
local math_floor = math.floor
local math_min = math.min
local remove = table.remove
local table_sort = table.sort
local type = type

--- @class TE.List
local List = {}

--- @generic T
--- @param list T[]
--- @param depth integer
--- @return T[]
local function copyArrayImpl(list, depth)
	local result = table_new(#list, 0)

	if depth > 0 then
		for i, v in ipairs(list) do
			if type(v) == "table" then
				result[i] = copyArrayImpl(v, depth - 1)
			end
		end
	else
		for i, v in ipairs(list) do
			result[i] = v
		end
	end

	return result
end

--- Copy array part of a table.
--- This is faster than `Table.fastCopy`, but only support copy array part of tables inside.
--- @generic T
--- @param list T[]
--- @param depth integer? @default: 0
--- @return T[]
--- @nodiscard
function List.copy(list, depth)
	depth = tonumber(depth) or 0

	return copyArrayImpl(list, depth)
end

--- 从列表`list`中快速移除第一个值为`value`的元素。
--- 与`List.removeFirst`不同的是，它通过交换待移除元素与列表最后一个元素来实现，因此该函数会使列表无序。
--- @generic T
--- @param list T[]
--- @param value T
--- @return integer? pos
function List.fastRemoveFirst(list, value)
	for pos, val in ipairs(list) do
		if val == value then
			list[pos] = remove(list, #list)
			return pos
		end
	end
end

--- @generic T
--- @param list T[]
--- @param condition fun(value: T, index: integer): boolean
--- @return integer? pos
function List.fastRemoveFirstIf(list, condition)
	for pos, val in ipairs(list) do
		if condition(val, pos) then
			list[pos] = remove(list, #list)
			return pos
		end
	end
end

--- @generic T
--- @generic TArg...
--- @param list T[]
--- @param condition fun(value: T, index: integer, ...: TArg...): boolean
--- @return integer? pos
function List.fastRemoveFirstIfV(list, condition, ...)
	for pos, val in ipairs(list) do
		if condition(val, pos, ...) then
			list[pos] = remove(list, #list)
			return pos
		end
	end
end

--- 从列表`list`中移除第一个值为`value`的元素，并返回下标索引。它通过`table.remove`实现，会保持列表有序。
--- @generic T
--- @param list T[]
--- @param value T
--- @return integer?
function List.removeFirst(list, value)
	for pos, val in ipairs(list) do
		if val == value then
			remove(list, pos)
			return pos
		end
	end
end

--- @generic T
--- @param list T[]
--- @param func fun(value: T, index: integer): boolean
function List.removeFirstIf(list, func)
	for pos, val in ipairs(list) do
		if func(val, pos) then
			return remove(list, pos)
		end
	end
end

--- @generic T
--- @generic TArgs
--- @param list T[]
--- @param condition fun(value: T, index: integer, ...: TArgs): boolean
--- @param ... TArgs
function List.removeFirstIfV(list, condition, ...)
	for pos, val in ipairs(list) do
		if condition(val, pos, ...) then
			return remove(list, pos)
		end
	end
end

--- @generic T
--- @param list T[]
--- @return T[]
function List.removeDuplications(list)
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
--- @return T[]
function List.removeSortedListDuplications(list)
	local ti = 1
	local tv = list[1]

	for read = 2, #list do
		local v = list[read]
		if v ~= tv then
			ti = ti + 1
			list[ti] = v
			tv = v
		end
	end

	for i = #list, ti + 1, -1 do
		list[i] = nil
	end

	return list
end

---@generic T
---@param list T[]
---@param compare? fun(a: T, b: T): boolean
function List.sort(list, compare)
	table_sort(list, compare)
	return list
end

--- @generic T
--- @param list T[]
--- @param comp (fun(a: integer, b: T): boolean)?
--- @return T[]
function List.sortAndRemoveDuplications(list, comp)
	table_sort(list, comp)
	return List.removeSortedListDuplications(list)
end

--- @generic T
--- @param list T[]
--- @param condition fun(value: T, index: integer): boolean
--- @return T[] list
function List.removeIf(list, condition)
	local l, r = 1, 1

	while #list >= r do
		if condition(list[r], r) then
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

--- @generic T, TArgs...
--- @param list T[]
--- @param condition fun(value: T, index: integer, ...: TArgs...): boolean
--- @param ... TArgs...
--- @return T[] list
function List.removeIfV(list, condition, ...)
	local l, r = 1, 1

	while #list >= r do
		if condition(list[r], r, ...) then
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

--- @generic T
--- @param l T[]
--- @param r T[]
--- @return T[]
--- @nodiscard
function List.concat(l, r)
	local ll = #l
	for i = 1, #r do
		l[ll + i] = r[i]
	end
	return l
end

--- @generic T
--- @param list T[]
--- @param value T
--- @return integer? index
--- @return T? value
--- @nodiscard
function List.findFirst(list, value)
	return Table_find(list, value, ipairs)
end

--- @generic T
--- @param list table<integer, T>
--- @param condition fun(value: T, index: integer): boolean
--- @return integer? index
--- @return T? value
--- @nodiscard
function List.findFirstIf(list, condition)
	return Table_findFirstIf(list, condition, ipairs)
end

--- @generic T, TArgs...
--- @param list table<integer, T>
--- @param condition fun(value: T, index: integer, ...: TArgs...): boolean
--- @param ... TArgs...
--- @return integer? index
--- @return T? value
--- @nodiscard
function List.findFirstIfV(list, condition, ...)
	return Table_findFirstIfV(list, condition, ipairs, ...)
end

--- Same as `List.findFirstIf`, but only return a value instead of a pair.
--- @generic TValue
--- @param list table<integer, TValue>
--- @param condition fun(value: TValue, index: integer): boolean
--- @return TValue? value
--- @nodiscard
function List.findFirstValueIf(list, condition)
	local _, value = Table_findFirstIf(list, condition, ipairs)
	return value
end

--- @generic TValue, TArgs...
--- @param list table<integer, TValue>
--- @param condition fun(value: TValue, index: integer, ...: TArgs...): boolean
--- @param ... TArgs...
--- @return TValue? value
--- @nodiscard
function List.findFirstValueIfV(list, condition, ...)
	local _, value = Table_findFirstIfV(list, condition, ipairs, ...)
	return value
end

local List_reversedPairs

--- @generic TValue
--- @param list table<integer, TValue>
--- @param condition fun(value: TValue, index: integer): boolean
--- @return integer? index
--- @return TValue? value
--- @nodiscard
function List.findLastIf(list, condition)
	return Table_findFirstIf(list, condition, List_reversedPairs)
end

--- @generic TValue, TArgs...
--- @param list table<integer, TValue>
--- @param condition fun(value: TValue, index: integer, ...: TArgs...): boolean
--- @param ... TArgs...
--- @return integer? index
--- @return TValue? value
--- @nodiscard
function List.findLastIfV(list, condition, ...)
	return Table_findFirstIfV(list, condition, List_reversedPairs, ...)
end

--- @generic TValue
--- @param list table<integer, TValue>
--- @param condition fun(value: TValue, index: integer): boolean
--- @return TValue? value
--- @nodiscard
function List.findLastValueIf(list, condition)
	local _, value = Table_findFirstIf(list, condition, List_reversedPairs)
	return value
end

--- @generic TValue, TArgs...
--- @param list table<integer, TValue>
--- @param condition fun(value: TValue, index: integer, ...: TArgs...): boolean
--- @param ... TArgs...
--- @return TValue? value
--- @nodiscard
function List.findLastValueIfV(list, condition, ...)
	local _, value = Table_findFirstIfV(list, condition, List_reversedPairs, ...)
	return value
end

function List.reverse(list)
	local len = #list

	for i = 1, math_floor(len / 2) do
		list[len - i + 1] = list[i]
		list[i] = list[len - i + 1]
	end

	return list
end

--- @generic T
--- @param list T[]
--- @param index integer
--- @return integer? index
--- @return T? index
--- @nodiscard
local function reversedPairsIterator(list, index)
	index = index - 1

	if list[index] then
		return index, list[index]
	end
end

--- @generic T
--- @param list T[]
--- @return fun(list: T[], index: integer): (index: integer?, index: T?)
--- @return T[]
--- @return integer?
function List.reversedPairs(list)
	return reversedPairsIterator, list, #list + 1
end

List_reversedPairs = List.reversedPairs

--- @generic T
--- @param list T[]
--- @param items T[]
--- @return T[] list
function List.append(list, items)
	local len = #list

	for idx, val in ipairs(items) do
		list[len + idx] = val
	end

	return list
end

--- @generic T
--- @param list T[]
--- @return { [T]: integer }
function List.toSet(list)
	local set = {}

	for i = 1, #list do
		set[list[i]] = i
	end

	return set
end

--- @generic T
--- @param list T[]
--- @param length integer?
--- @return T[]
function List.shrink(list, length)
	local result = table_new(#list, 0)

	for i = 1, length and math_min(#list, length) or #list do
		result[i] = list[i]
	end

	return result
end

--- @generic T
--- @param list T[]
--- @return table<T, number>
--- @nodiscard
function List.invertIndexValues(list)
	local tbl = table_new(0, #list)

	for index, value in ipairs(list) do
		tbl[value] = index
	end

	return tbl
end

--- @generic T
--- @param list T[]
--- @param value T
--- @return integer
function List.lowerBound(list, value)
	local first, count = 1, #list
	local step, index

	while count > 0 do
		step = math_floor(count / 2)
		index = first + step

		if value > list[index] then
			first = index + 1
			count = count - step - 1
		else
			count = step
		end
	end

	return first
end

--- @param l any[]
--- @param r any[]
--- @return boolean
function List.equals(l, r)
	if #l ~= #r then
		return false
	end

	for i = 1, #l do
		if l[i] ~= r[i] then
			return false
		end
	end

	return true
end

local List_equals = List.equals

--- @param l any[]
--- @param r any[]
--- @param ... any[]
--- @return boolean
function List.equalsV(l, r, ...)
	if not List_equals(l, r) then
		return false
	end

	for i = 1, select("#", ...) do
		if not List_equals(l, select(i, ...)) then
			return false
		end
	end

	return true
end

return List
