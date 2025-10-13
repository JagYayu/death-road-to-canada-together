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

local Enum = require("TE.Enum")
local Function = require("TE.Function")

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
	State = 1,
	TimeStart = 2,
	TimePaused = 3,
	ElapsedPaused = 4,
	Mode = 5,
	Zombies = 6,
})

local GWorldSession_Attribute_State = GWorldSession.Attribute.State
local GWorldSession_Attribute_TimeStart = GWorldSession.Attribute.TimeStart
local GWorldSession_Attribute_TimePaused = GWorldSession.Attribute.TimePaused
local GWorldSession_Attribute_ElapsedPaused = GWorldSession.Attribute.ElapsedPaused

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
	-- [GWorldSession.Attribute.State] = Function.generateTableValidator({
	-- 	{ "state", "number" },
	-- 	{ "timeStart", "number" },
	-- 	{ "timePaused", "number" },
	-- 	{ "elapsedPaused", "number" },
	-- }),
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

--- @return dr2c.WorldSession WorldSession
function GWorldSession.new()
	--- @class dr2c.WorldSession
	local WorldSession

	local scriptName = TE.scriptLoader:getLoadingScriptName()
	if scriptName ~= "" then
		if worldSessionModules[scriptName] then
			return worldSessionModules[scriptName]
		else
			WorldSession = {}
			worldSessionModules[scriptName] = WorldSession
		end
	else
		WorldSession = {}
	end

	worldSessionModules[scriptName] = WorldSession

	--- @class dr2c.WorldSessionAttributes
	local worldSessionAttributes

	function WorldSession.resetAttributes()
		worldSessionAttributes = {
			[GWorldSession_Attribute_State] = GWorldSession.State.Inactive,
			[GWorldSession_Attribute_TimeStart] = Time.getSystemTime(),
			[GWorldSession_Attribute_TimePaused] = Time.getSystemTime(),
			[GWorldSession_Attribute_ElapsedPaused] = 0,
			[GWorldSession_Attribute_Mode] = GWorldSession.Mode.None,
		}
	end

	--- @return dr2c.WorldSessionAttributes
	function WorldSession.getAttributes()
		return worldSessionAttributes
	end

	--- @param attributes dr2c.WorldSessionAttributes
	function WorldSession.setAttributes(attributes)
		worldSessionAttributes = attributes
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

	WorldSession.resetAttributes()

	return WorldSession
end

TE.events:add("DebugSnapshot", function(e)
	e.worldSessionModules = worldSessionModules
end, scriptName, nil, scriptName)

return GWorldSession
