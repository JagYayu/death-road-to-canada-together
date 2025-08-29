local Enum = require("tudov.Enum")
local Function = require("tudov.Function")

--- @class dr2c.GServer
local GServer = {}

--- @alias dr2c.ServerAttributeRooms dr2c.RoomID[]

GServer.Attribute = Enum.sequence({
	-- 服务器名
	DisplayName = 1,
	-- 当前客户端数
	Clients = 2,
	-- 最大客户端数
	MaxClients = 3,
	-- 启动时间
	StartupTime = 4,
	-- app版本
	Version = 5,
	-- 模组列表
	Mods = 6,
	-- 是否有密码
	HasPassword = 7,
	-- 房间ID列表
	Rooms = 8,
	-- 套接口类型
	SocketType = 9,
})

--- @type table<dr2c.GClient.PrivateAttribute, fun(value: any): boolean?>
local attributeValidators = {
	[GServer.Attribute.DisplayName] = Function.isTypeString,
	[GServer.Attribute.Clients] = Function.isTypeInteger,
	[GServer.Attribute.MaxClients] = Function.isTypeInteger,
	[GServer.Attribute.StartupTime] = Function.isTypeNumber,
	[GServer.Attribute.Version] = Function.isTypeString,
	[GServer.Attribute.Mods] = Function.isTypeTable,
	[GServer.Attribute.HasPassword] = Function.isTypeBoolean,
	[GServer.Attribute.Rooms] = Function.isTypeTable,
}

function GServer.validateAttribute(attribute, value)
	return not not (attributeValidators[attribute] or Function.alwaysTrue)(value)
end

return GServer
