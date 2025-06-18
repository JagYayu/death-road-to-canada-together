local Function = require "dr2c.utils.Function"
local Table = {}

local _ScriptEngine = require "#lua.ScriptEngine"

--- @param tbl table
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

function Table.readonly(tbl)
	return tableToReadonly(tbl, {})
end

return Table
