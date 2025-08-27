--- @meta
error("this is a lua library module")

--- @class ScriptErrors
scriptErrors = {}

--- Import from engine
function scriptErrors.addLoadtimeError(scriptID, traceback) end

--- Import from engine
function scriptErrors.addRuntimeError() end
