--[[
-- @module dr2c.shared.world.Session
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("tudov.Enum")
local Function = require("tudov.Function")

--- @alias dr2c.WorldSessionAttribute dr2c.GWorldSession.Attribute

--- @class dr2c.WorldSessionAttribute.Internal
--- @field state dr2c.WorldSessionState
--- @field timeStart number
--- @field timePaused number
--- @field elapsedPaused number

--- @alias dr2c.WorldSessionMode dr2c.GWorldSession.Mode

--- @alias dr2c.WorldSessionState dr2c.GWorldSession.State

--- @class dr2c.GWorldSession
local GWorldSession = {}

GWorldSession.Attribute = Enum.sequence({
	Internal = 0,
	Mode = 1,
})

local GWorldSession_Attribute_Internal = GWorldSession.Attribute.Internal

GWorldSession.Mode = Enum.sequence({
	None = 0,
	Default = 1,
})

local GWorldSession_Attribute_Mode = GWorldSession.Attribute.Mode

GWorldSession.State = Enum.immutable({
	Inactive = 0,
	Playing = 1,
	Paused = 2,
})

local GWorldSession_State_Paused = GWorldSession.State.Paused
local GWorldSession_State_Playing = GWorldSession.State.Playing

--- @type table<string, dr2c.WorldSession>
local worldSessionModules = setmetatable({}, { __mode = "v" })

--- @type table<dr2c.WorldSessionAttribute, dr2c.ValueValidator>
local attributeValidators = {
	[GWorldSession.Attribute.Internal] = Function.generateTableValidator({
		{ "state", "number" },
		{ "timeStart", "number" },
		{ "timePaused", "number" },
		{ "elapsedPaused", "number" },
	}),
}

worldSessionModules = persist("worldSessionModules", function()
	return worldSessionModules
end)
attributeValidators = persist("attributeValidators", function()
	return attributeValidators
end)

--- @param attribute dr2c.WorldSessionAttribute
--- @param value any
--- @return boolean
function GWorldSession.validateAttribute(attribute, value)
	local validate = attributeValidators[attribute]
	if validate then
		return not not validate(value)
	else
		return true
	end
end

--- @param attribute dr2c.WorldSessionAttribute
--- @param validate dr2c.ValueValidator
function GWorldSession.setAttributeValidator(attribute, validate)
	attributeValidators[attribute] = validate
end

--- @return dr2c.WorldSession worldSessionModule
function GWorldSession.new()
	local scriptName = scriptLoader:getLoadingScriptName()
	if scriptName ~= "" and worldSessionModules[scriptName] then
		return worldSessionModules[scriptName]
	end

	--- @class dr2c.WorldSession
	local WorldSession = {}

	worldSessionModules[scriptName] = WorldSession

	--- @class dr2c.WorldSessionAttributes
	local worldSessionAttributes

	function WorldSession.getAttributes()
		return worldSessionAttributes
	end

	--- @param attribute dr2c.WorldSessionAttribute
	--- @return any
	function WorldSession.getAttribute(attribute)
		return worldSessionAttributes[attribute]
	end

	--- @param attribute dr2c.WorldSessionAttribute
	--- @param value any
	function WorldSession.setAttribute(attribute, value)
		if not GWorldSession.validateAttribute(value) then
			error("Attribute value is not valid", 2)
		end

		worldSessionAttributes[attribute] = value
	end

	function WorldSession.resetAttributes()
		worldSessionAttributes = {
			[GWorldSession_Attribute_Internal] = {
				state = GWorldSession.State.Inactive,
				timeStart = Time.getSystemTime(),
				timePaused = Time.getSystemTime(),
				elapsedPaused = 0,
			},
			[GWorldSession_Attribute_Mode] = GWorldSession.Mode.None,
		}
	end

	--- Equivalent to `WorldSession.getAttribute(GWorldSession.Attribute.Mode)`
	--- @return dr2c.WorldSessionMode
	function WorldSession.getMode()
		return worldSessionAttributes[GWorldSession_Attribute_Mode].mode
	end

	--- Equivalent to `WorldSession.getAttribute(GWorldSession.Attribute.Internal).state`
	--- @return dr2c.WorldSessionMode
	function WorldSession.getState()
		return worldSessionAttributes[GWorldSession_Attribute_Internal].state
	end

	WorldSession.resetAttributes()

	return WorldSession
end

events:add("DebugSnapshot", function(e)
	e.worldSessionModules = worldSessionModules
end, scriptName, nil, scriptName)

return GWorldSession
