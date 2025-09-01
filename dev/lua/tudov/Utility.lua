local Number = require("tudov.Number")

local indexNan = Number.nan

--- @class Utility
local Utility = {}

--- @param levelOverride integer? @default: 3
function Utility.assertLoadtime(levelOverride)
	if scriptLoader:getLoadingScriptID() == 0 then
		error("Function must be called in script run time!", levelOverride or 3)
	end
end

--- @param levelOverride integer? @default: 3
function Utility.assertRuntime(levelOverride)
	if scriptLoader:getLoadingScriptID() ~= 0 then
		error("Function must be called in script run time!", levelOverride or 3)
	end
end

--- Check if the key can be a nice index
--- Only `nil` and `nan` values are not valid in LuaJIT.
--- @param value any?
--- @return boolean
function Utility.canBeIndex(value)
	return value ~= nil and value ~= indexNan
end

return Utility
