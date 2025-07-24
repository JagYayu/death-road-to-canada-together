local type = type

local lockedMetatables = setmetatable({}, { __mode = "k" })

return {
	initialize = function()
		-- jit.off()
	end,
	--- @param mt metatable
	markAsLocked = function(mt)
		if type(mt) == "table" then
			lockedMetatables[mt] = true
		end
	end,
	--- @param modGlobals table
	--- @param luaGlobals table
	--- @param sandboxed boolean
	postProcessModGlobals = function(modUID, sandboxed, modGlobals, luaGlobals)
		modGlobals.mod = luaGlobals.getModConfig(modUID)

		function modGlobals.N_(str)
			return ("%s_%s"):format(modGlobals.mod.namespace, str)
		end

		if sandboxed then
			local getmetatable = luaGlobals.getmetatable
			local setmetatable = luaGlobals.setmetatable

			function modGlobals.getmetatable(object)
				if type(object) ~= "table" then
					error(("bad argument #1 to 'object' (table expected, got %s)"):format(type(object)), 2)
				end

				local mt = getmetatable(object)
				if mt ~= nil and lockedMetatables[mt] then
					error("metatable is locked, inaccessible in sandboxed environment", 2)
				end

				return mt
			end

			function modGlobals.setmetatable(table, metatable)
				if type(table) ~= "table" then
					error(("bad argument #1 to 'table' (table expected, got %s)"):format(type(table)), 2)
				elseif type(metatable) ~= "table" then
					error(("bad argument #2 to 'metatable' (table expected, got %s)"):format(type(metatable)), 2)
				end

				local mt = getmetatable(table)
				if mt ~= nil and lockedMetatables[mt] then
					error("metatable is locked, unmodifiable in sandboxed environment", 2)
				end

				return setmetatable(table, metatable)
			end
		end
	end,
	postProcessScriptGlobals = function(scriptID, scriptName, modUID, sandboxed, func, scriptGlobals, luaGlobals) end,
}
