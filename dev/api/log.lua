--- @meta
error("this is a lua library module")

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

--- @class Log
log = {}

--- @param ... any
function log.debug(...) end

--- @param ... any
function log.error(...) end

--- @param ... any
function log.info(...) end

--- @param ... any
function log.trace(...) end

--- @param ... any
function log.warn(...) end

--- @param verb string
--- @return boolean
function log:canOutput(verb) end

--- @param verb string
--- @param ... any
function log:output(verb, ...) end
