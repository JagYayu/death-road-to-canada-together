--[[
-- @module dr2c.shared.network.Client
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

--- @alias dr2c.ClientPublicAttribute dr2c.GClient.PublicAttribute

--- @alias dr2c.ClientPrivateAttribute dr2c.GClient.PrivateAttribute

--- @alias dr2c.ClientPermission dr2c.GClient.Permission

--- @class dr2c.GClient
local GClient = {}

GClient.State = Enum.sequence({
	Disconnected = 0,
	Verifying = 1,
	Verified = 2,
})

GClient.AttributeTrait = Enum.immutable({
	-- 客户端可随时修改该属性（默认值）
	Editable = 0,
	-- 客户端只能在验证时期修改
	Validation = 1,
	-- 客户端不允许修改
	Authoritative = 2,
})

GClient.PublicAttribute = Enum.sequence({
	-- 存储ClientID
	ID = 0,
	-- 许可
	Permissions = 1,
	-- 当前连接状态
	State = 2,
	-- 游戏平台
	Platform = 3,
	-- 游戏平台账户ID
	PlatformID = 4,
	-- 操作系统
	OperatingSystem = 5,
	-- 账户资料
	Statistics = 6,
	-- 昵称（覆盖账户名）
	DisplayName = 7,
	-- 所在服务器房间
	Room = 8,
	-- app版本
	Version = 9,
	-- 游戏文件哈希值，用于判断游戏的核心文件是否和服务器一致
	ContentHash = 10,
	-- 游戏模组哈希值，用于判断模组是否和服务器一致
	ModsHash = 11,
	-- 套接口类型
	SocketType = 12,
	-- 是否含有客户端模组
	HasClientOnlyMods = 13,
})

GClient.PrivateAttribute = Enum.sequence({
	SecretToken = 1,
	SecretStatistics = 2,
})

--- @enum dr2c.GClient.Permission
GClient.Permission = { -- TODO Enum.bits
	Authority = bit.lshift(1, 0),
	Chat = bit.lshift(1, 1),
	KickClient = bit.lshift(1, 2),
	BanClient = bit.lshift(1, 3),
	ChangeSetting = bit.lshift(1, 4),
	ChangeAttribute = bit.lshift(1, 5),
	ServerCommand = bit.lshift(1, 6),
}

local Permission = GClient.Permission
local permissionsDefault = Permission.Chat
local permissionsAdmin = bit.bor(permissionsDefault, Permission.KickClient, Permission.ChangeSetting)
local permissionsHost = bit.bor(
	permissionsAdmin,
	Permission.Authority,
	Permission.BanClient,
	Permission.ChangeAttribute,
	Permission.ServerCommand
)

GClient.Permissions = Enum.sequence({
	Default = permissionsDefault,
	Admin = permissionsAdmin,
	Host = permissionsHost,
})

local publicAttributeTraits = {
	[GClient.PublicAttribute.ID] = GClient.AttributeTrait.Authoritative,
	[GClient.PublicAttribute.Permissions] = GClient.AttributeTrait.Authoritative,
	[GClient.PublicAttribute.State] = GClient.AttributeTrait.Validation,
	[GClient.PublicAttribute.Platform] = GClient.AttributeTrait.Validation,
	[GClient.PublicAttribute.PlatformID] = GClient.AttributeTrait.Validation,
	[GClient.PublicAttribute.OperatingSystem] = GClient.AttributeTrait.Validation,
	[GClient.PublicAttribute.Room] = GClient.AttributeTrait.Authoritative,
	[GClient.PublicAttribute.Version] = GClient.AttributeTrait.Validation,
	[GClient.PublicAttribute.ContentHash] = GClient.AttributeTrait.Validation,
	[GClient.PublicAttribute.ModsHash] = GClient.AttributeTrait.Validation,
}

local privateAttributeTraits = {
	[GClient.PrivateAttribute.SecretToken] = GClient.AttributeTrait.Authoritative,
}

publicAttributeTraits = persist("publicAttributeTraits", function()
	return publicAttributeTraits
end)
privateAttributeTraits = persist("privateAttributeTraits", function()
	return privateAttributeTraits
end)

--- @param attribute dr2c.GClient.PublicAttribute
--- @return dr2c.GClient.AttributeTrait trait
function GClient.getPublicAttributeTrait(attribute)
	return publicAttributeTraits[attribute] or GClient.AttributeTrait.Editable
end

--- @param attribute dr2c.GClient.PrivateAttribute
--- @return dr2c.GClient.AttributeTrait trait
function GClient.getPrivateAttributeTrait(attribute)
	return privateAttributeTraits[attribute] or GClient.AttributeTrait.Editable
end

--- @type table<dr2c.GClient.PublicAttribute, fun(value: any): boolean?>
local publicAttributeValidators = {
	[GClient.PublicAttribute.ID] = Function.isTypeInteger,
	[GClient.PublicAttribute.State] = Function.isTypeInteger,
	[GClient.PublicAttribute.Platform] = Function.isTypeNumber,
	[GClient.PublicAttribute.PlatformID] = Function.isTypeStringOrNil,
	[GClient.PublicAttribute.Statistics] = Function.isTypeTable,
	[GClient.PublicAttribute.DisplayName] = Function.isTypeStringOrNil,
	[GClient.PublicAttribute.Room] = Function.isTypeIntegerOrNil,
	[GClient.PublicAttribute.Version] = Function.isTypeString,
	[GClient.PublicAttribute.ContentHash] = Function.isTypeNumber,
	[GClient.PublicAttribute.ModsHash] = Function.isTypeNumber,
}

--- @type table<dr2c.GClient.PrivateAttribute, dr2c.ValueValidator>
local privateAttributeValidators = {
	[GClient.PrivateAttribute.SecretToken] = Function.isTypeNumberOrNil,
	[GClient.PrivateAttribute.SecretStatistics] = Function.isTypeTable,
}

--- @param attribute dr2c.GClient.PublicAttribute
--- @param value integer | string
--- @return boolean
function GClient.validatePublicAttribute(attribute, value)
	local validator = publicAttributeValidators[attribute]
	if validator then
		return not not validator(value)
	else
		return true
	end
end

--- @param attribute dr2c.GClient.PrivateAttribute
--- @param value integer | string
--- @return boolean
function GClient.validatePrivateAttribute(attribute, value)
	local validator = privateAttributeValidators[attribute]
	if validator then
		return not not validator(value)
	else
		return true
	end
end

--- @param attribute dr2c.ClientPublicAttribute
--- @param validator dr2c.ValueValidator
function GClient.setPublicAttributeValidator(attribute, validator)
	publicAttributeValidators[attribute] = validator
end

--- @param attribute dr2c.ClientPrivateAttribute
--- @param validator dr2c.ValueValidator
function GClient.setPrivateAttributeValidator(attribute, validator)
	privateAttributeValidators[attribute] = validator
end

return GClient
