--- @meta
error("this is a lua library module")

--- @class TE.ScriptLoader
local scriptLoader

--- Link script dependency
--- e.g. "A.lua" has script `require target "B.lua"`, "B.lua" procession will also affect "A.lua".
--- @param source string | integer | nil
--- @param target string | integer
function scriptLoader:addReverseDependency(source, target) end

--- @return integer
function scriptLoader:getLoadingScriptID() end

--- @return string
function scriptLoader:getLoadingScriptName() end

TE.scriptLoader = scriptLoader
