local ffi = require("ffi")
local inspect = require("inspect")
local jit_util = require("jit.util")

local type = type
local table_concat = table.concat

local lockedTables = setmetatable({}, { __mode = "k" })

local estimateMemoryValueSize = (jit.arch == "x64" or jit.arch == "arm64") and 16 or 8
local estimateMemoryTableStructSize = 40
local estimateMemoryThreadSize = (function()
	collectgarbage()
	local before = collectgarbage("count") * 1024 -- kilo bytes
	local co = coroutine.create(function() end)
	collectgarbage()
	local after = collectgarbage("count") * 1024
	return after - before
end)()
local estimateMemoryDefaultExtraArgs = {
	getFunctionMemory = function(func)
		return 0
	end,
	getUserdataMemory = function(userdata)
		return 0
	end,
}

local function estimateMemorySizeImpl(value, extraArgs, visitedTables)
	local type_ = type(value)
	if type_ == "boolean" or type_ == "number" then
		return estimateMemoryValueSize
	elseif type_ == "string" then
		return estimateMemoryValueSize + #value
	elseif type_ == "function" then
		return estimateMemoryValueSize + extraArgs.getFunctionMemory(value)
	elseif type_ == "userdata" then
		return estimateMemoryValueSize + extraArgs.getUserdataMemory(value)
	elseif type_ == "thread" then
		return estimateMemoryValueSize + estimateMemoryThreadSize
	elseif type_ == "cdata" then
		local ok, size = pcall(ffi.sizeof, value)
		if ok then
			return estimateMemoryValueSize + size
		else
			return estimateMemoryValueSize
		end
	elseif type_ == "table" then
		if visitedTables[value] then
			return 0
		else
			visitedTables[value] = true
		end

		local array_size = jit_util.tablen(value)
		local hash_size = jit_util.tablekeys(value)

		local size = estimateMemoryTableStructSize
		size = size + array_size * estimateMemoryValueSize
		size = size + hash_size * estimateMemoryValueSize * 2

		for k, v in pairs(value) do
			size = size + estimateMemorySizeImpl(k, extraArgs, visitedTables)
			size = size + estimateMemorySizeImpl(v, extraArgs, visitedTables)
		end

		return size
	else
		return 0
	end
end

local function estimateMemorySize(value, extraArgs)
	if type(extraArgs) ~= "table" then
		extraArgs = estimateMemoryDefaultExtraArgs
	end

	estimateMemorySizeImpl(value, extraArgs)
end

local function initialize(luaGlobals)
	jit.off()

	--- @param mt metatable
	--- @return boolean
	function luaGlobals.isLocked(mt)
		return not not lockedTables[mt]
	end
end

