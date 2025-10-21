--[[
-- @module ScriptEngine
-- @author JagYayu
-- @brief Core module for Tudov Engine
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local ffi = require("ffi")
local jit_util = require("jit.util")
local inspect = require("inspect")

local pcall = pcall
local type = type

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
	error("NOT IMPLEMENT YET", 2)

	if type(extraArgs) ~= "table" then
		extraArgs = estimateMemoryDefaultExtraArgs
	end

	estimateMemorySizeImpl(value, extraArgs)
end

--- @param mt metatable
local function lockMetatable(mt)
	if type(mt) == "table" then
		lockedTables[mt] = true
		--- @diagnostic disable-next-line: inject-field
		mt._TE_locked = true
	end
end

local function getUserdataTypeIDImpl(userdata)
	return userdata.getTypeID()
end

local function initialize(luaGlobals)
	jit.off()
	-- jit.opt.start("maxtrace=8000", "maxrecord=16000", "maxmcode=40960")

	--- @type TE.TudovEngine
	local TE = luaGlobals.TE

	--- @param value userdata | any?
	--- @return TE.TypeID?
	function TE.getUserdataTypeID(value)
		if type(value) == "userdata" then
			local success, result = pcall(getUserdataTypeIDImpl, value)
			return success and result or nil
		end
	end

	-- Lock all core metatables
	for _, value in pairs(luaGlobals) do
		if type(value) == "table" then
			lockMetatable(debug.getmetatable(value))
		end
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

		function modGlobals.load(chunk, chunkname, mode, env)
			if type(env) ~= "table" then
				env = modGlobals
			end

			return luaGlobals.load(chunk, chunkname, mode, env)
		end

		function modGlobals.loadfile(filename, mode, env)
			error("This function is not available currently", 2)
		end

		function modGlobals.loadstring(text, chunkname)
			local func, errorMessage = luaGlobals.loadstring(text, chunkname)
			if func then
				luaGlobals.setfenv(func, modGlobals)
			end

			return func, errorMessage
		end

		--- @param tbl table
		--- @return metatable?
		function modGlobals.getmetatable(tbl)
			if type(tbl) ~= "table" then
				error(("bad argument #1 to 'table' (table expected, got %s)"):format(type(tbl)), 2)
			end

			local mt = luaGlobals.getmetatable(tbl)
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

			local lmt = luaGlobals.getmetatable(tbl)
			if lmt ~= nil and lockedTables[lmt] then
				error("metatable is locked, unmodifiable in sandboxed environment", 2)
			end

			return luaGlobals.setmetatable(tbl, mt)
		end
	else
		modGlobals.collectgarbage = luaGlobals.collectgarbage
		modGlobals.debug = luaGlobals.debug
		modGlobals.getfenv = luaGlobals.getfenv
		modGlobals.io = luaGlobals.io
		modGlobals.load = luaGlobals.load
		modGlobals.loadfile = luaGlobals.loadfile
		modGlobals.loadstring = luaGlobals.loadstring
		modGlobals.newproxy = luaGlobals.newproxy
		modGlobals.os = luaGlobals.os
		modGlobals.package = luaGlobals.package
		modGlobals.setfenv = luaGlobals.setfenv
		modGlobals["jit.profile"] = luaGlobals["jit.profile"]
		modGlobals["jit.util"] = luaGlobals["jit.util"]
	end
end

local function generateCanOutputFunction(getLog, verb)
	return function()
		return getLog():canOutput(verb)
	end
end

local function generateOutputFunction(getLog, verb)
	return function(...)
		local log = getLog()
		if not log:canOutput(verb) then
			return
		end

		local args = { ... }
		for index, arg in ipairs(args) do
			args[index] = inspect.inspect(arg)
		end

		log:output(verb, args)
	end
end

local function wrapScriptGlobalsLog(scriptGlobals)
	local rawGetLog = scriptGlobals.log
	assert(type(rawGetLog) == "function")

	local metatable = {}

	local function getLog()
		local log = metatable[metatable]
		if not log then
			log = assert(rawGetLog())
			metatable[metatable] = log
		end

		return log
	end

	metatable.__call = getLog

	function metatable.__index(self, k)
		local ELogVerbosity = scriptGlobals.ELogVerbosity

		rawset(self, "canError", generateCanOutputFunction(getLog, ELogVerbosity.Error))
		rawset(self, "canWarn", generateCanOutputFunction(getLog, ELogVerbosity.Warn))
		rawset(self, "canInfo", generateCanOutputFunction(getLog, ELogVerbosity.Info))
		rawset(self, "canDebug", generateCanOutputFunction(getLog, ELogVerbosity.Debug))
		rawset(self, "canTrace", generateCanOutputFunction(getLog, ELogVerbosity.Trace))
		rawset(self, "error", generateOutputFunction(getLog, ELogVerbosity.Error))
		rawset(self, "warn", generateOutputFunction(getLog, ELogVerbosity.Warn))
		rawset(self, "info", generateOutputFunction(getLog, ELogVerbosity.Info))
		rawset(self, "debug", generateOutputFunction(getLog, ELogVerbosity.Debug))
		rawset(self, "trace", generateOutputFunction(getLog, ELogVerbosity.Trace))

		function metatable.__index(_, k1)
			return getLog()[k1]
		end

		if self[k] ~= nil then
			return self[k]
		end

		return getLog()[k]
	end

	lockMetatable(metatable)

	scriptGlobals.log = setmetatable({}, metatable)
end

--- @param scriptID integer
--- @param scriptName string
--- @param modUID string
--- @param sandboxed boolean
--- @param func function
--- @param scriptGlobals table
--- @param luaGlobals _G
local function postProcessScriptGlobals(scriptID, scriptName, modUID, sandboxed, func, scriptGlobals, luaGlobals)
	assert(rawget(scriptGlobals, "scriptID") == nil, "`_G.scriptID` is not nil")
	assert(rawget(scriptGlobals, "scriptName") == nil, "`_G.scriptName` is not nil")

	rawset(scriptGlobals, "scriptID", scriptID)
	rawset(scriptGlobals, "scriptName", scriptName)

	wrapScriptGlobalsLog(scriptGlobals)
end

return {
	estimateMemorySize = estimateMemorySize,
	initialize = initialize,
	lockMetatable = lockMetatable,
	postProcessModGlobals = postProcessModGlobals,
	postProcessScriptGlobals = postProcessScriptGlobals,
}
