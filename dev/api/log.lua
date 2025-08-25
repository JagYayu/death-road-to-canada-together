--- @meta
error("this is a lua library module")

--- Import from cpp
--- @enum Log.ELogVerbosity
ELogVerbosity = {
	All = 255,
	Debug = 16,
	Error = 2,
	Fatal = 1,
	Info = 8,
	None = 0,
	Trace = 32,
	Warn = 4,
}

--- Import from cpp
--- @class Log
log = {}

--- Import from lua
--- @return boolean
function log.canDebug(...) end

--- Import from lua
--- @return boolean
function log.canInfo(...) end

--- Import from lua
--- @return boolean
function log.canTrace(...) end

--- Import from lua
--- @return boolean
function log.canWarn(...) end

--- Import from lua
--- @param ... any
function log.debug(...) end

--- Import from lua
--- @param ... any
function log.info(...) end

--- Import from lua
--- @param ... any
function log.trace(...) end

--- Import from lua
--- @param ... any
function log.warn(...) end

--- Import from cpp
--- @param verb string
--- @return boolean
function log:canOutput(verb) end

--- Import from cpp
--- @param verb string
--- @param ... any
function log:output(verb, ...) end
