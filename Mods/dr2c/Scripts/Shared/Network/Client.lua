--[[
-- @module dr2c.Shared.Network.Client
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

local GNetworkPlatform = require("dr2c.Shared.Network.Platform")

--- @alias dr2c.ClientAttributeTrait dr2c.GNetworkClient.AttributeTrait

--- @alias dr2c.NetworkClientPublicAttribute dr2c.GNetworkClient.PublicAttribute

--- @alias dr2c.NetworkClientPrivateAttribute dr2c.GNetworkClient.PrivateAttribute

--- @alias dr2c.ClientPermission dr2c.GNetworkClient.Permission

--- @class dr2c.GClient
local GNetworkClient = {}

GNetworkClient.State = Enum.sequence({
	--- 断开连接
	Disconnected = 0,
	--- 验证阶段
	Verifying = 1,
	--- 已验证
	Verified = 2,
	--- 假客户端
	Dummy = 3,
})

local State = GNetworkClient.State

GNetworkClient.AttributeTrait = Enum.immutable({
	-- 客户端可随时修改该属性（默认值）
	Editable = 0,
	-- 客户端只能在验证时期修改
	Validation = 1,
	-- 客户端不允许修改
	Authoritative = 2,
})

local AttributeTrait = GNetworkClient.AttributeTrait

GNetworkClient.PublicAttribute = Enum.sequence({
	-- 存储ClientID
	ID = 0,
	-- 当前客户端状态
	State = 1,
	-- 许可
	Permissions = 2,
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
	-- App版本
	Version = 9,
	-- 游戏文件哈希值，用于判断游戏的核心文件是否和服务器一致
	ContentHash = 10,
	-- 游戏Mods哈希值，用于判断模组是否和服务器一致
	ModsHash = 11,
	-- 套接口类型
	SocketType = 12,
	-- 是否含有客户端模组
	HasClientOnlyMods = 13,
})

local PublicAttribute = GNetworkClient.PublicAttribute

GNetworkClient.PrivateAttribute = Enum.sequence({
	SecretToken = 1,
	SecretStatistics = 2,
})

local PrivateAttribute = GNetworkClient.PrivateAttribute

--- @enum dr2c.GNetworkClient.Permission
GNetworkClient.Permission = { -- TODO Enum.bits
	Authority = bit.lshift(1, 0),
	Chat = bit.lshift(1, 1),
	KickClient = bit.lshift(1, 2),
	BanClient = bit.lshift(1, 3),
	ChangeSetting = bit.lshift(1, 4),
	ChangeAttribute = bit.lshift(1, 5),
	ServerCommand = bit.lshift(1, 6),
}

local Permission = GNetworkClient.Permission
local permissionsDefault = Permission.Chat
local permissionsAdmin = bit.bor(permissionsDefault, Permission.KickClient, Permission.ChangeSetting)
local permissionsHost = bit.bor(
	permissionsAdmin,
	Permission.Authority,
	Permission.BanClient,
	Permission.ChangeAttribute,
	Permission.ServerCommand
)

GNetworkClient.Permissions = Enum.immutable({
	None = 0,
	Default = permissionsDefault,
	Admin = permissionsAdmin,
	Host = permissionsHost,
})

local Permissions = GNetworkClient.Permissions

--- @generic TPublic : table<dr2c.NetworkClientPublicAttribute, any>
--- @generic TPrivate : table<dr2c.NetworkClientPrivateAttribute, any>
--- @param clientID any
--- @param publicAttributes TPublic
--- @param privateAttributes TPrivate
--- @return TPublic publicAttributes
--- @return TPrivate privateAttributes
function GNetworkClient.initializeClientAttributes(clientID, publicAttributes, privateAttributes)
	publicAttributes[PublicAttribute.ID] = clientID

	publicAttributes[PublicAttribute.State] = State.Disconnected
	publicAttributes[PublicAttribute.Permissions] = Permissions.None
	publicAttributes[PublicAttribute.Platform] = GNetworkPlatform.Type.Standalone
	publicAttributes[PublicAttribute.PlatformID] = nil
	publicAttributes[PublicAttribute.OperatingSystem] = EOperatingSystem.Unknown
	publicAttributes[PublicAttribute.Statistics] = {}
	publicAttributes[PublicAttribute.DisplayName] = ""
	publicAttributes[PublicAttribute.Room] = 0
	publicAttributes[PublicAttribute.Version] = ""
	publicAttributes[PublicAttribute.ContentHash] = 0
	publicAttributes[PublicAttribute.ModsHash] = 0
	publicAttributes[PublicAttribute.SocketType] = ESocketType.Local
	publicAttributes[PublicAttribute.HasClientOnlyMods] = false

	privateAttributes[PrivateAttribute.SecretStatistics] = {}
	privateAttributes[PrivateAttribute.SecretToken] = nil

	return publicAttributes, privateAttributes
end

local publicAttributeTraits = {
	[PublicAttribute.ID] = AttributeTrait.Authoritative,
	[PublicAttribute.Permissions] = AttributeTrait.Authoritative,
	[PublicAttribute.State] = AttributeTrait.Validation,
	[PublicAttribute.Platform] = AttributeTrait.Validation,
	[PublicAttribute.PlatformID] = AttributeTrait.Validation,
	[PublicAttribute.OperatingSystem] = AttributeTrait.Validation,
	[PublicAttribute.Room] = AttributeTrait.Authoritative,
	[PublicAttribute.Version] = AttributeTrait.Validation,
	[PublicAttribute.ContentHash] = AttributeTrait.Validation,
	[PublicAttribute.ModsHash] = AttributeTrait.Validation,
}

local privateAttributeTraits = {
	[PrivateAttribute.SecretToken] = AttributeTrait.Authoritative,
}

publicAttributeTraits = persist("publicAttributeTraits", function()
	return publicAttributeTraits
end)
privateAttributeTraits = persist("privateAttributeTraits", function()
	return privateAttributeTraits
end)

--- @param attribute dr2c.NetworkClientPublicAttribute
--- @return dr2c.ClientAttributeTrait trait
function GNetworkClient.getPublicAttributeTrait(attribute)
	return publicAttributeTraits[attribute] or GNetworkClient.AttributeTrait.Editable
end

--- @param attribute dr2c.NetworkClientPrivateAttribute
--- @return dr2c.ClientAttributeTrait trait
function GNetworkClient.getPrivateAttributeTrait(attribute)
	return privateAttributeTraits[attribute] or GNetworkClient.AttributeTrait.Editable
end

--- @type table<dr2c.NetworkClientPublicAttribute, fun(value: any): boolean?>
local publicAttributeValidators = {
	[PublicAttribute.ID] = Function.isTypeInteger,
	[PublicAttribute.State] = Function.isTypeInteger,
	[PublicAttribute.Platform] = Function.isTypeNumber,
	[PublicAttribute.PlatformID] = Function.isTypeStringOrNil,
	[PublicAttribute.Statistics] = Function.isTypeTable,
	[PublicAttribute.DisplayName] = Function.isTypeString,
	[PublicAttribute.Room] = Function.isTypeInteger,
	[PublicAttribute.Version] = Function.isTypeString,
	[PublicAttribute.ContentHash] = Function.isTypeNumber,
	[PublicAttribute.ModsHash] = Function.isTypeNumber,
}

--- @type table<dr2c.NetworkClientPrivateAttribute, dr2c.ValueValidator>
local privateAttributeValidators = {
	[PrivateAttribute.SecretToken] = Function.isTypeNumberOrNil,
	[PrivateAttribute.SecretStatistics] = Function.isTypeTable,
}

--- @param attribute dr2c.NetworkClientPublicAttribute
--- @param value integer | string
--- @return boolean
function GNetworkClient.validatePublicAttribute(attribute, value)
	local validator = publicAttributeValidators[attribute]
	if validator then
		return not not validator(value)
	else
		return true
	end
end

--- @param attribute dr2c.NetworkClientPrivateAttribute
--- @param value integer | string
--- @return boolean
function GNetworkClient.validatePrivateAttribute(attribute, value)
	local validator = privateAttributeValidators[attribute]
	if validator then
		return not not validator(value)
	else
		return true
	end
end

--- @param attribute dr2c.NetworkClientPublicAttribute
--- @param validator dr2c.ValueValidator
function GNetworkClient.setPublicAttributeValidator(attribute, validator)
	publicAttributeValidators[attribute] = validator
end

--- @param attribute dr2c.NetworkClientPrivateAttribute
--- @param validator dr2c.ValueValidator
function GNetworkClient.setPrivateAttributeValidator(attribute, validator)
	privateAttributeValidators[attribute] = validator
end

return GNetworkClient
