--[[
-- @module tudov.Utility
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class TE.Utility
local Utility = {}

Utility.inf = 1 / 0

Utility.nan = 0 / 0

--- @param levelOverride integer? @default: 3
function Utility.assertLoadtime(levelOverride)
	if TE.scriptLoader:getLoadingScriptID() == 0 then
		error("Function must be called in script run time!", levelOverride or 3)
	end
end

--- @param levelOverride integer? @default: 3
function Utility.assertRuntime(levelOverride)
	if TE.scriptLoader:getLoadingScriptID() ~= 0 then
		error("Function must be called in script run time!", levelOverride or 3)
	end
end

--- Check if the key can be a nice index
--- Only `nil` and `nan` values are not valid in LuaJIT.
--- @param value any?
--- @return boolean
function Utility.canBeIndex(value)
	return value ~= nil and value ~= Utility.nan
end

return Utility
