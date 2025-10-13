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

local Math = require("TE.Math")

local type = type

--- @class TE.Utility
local Utility = {}

local nan = Math.nan

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
	return value ~= nil and value ~= nan
end

--- @generic T
--- @param value? T | fun(): T
--- @param default? T
--- @return T
function Utility.evaluate(value, default)
	if default == nil then
		if type(value) == "function" then
			return value()
		else
			return value
		end
	else
		local result

		if type(value) == "function" then
			result = value()
		else
			result = value
		end

		if type(result) ~= type(default) then
			return default
		else
			return result
		end
	end
end

return Utility