--- @param mt metatable
local function markAsLocked(mt)
	if type(mt) == "table" then
		if mt.__metatable then
			mt.__metatable = "Metatable is locked"
		end

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
	local modNamespace = luaGlobals.getModConfig(modUID).namespace

	modGlobals.modNamespace = modNamespace
	modGlobals.modSandboxed = sandboxed
	modGlobals.modUID = modUID

	--- @param str string
	--- @return string
	function modGlobals.N_(str)
		return ("%s_%s"):format(modNamespace, str)
	end

	if sandboxed then
		modGlobals.debug = {
			getinfo = luaGlobals.debug.getinfo,
			traceback = luaGlobals.debug.traceback,
		}

		local getmetatable = luaGlobals.getmetatable
		local setmetatable = luaGlobals.setmetatable
		local debugGetmetatable = luaGlobals.debug.getmetatable
		local debugSetmetatable = luaGlobals.debug.setmetatable

		--- @param tbl table
		--- @return metatable?
		function modGlobals.getmetatable(tbl)
			if type(tbl) ~= "table" then
				error(("bad argument #1 to 'table' (table expected, got %s)"):format(type(tbl)), 2)
			end

			local mt = getmetatable(tbl)
			if mt ~= nil and lockedTables[mt] then
				error("Metatable is locked, inaccessible in a sandboxed environment", 2)
			end

			return mt
		end

		--- @param tbl table
		--- @param mt metatable
		--- @return nil
		function modGlobals.setmetatable(tbl, mt)
			if type(tbl) ~= "table" then
				error(("bad argument #1 to 'table' (table expected, got %s)"):format(type(tbl)), 2)
			elseif type(mt) ~= "table" then
				error(("bad argument #2 to 'metatable' (table expected, got %s)"):format(type(mt)), 2)
			end

			local lmt = getmetatable(tbl)
			if lmt ~= nil and lockedTables[lmt] then
				error("metatable is locked, unmodifiable in sandboxed environment", 2)
			end

			return setmetatable(tbl, mt)
		end

		--- @param tbl table
		--- @return metatable?
		function modGlobals.debug.getmetatable(tbl)
			if type(tbl) ~= "table" then
				error(("bad argument #1 to 'table' (table expected, got %s)"):format(type(tbl)), 2)
			end

			local mt = getmetatable(tbl)
			if mt ~= nil and lockedTables[mt] then
				error("Metatable is locked, inaccessible in a sandboxed environment", 2)
			end

			return debugGetmetatable(tbl)
		end

		--- @param tbl table
		--- @param mt metatable
		--- @return nil
		function modGlobals.debug.setmetatable(tbl, mt)
			if type(tbl) ~= "table" then
				error(("bad argument #1 to 'table' (table expected, got %s)"):format(type(tbl)), 2)
			elseif type(mt) ~= "table" then
				error(("bad argument #2 to 'metatable' (table expected, got %s)"):format(type(mt)), 2)
			end

			local lmt = getmetatable(tbl)
			if lmt ~= nil and lockedTables[lmt] then
				error("metatable is locked, unmodifiable in sandboxed environment", 2)
			end

			return debugSetmetatable(tbl, mt)
		end
	else
		modGlobals.collectgarbage = luaGlobals.collectgarbage
		modGlobals.debug = luaGlobals.debug
		modGlobals.io = luaGlobals.io
		modGlobals.os = luaGlobals.os
		modGlobals.package = luaGlobals.package
		modGlobals["jit.profile"] = luaGlobals["jit.profile"]
		modGlobals["jit.util"] = luaGlobals["jit.util"]
	end
end

local function generateCanOutputFunction(scriptGlobals, rawLog, verb)
	return function()
		return rawLog:canOutput(verb)
	end
end

local function generateOutputFunction(scriptGlobals, rawLog, verb)
	local func = function(...)
		local args = { ... }
		for index, arg in ipairs(args) do
			args[index] = inspect.inspect(arg)
		end

		rawLog:output(verb, args)
	end

	return func
end

local function wrapScriptGlobalsLog(scriptGlobals)
	local rawLog = scriptGlobals.log
	local generated = false

	local function index(self, k)
		if not generated then
			rawset(self, "canTrace", generateCanOutputFunction(self, rawLog, "Trace"))
			rawset(self, "canDebug", generateCanOutputFunction(self, rawLog, "Debug"))
			rawset(self, "canInfo", generateCanOutputFunction(self, rawLog, "Info"))
			rawset(self, "canWarn", generateCanOutputFunction(self, rawLog, "Warn"))
			rawset(self, "canError", generateCanOutputFunction(self, rawLog, "Error"))
			rawset(self, "trace", generateOutputFunction(self, rawLog, "Trace"))
			rawset(self, "debug", generateOutputFunction(self, rawLog, "Debug"))
			rawset(self, "info", generateOutputFunction(self, rawLog, "Info"))
			rawset(self, "warn", generateOutputFunction(self, rawLog, "Warn"))
			rawset(self, "error", generateOutputFunction(self, rawLog, "Error"))

			generated = true

			if self[k] ~= nil then
				return self[k]
			end
		end

		return rawLog[k]
	end

	scriptGlobals.log = setmetatable({}, {
		__index = index,
	})
end

--- @param scriptID integer
--- @param scriptName string
--- @param modUID string
--- @param sandboxed boolean
--- @param func function
--- @param scriptGlobals table
--- @param luaGlobals _G
local function postProcessScriptGlobals(scriptID, scriptName, modUID, sandboxed, func, scriptGlobals, luaGlobals)
	assert(scriptGlobals.scriptID == nil, "`_G.scriptID` is not nil")
	assert(scriptGlobals.scriptName == nil, "`_G.scriptName` is not nil")

	scriptGlobals.scriptID = scriptID
	scriptGlobals.scriptName = scriptName

	wrapScriptGlobalsLog(scriptGlobals)
end

return {
	estimateMemorySize = estimateMemorySize,
	initialize = initialize,
	markAsLocked = markAsLocked,
	postProcessModGlobals = postProcessModGlobals,
	postProcessScriptGlobals = postProcessScriptGlobals,
}
