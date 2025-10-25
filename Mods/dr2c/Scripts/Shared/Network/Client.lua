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
local GUtilsAttribute = require("dr2c.Shared.Utils.Attribute")

--- @alias dr2c.ClientAttributeTrait dr2c.GNetworkClient.AttributeTrait

--- @alias dr2c.ClientPublicAttribute dr2c.GNetworkClient.PublicAttribute

--- @alias dr2c.ClientPrivateAttribute dr2c.GNetworkClient.PrivateAttribute

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

--- @generic TPublic : table<dr2c.ClientPublicAttribute, any>
--- @generic TPrivate : table<dr2c.ClientPrivateAttribute, any>
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

--- @param attribute dr2c.ClientPublicAttribute
--- @return dr2c.ClientAttributeTrait trait
function GNetworkClient.getPublicAttributeTrait(attribute)
	return publicAttributeTraits[attribute] or GNetworkClient.AttributeTrait.Editable
end

--- @param attribute dr2c.ClientPrivateAttribute
--- @return dr2c.ClientAttributeTrait trait
function GNetworkClient.getPrivateAttributeTrait(attribute)
	return privateAttributeTraits[attribute] or GNetworkClient.AttributeTrait.Editable
end

local proxyPublicAttributeProperties = GUtilsAttribute.newProperties({
	[PublicAttribute.ID] = {
		default = 0,
		validator = Function.isTypeInteger,
	},
	[PublicAttribute.State] = {
		default = State.Disconnected,
		validator = Function.isTypeInteger,
	},
	[PublicAttribute.Permissions] = {
		default = GNetworkClient.Permissions.None,
		validator = Function.isTypeInteger,
	},
	[PublicAttribute.Platform] = {
		default = GNetworkPlatform.Type.Standalone,
		validator = Function.isTypeNumber,
	},
	[PublicAttribute.PlatformID] = {
		validator = Function.isTypeStringOrNil,
	},
	[PublicAttribute.Statistics] = {
		default = {},
		validator = Function.isTypeTable,
	},
	[PublicAttribute.DisplayName] = {
		default = "",
		validator = Function.isTypeString,
	},
	[PublicAttribute.Room] = {
		validator = Function.isTypeIntegerOrNil,
	},
	[PublicAttribute.Version] = {
		validator = Function.isTypeString,
	},
	[PublicAttribute.ContentHash] = {
		validator = Function.isTypeNumber,
	},
	[PublicAttribute.ModsHash] = {
		validator = Function.isTypeNumber,
	},
})

local proxyPrivateAttributeProperties = GUtilsAttribute.newProperties({
	[PrivateAttribute.SecretToken] = {
		validator = Function.isTypeNumberOrNil,
	},
	[PrivateAttribute.SecretStatistics] = {
		validator = Function.isTypeTable,
	},
})

--- @type fun(attribute: dr2c.ClientPublicAttribute): dr2c.AttributeProperty
GNetworkClient.getPublicAttributeProperty = proxyPublicAttributeProperties.getAttributeProperty

--- @type fun(attribute: dr2c.ClientPublicAttribute, args: dr2c.AttributePropertyArgs)
GNetworkClient.setPublicAttributeProperty = proxyPublicAttributeProperties.setAttributeProperty

--- @type fun(attribute: dr2c.ClientPublicAttribute, value?: Serializable): boolean
GNetworkClient.validatePublicAttribute = proxyPublicAttributeProperties.validateAttribute

--- @type fun(attributes: table<dr2c.ClientPublicAttribute, Serializable>)
GNetworkClient.resetPublicAttributes = proxyPublicAttributeProperties.resetAttributes

--- @type fun(attribute: dr2c.ClientPrivateAttribute): dr2c.AttributeProperty
GNetworkClient.getPrivateAttributeProperty = proxyPrivateAttributeProperties.getAttributeProperty

--- @type fun(attribute: dr2c.ClientPrivateAttribute, args: dr2c.AttributePropertyArgs)
GNetworkClient.setPrivateAttributeProperty = proxyPrivateAttributeProperties.setAttributeProperty

--- @type fun(attribute: dr2c.ClientPrivateAttribute, value: integer | string): boolean
GNetworkClient.validatePrivateAttribute = proxyPrivateAttributeProperties.validateAttribute

--- @type fun(attributes: table<dr2c.ClientPrivateAttribute, Serializable>)
GNetworkClient.resetPrivateAttributes = proxyPrivateAttributeProperties.resetAttributes

--- @param dest table<dr2c.ClientPublicAttribute, any>
--- @param src table<dr2c.ClientPublicAttribute, any>
function GNetworkClient.mergePublicAttributes(dest, src)
	for attribute, value in pairs(src) do
		if not Enum.hasValue(PublicAttribute, attribute) then
			error(("Invalid public attribute %s"):format(attribute), 2)
		elseif not GNetworkClient.validatePublicAttribute(attribute, value) then
			error(("Invalid public attribute %s's value %s"):format(attribute, value), 2)
		else
			dest[attribute] = value
		end
	end
end

--- @param dest table<dr2c.ClientPrivateAttribute, any>
--- @param src table<dr2c.ClientPrivateAttribute, any>
function GNetworkClient.mergePrivateAttributes(dest, src)
	for attribute, value in pairs(src) do
		if not Enum.hasValue(PrivateAttribute, attribute) then
			error(("Invalid private attribute %s"):format(attribute), 2)
		elseif not GNetworkClient.validatePrivateAttribute(attribute, value) then
			error(("Invalid private attribute %s's value %s"):format(attribute, value), 2)
		else
			dest[attribute] = value
		end
	end
end

return GNetworkClient
