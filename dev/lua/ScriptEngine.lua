local ffi = require("ffi")
local jit_util = require("jit.util")

local type = type
local table_concat = table.concat

--- @class dr2c.Utility
local Utility = {}

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

local function initialize() end

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
	local modNamespace = luaGlobals.getModConfig(modUID).namespace

	modGlobals.modNamespace = modNamespace
	modGlobals.modSandboxed = sandboxed
	modGlobals.modUID = modUID

	function modGlobals.N_(str)
		return ("%s_%s"):format(modNamespace, str)
	end

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

local function generateCanOutputFunction(scriptGlobals, rawLog, verb)
	return function()
		return rawLog:canOutput(verb)
	end
end

local function generateOutputFunction(scriptGlobals, rawLog, verb)
	return function(...)
		local inspect = scriptGlobals.require("inspect")

		local args = { ... }
		for index, arg in ipairs(args) do
			args[index] = inspect(arg)
		end

		rawLog:output(verb, table_concat(args, "\t"))
	end
end

local function wrapScriptGlobalsLog(scriptGlobals)
	local rawLog = scriptGlobals.log

	local function index(self, k)
		if not self._generated then
			rawset(self, "canTrace", generateCanOutputFunction("Trace"))
			rawset(self, "canDebug", generateCanOutputFunction("Debug"))
			rawset(self, "canInfo", generateCanOutputFunction("Info"))
			rawset(self, "canWarn", generateCanOutputFunction("Warn"))
			rawset(self, "canError", generateCanOutputFunction("Error"))
			rawset(self, "trace", generateOutputFunction("Trace"))
			rawset(self, "debug", generateOutputFunction("Debug"))
			rawset(self, "info", generateOutputFunction("Info"))
			rawset(self, "warn", generateOutputFunction("Warn"))
			rawset(self, "error", generateOutputFunction("Error"))

			self._generated = true

			if self[k] ~= nil then
				return self[k]
			end
		end

		return rawLog[k]
	end

	scriptGlobals.log = setmetatable({
		_generated = true,
	}, {
		__index = index,
	})
end

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
