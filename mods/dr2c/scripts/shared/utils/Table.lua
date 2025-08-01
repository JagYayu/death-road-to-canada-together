local Function = require "dr2c.shared.utils.Function"
local Table = {}

local _ScriptEngine = require "#ScriptEngine"

--- Lock a table's metatable, which can no longer be accessed by `getmetatable` functions in sandboxed scripts.
--- @param tbl table
--- @return metatable? metatable @nil if metatable not exists or valid.
function Table.lockMetatable(tbl)
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
function Table.readonly(tbl)
	return tableToReadonly(tbl, {})
end

--- @param l table
--- @param r table
function Table.deepEquals(l, r)

end

return Table
