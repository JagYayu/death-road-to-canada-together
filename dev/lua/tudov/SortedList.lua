--- @class SortedList.Data
--- @field compare fun(l: any, r: any): boolean?

--- @class SortedList
local SortedList = {}

function SortedList:insert()
	--
end

--- @return SortedList.Data
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

return SortedList
