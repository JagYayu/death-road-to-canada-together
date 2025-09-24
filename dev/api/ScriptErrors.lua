--- @meta
error("this is a lua library module")

--- @class TE.ScriptErrors
TE.scriptErrors = {}

--- Import from engine
function TE.scriptErrors.addLoadtimeError(scriptID, traceback) end

--- Import from engine
function TE.scriptErrors.addRuntimeError() end
