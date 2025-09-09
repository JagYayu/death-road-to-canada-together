-- local Table = require "tudov.Table"

-- --- @class dr2c.GReuseTable
-- local GReuseTable = {}

-- local defaultMetatable = {}

-- local tablePools = {
-- 	[defaultMetatable] = {
-- 		size = 32,
-- 	},
-- }

-- --- @param metatable metatable?
-- function GReuseTable.setPoolSize(size, metatable)
-- 	local pools = tablePools[metatable or defaultMetatable]
-- 	if not pools then
-- 		pools ={}
-- 		tablePools[metatable or defaultMetatable] = pools
-- 	end
-- end

-- --- @param metatable metatable?
-- --- @param array integer?
-- --- @param hash integer?
-- function GReuseTable.alloc(metatable, array, hash)
-- 	if not metatable then
-- 		metatable = defaultMetatable
-- 	elseif metatable.__gc then
-- 		error("Could not customize custom '__gc' behavior", 2)
-- 	end

-- 	local tbl = Table.new(array or 0, hash or 0)
-- 	Table.setProxyMetatable(tbl, metatable)

-- 	setmetatable(tbl, {})
-- end

-- function GReuseTable.dealloc()

-- end

-- return GReuseTable
