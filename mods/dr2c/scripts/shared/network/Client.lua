local Enum = require("tudov.Enum")
local Function = require("tudov.Function")

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
	-- 当前连接状态
	State = 1,
	-- 游戏平台
	Platform = 2,
	-- 游戏平台账户ID
	PlatformID = 3,
	-- 操作系统
	OperatingSystem = 4,
	-- 账户资料
	Statistics = 5,
	-- 昵称（覆盖账户名）
	DisplayName = 6,
	-- 所在服务器房间
	Room = 7,
	-- app版本
	Version = 8,
	-- 游戏文件哈希值，用于判断游戏的核心文件是否和服务器一致
	ContentHash = 9,
	-- 游戏模组哈希值，用于判断必要的模组是否和服务器一致
	ModsHash = 10,
	-- 套接口类型
	SocketType = 11,
})

GClient.PrivateAttribute = Enum.sequence({
	SecretToken = 1,
	SecretStatistics = 2,
})

local publicAttributeTraits = {
	[GClient.PublicAttribute.ID] = GClient.AttributeTrait.Authoritative,
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

--- @type table<dr2c.GClient.PrivateAttribute, fun(value: any): boolean?>
local privateAttributeValidators = {
	[GClient.PrivateAttribute.SecretToken] = Function.isTypeNumberOrNil,
	[GClient.PrivateAttribute.SecretStatistics] = Function.isTypeTable,
}

--- @param attribute dr2c.GClient.PublicAttribute
--- @param value integer | string
--- @return boolean
function GClient.validatePublicAttribute(attribute, value)
	return not not (publicAttributeValidators[attribute] or Function.alwaysTrue)(value)
end

--- @param attribute dr2c.GClient.PrivateAttribute
--- @param value integer | string
--- @return boolean
function GClient.validatePrivateAttribute(attribute, value)
	return not not (privateAttributeValidators[attribute] or Function.alwaysTrue)(value)
end

return GClient
