--- @meta
error("this is a lua library module")

--- @class TE.ScriptProvider
TE.scriptProvider = {}

--- @param scriptName string
--- @return integer
function TE.scriptProvider:getScriptIDByName(scriptName) end

--- @param scriptID integer
--- @return string
function TE.scriptProvider:getScriptNameByID(scriptID) end
