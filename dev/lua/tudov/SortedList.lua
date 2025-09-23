--[[
-- @module tudov.SortedList
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local ScriptEngine = require("ScriptEngine")

--- @class TE.SortedList.Data
--- @field compare fun(l: any, r: any): boolean?

--- @class TE.SortedList
local SortedList = {}

function SortedList:insert(value)
	--
end

--- @return TE.SortedList.Data
function SortedList.create(compare)
	return {
		compare = compare,
	}
end

local sortedListMetatable = {
	__index = SortedList,
}

function SortedList.setmetatable(data)
	return setmetatable(data, sortedListMetatable)
end

function SortedList.new()
	return SortedList.setmetatable(SortedList.create())
end

ScriptEngine.lockMetatable(sortedListMetatable)

return SortedList
