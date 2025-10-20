--[[
-- @module dr2c.Shared.World.Session
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
	-- Framework
	-- 框架

	--- 当前游戏状态
	State = 1,
	--- 开始的时间
	TimeStart = 2,
	--- 暂停的时间
	TimePaused = 3,
	--- 总暂停时间
	ElapsedPaused = 4,
	--- 数据寿命，影响快照、玩家输入的缓存，超过最远部分会被丢弃。单位为时间秒，默认10
	DataLifetime = 5,
	--- 回滚限度，用于确定回滚的预期最远距离，服务器会忽略过于遥远的用户输入。单位为时间秒，默认1
	RollbackLimit = 6,

	-- Gameplay
	-- 玩法

	--- 当前关卡
	Level = 11,
	--- 关卡内所有的场景
	Scenes = 12,
	--- 游戏模式
	Mode = 13,
	--- 所有瓦片地图
	TileMaps = 14,
	--- 僵尸数量乘数
	ZombieMultiplier = 15,
})

local GWorldSession_Attribute_State = GWorldSession.Attribute.State

GWorldSession.Mode = Enum.sequence({
	None = 0,
	DeathRoad = 1,
})

local GWorldSession_Attribute_Mode = GWorldSession.Attribute.Mode

GWorldSession.State = Enum.immutable({
	Inactive = 0,
	Playing = 1,
	Paused = 2,
})

local GWorldSession_State_Inactive = GWorldSession.State.Inactive
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
	TE.scriptLoader:addReverseDependency(TE.scriptLoader:getLoadingScriptID(), scriptID)

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
	--- @field [dr2c.WorldSessionAttribute] any
	local worldSessionAttributes

	function WorldSession.resetAttributes()
		worldSessionAttributes = {
			[GWorldSession_Attribute_State] = GWorldSession_State_Inactive,
			[GWorldSession.Attribute.TimeStart] = Time.getSystemTime(),
			[GWorldSession.Attribute.TimePaused] = Time.getSystemTime(),
			[GWorldSession.Attribute.ElapsedPaused] = 0,
			[GWorldSession.Attribute.Scenes] = {},
			[GWorldSession.Attribute.Mode] = GWorldSession.Mode.None,
			[GWorldSession.Attribute.DataLifetime] = 10,
			[GWorldSession.Attribute.RollbackLimit] = 1,
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

	--- A short to `WorldSession.getAttribute(GWorldSession.Attribute.State) == GWorldSession.State.Inactive`
	--- @return boolean
	--- @nodiscard
	function WorldSession.isInactive()
		return worldSessionAttributes[GWorldSession_Attribute_State] == GWorldSession_State_Inactive
	end

	--- A short to `WorldSession.getAttribute(GWorldSession.Attribute.State) == GWorldSession.State.Playing`
	--- @return boolean
	--- @nodiscard
	function WorldSession.isPlaying()
		return worldSessionAttributes[GWorldSession_Attribute_State] == GWorldSession_State_Playing
	end

	--- A short to `WorldSession.getAttribute(GWorldSession.Attribute.State) == GWorldSession.State.Paused`
	--- @return boolean
	--- @nodiscard
	function WorldSession.isPaused()
		return worldSessionAttributes[GWorldSession_Attribute_State] == GWorldSession_State_Paused
	end

	WorldSession.resetAttributes()

	return WorldSession
end

TE.events:add("DebugSnapshot", function(e)
	e.worldSessionModules = worldSessionModules
end, scriptName, nil, scriptName)

return GWorldSession
