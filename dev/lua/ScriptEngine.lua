local type = type

local function initialize()
	--
end

local lockedTables = setmetatable({}, { __mode = "k" })

--- @param mt metatable
local function markAsLocked(mt)
	if type(mt) == "table" then
		lockedTables[mt] = true
	end
end

--- Assign global shared functions for mod.
--- Override some *dangerous* functions if mod is sandboxed.
--- @param modUID string
--- @param sandboxed boolean
--- @param modGlobals table
--- @param luaGlobals table
local function postProcessModGlobals(modUID, sandboxed, modGlobals, luaGlobals)
	modGlobals.mod = luaGlobals.getModConfig(modUID)

	function modGlobals.N_(str)
		return ("%s_%s"):format(modGlobals.mod.namespace, str)
	end

	assert(modGlobals.N_ == nil)

	if sandboxed then
		local getmetatable = luaGlobals.getmetatable
		local setmetatable = luaGlobals.setmetatable

		function modGlobals.getmetatable(object)
			if type(object) ~= "table" then
				error(("bad argument #1 to 'object' (table expected, got %s)"):format(type(object)), 2)
			end

			local mt = getmetatable(object)
			if mt ~= nil and lockedTables[mt] then
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
			if mt ~= nil and lockedTables[mt] then
				error("metatable is locked, unmodifiable in sandboxed environment", 2)
			end

			return setmetatable(table, metatable)
		end
	else
		modGlobals["jit.profile"] = luaGlobals["jit.profile"]
		modGlobals["jit.util"] = luaGlobals["jit.util"]
	end
end

local function postProcessScriptGlobals(scriptID, scriptName, modUID, sandboxed, func, scriptGlobals, luaGlobals)
	assert(scriptGlobals.scriptID == nil, "`_G.scriptID` is not nil")
	assert(scriptGlobals.scriptName == nil, "`_G.scriptName` is not nil")

	scriptGlobals.scriptID = scriptID
	scriptGlobals.scriptName = scriptName
end

return {
	initialize = initialize,
	markAsLocked = markAsLocked,
	postProcessModGlobals = postProcessModGlobals,
	postProcessScriptGlobals = postProcessScriptGlobals,
}
