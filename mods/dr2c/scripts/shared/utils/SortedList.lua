
--- @class dr2c.SortedList.Data
--- @field compare fun(l: any, r: any): boolean?

--- @class dr2c.SortedList
local SortedList = {}

function SortedList:insert()
	--
end

--- @return dr2c.SortedList.Data
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
