--- @meta
error("this is a lua library module")

--- @class TE.ScriptLoader
local scriptLoader

--- Link script dependency
--- e.g. "A.lua" has script `require target "B.lua"`, "B.lua" procession will also affect "A.lua".
--- @param source string | integer | nil
--- @param target string | integer
function scriptLoader:addReverseDependency(source, target) end

--- 获取当前正在加载的脚本ID，若没有则返回`0`
--- @return integer | 0
function scriptLoader:getLoadingScriptID() end

--- 获取当前正在加载的脚本名，若没有则返回`""`
--- @return string | ""
function scriptLoader:getLoadingScriptName() end

TE.scriptLoader = scriptLoader
