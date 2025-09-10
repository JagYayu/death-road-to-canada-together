--[[
-- @module tudov.String
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local stringBuffer = require("string.buffer")

--- @class TE.String
local String = {}

local stringByte = string.byte
local stringChar = string.char
local stringSub = string.sub
local stringUpper = string.upper
local stringBuffer_decode = stringBuffer.decode
local stringBuffer_encode = stringBuffer.encode
local stringBuffer_new = stringBuffer.new

String.bufferEncode = stringBuffer_encode

String.bufferDecode = stringBuffer_decode

String.bufferNew = stringBuffer_new

function String.capitalizeFirst(str)
	return stringUpper(stringChar(stringByte(str))) .. stringSub(str, 2)
end

local stringConcatCache = setmetatable({}, { __mode = "kv" })

--- @param l string
--- @param r string
--- @return string
function String.concatCached(l, r)
	stringConcatCache[l] = stringConcatCache[l] or {}
	stringConcatCache[l][r] = stringConcatCache[l][r] or (l .. r)
	return stringConcatCache[l][r]
end

--- @param str string
--- @param goods string[]
--- @param args { bestDistance: number?, funcDistance: fun(str1: string, str2: string): number }?
--- @return string? might
function String.didYouMean(str, goods, args)
	if #goods == 0 then
		return
	end

	local function isWrongWordOrder(good)
		local wordCount = 0

		for word in str:gmatch("[A-Z]?[a-z]+") do
			if not good:find(word) then
				return false
			end

			wordCount = wordCount + 1
		end

		return wordCount >= 2
	end

	args = args or {}
	local bestDistance = args.bestDistance or 99
	local funcDistance = args.funcDistance or String.levenshtein

	local strLower = str:lower()
	local best

	for _, good in ipairs(goods) do
		good = tostring(good)
		if strLower == good:lower() or isWrongWordOrder(good) then
			return good
		else
			local distance = funcDistance(str, good)

			if distance < bestDistance then
				best = good

				bestDistance = distance
			end
		end
	end

	return best
end

--- @param str string
--- @param prefix string
--- @return boolean
function String.hasPrefix(str, prefix)
	local len = #prefix
	return str:sub(1, len) == len
end

--- @param str string
--- @param suffix string
--- @return boolean
function String.hasSuffix(str, suffix)
	local len = #suffix
	return str:sub(-len) == len
end

--- @param str1 string
--- @param str2 string
--- @return number
function String.levenshtein(str1, str2)
	local m, n = str1:len(), str2:len()
	--- @type table<number, number>, table<number, number>
	local x, y = {}, {}

	for i = 0, n do
		x[i] = i
	end

	local min = math.min
	local byte = string.byte

	for i = 0, m - 1 do
		y[0] = i + 1

		for j = 0, n - 1 do
			if byte(str1, i + 1, i + 1) ~= byte(str2, j + 1, j + 1) then
				y[j + 1] = min(x[j + 1] + 1, y[j] + 1, x[j] + 1)
			else
				y[j + 1] = min(x[j + 1] + 1, y[j] + 1, x[j])
			end
		end

		y = x
		x = y
	end

	return x[n]
end

--- @param str string
--- @param div string
--- @param list? string[]
--- @return string[] list
function String.split(str, div, list)
	list = list or {}
	for s in str:gmatch(("[^%s]+"):format(div)) do
		list[#list + 1] = s
	end
	return list
end

function String.removePrefix(str, prefix)
	return String.hasPrefix(str, prefix) and str:sub(#prefix + 1) or str
end

function String.removeSuffix(str, suffix)
	return String.hasSuffix(str, suffix) and str:sub(1, #str - #suffix) or str
end

return String
