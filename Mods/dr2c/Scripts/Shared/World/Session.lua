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
local Table = require("TE.Table")
local Utility = require("TE.Utility")

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

	--- 当前会话ID
	ID = 0,
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

local GWorldSession_Attribute_ID = GWorldSession.Attribute.ID
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

--- @type table<string, dr2c.MWorldSession>
local modules = setmetatable({}, { __mode = "v" })

--- @type table<dr2c.WorldSessionAttribute, { default: any, validate: dr2c.ValueValidator }>
local attributeProperties = {
	[GWorldSession_Attribute_ID] = {
		validate = Function.isTypeNumber,
	},
	[GWorldSession_Attribute_State] = {
		default = GWorldSession_State_Inactive,
		validate = Function.generateIsEnumValue(GWorldSession.State),
	},
	[GWorldSession.Attribute.TimeStart] = {
		default = Time.getSystemTime,
		validate = Function.isTypeNumber,
	},
	[GWorldSession.Attribute.TimePaused] = {
		default = Time.getSystemTime,
		validate = Function.isTypeNumber,
	},
	[GWorldSession.Attribute.ElapsedPaused] = {
		default = 0,
		validate = Function.isTypeNumber,
	},
	[GWorldSession.Attribute.Scenes] = {
		default = {},
		validate = Function.isTypeTable,
	},
	[GWorldSession.Attribute.Mode] = {
		default = GWorldSession.Mode.None,
		validate = Function.generateIsEnumValue(GWorldSession.Mode),
	},
	[GWorldSession.Attribute.DataLifetime] = {
		default = 10,
		validate = Function.isTypeNumber,
	},
	[GWorldSession.Attribute.RollbackLimit] = {
		default = 1,
		validate = Function.isTypeNumber,
	},
}

modules = persist("modules", function()
	return modules
end)
attributeProperties = persist("attributeValidators", function()
	return attributeProperties
end)

--- @param attribute dr2c.WorldSessionAttribute
--- @param value any
--- @return boolean
function GWorldSession.validateAttribute(attribute, value)
	local property = attributeProperties[attribute]
	if property then
		return not not property.validate(value)
	else
		return true
	end
end

--- @warn 必须确保默认值能通过验证器的验证
--- @param attribute dr2c.WorldSessionAttribute
--- @param validate dr2c.ValueValidator
function GWorldSession.setAttributeProperty(attribute, default, validate)
	attributeProperties[attribute] = {
		default = default,
		validate = validate or Function.alwaysTrue,
	}
end

--- @return dr2c.MWorldSession module
function GWorldSession.new(roomID)
	--- @class dr2c.MWorldSession
	local MWorldSession = {}

	--- @class dr2c.WorldSessionAttributes
	--- @field [dr2c.WorldSessionAttribute] any
	local worldSessionAttributes

	Utility.persistModule(modules, function()
		return worldSessionAttributes
	end, function(t)
		worldSessionAttributes = t
	end, roomID)

	function MWorldSession.resetAttributes()
		worldSessionAttributes = {}

		for attribute, property in pairs(attributeProperties) do
			if type(property.default) == "table" then
				worldSessionAttributes[attribute] = Table.fastCopy(property.default)
			elseif type(property.default) == "function" then
				worldSessionAttributes[attribute] = property.default()
			elseif property.default ~= nil then
				worldSessionAttributes[attribute] = property.default
			end
		end
	end

	--- @return dr2c.WorldSessionAttributes
	function MWorldSession.getAttributes()
		return worldSessionAttributes
	end

	--- @param attributes dr2c.WorldSessionAttributes
	function MWorldSession.setAttributes(attributes)
		worldSessionAttributes = attributes
	end

	--- @param attribute dr2c.WorldSessionAttribute
	--- @return any
	function MWorldSession.getAttribute(attribute)
		return worldSessionAttributes[attribute]
	end

	--- @param attribute dr2c.WorldSessionAttribute
	--- @param value any
	function MWorldSession.setAttribute(attribute, value)
		if not GWorldSession.validateAttribute(value) then
			error("Attribute value is not valid", 2)
		end

		worldSessionAttributes[attribute] = value
	end

	--- A short to `WorldSession.getAttribute(GWorldSession.Attribute.ID)`
	--- @return integer
	--- @nodiscard
	function MWorldSession.getSessionID()
		return worldSessionAttributes[GWorldSession_Attribute_ID]
	end

	function MWorldSession.nextSessionID()
		worldSessionAttributes[GWorldSession_Attribute_ID] = worldSessionAttributes[GWorldSession_Attribute_ID] + 1
	end

	--- A short to `WorldSession.getAttribute(GWorldSession.Attribute.State) == GWorldSession.State.Inactive`
	--- @return boolean
	--- @nodiscard
	function MWorldSession.isInactive()
		return worldSessionAttributes[GWorldSession_Attribute_State] == GWorldSession_State_Inactive
	end

	--- A short to `WorldSession.getAttribute(GWorldSession.Attribute.State) == GWorldSession.State.Playing`
	--- @return boolean
	--- @nodiscard
	function MWorldSession.isPlaying()
		return worldSessionAttributes[GWorldSession_Attribute_State] == GWorldSession_State_Playing
	end

	--- A short to `WorldSession.getAttribute(GWorldSession.Attribute.State) == GWorldSession.State.Paused`
	--- @return boolean
	--- @nodiscard
	function MWorldSession.isPaused()
		return worldSessionAttributes[GWorldSession_Attribute_State] == GWorldSession_State_Paused
	end

	MWorldSession.resetAttributes()

	return MWorldSession
end

TE.events:add("DebugSnapshot", function(e)
	e.worldSessionModules = modules
end, scriptName, nil, scriptName)

return GWorldSession
