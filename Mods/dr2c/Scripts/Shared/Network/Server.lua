--[[
-- @module dr2c.Shared.Network.Server
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

local GUtilsAttribute = require("dr2c.Shared.Utils.Attribute")

--- @alias dr2c.ServerAttribute dr2c.GNetworkServer.Attribute

--- @alias dr2c.ServerAttributeRooms dr2c.NetworkRoomID[]

--- @class dr2c.GServer
local GNetworkServer = {}

--- @alias dr2c.NetworkServerAttribute dr2c.GNetworkServer.Attribute

GNetworkServer.Attribute = Enum.sequence({
	-- 服务器显示名
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
	-- 是否含有服务端模组
	HasServerOnlyMods = 10,
})

local proxyAttributeProperties = GUtilsAttribute.newProperties({
	[GNetworkServer.Attribute.DisplayName] = {
		default = "",
		validator = Function.isTypeString,
	},
	[GNetworkServer.Attribute.Clients] = {
		default = {},
		validator = Function.isTypeInteger,
	},
	[GNetworkServer.Attribute.MaxClients] = {
		default = 4,
		validator = Function.isTypeInteger,
	},
	[GNetworkServer.Attribute.StartupTime] = {
		default = Time.getSystemTime,
		validator = Function.isTypeNumber,
	},
	[GNetworkServer.Attribute.Version] = {
		default = function()
			return "" -- TODO
		end,
		validator = Function.isTypeString,
	},
	[GNetworkServer.Attribute.Mods] = {
		default = function()
			return {} -- TODO
		end,
		validator = Function.isTypeTable,
	},
	[GNetworkServer.Attribute.HasPassword] = {
		default = false,
		validator = Function.isTypeBoolean,
	},
	[GNetworkServer.Attribute.Rooms] = {
		default = {},
		validator = Function.isTypeTable,
	},
})

--- @type fun(attribute: dr2c.NetworkServerAttribute): dr2c.AttributeProperty
GNetworkServer.getAttributeProperty = proxyAttributeProperties.getAttributeProperty

--- @type fun(attribute: dr2c.NetworkServerAttribute, args: dr2c.AttributePropertyArgs)
GNetworkServer.setAttributeProperty = proxyAttributeProperties.setAttributeProperty

--- @type fun(attribute: dr2c.NetworkServerAttribute, value: any): boolean
GNetworkServer.validateAttribute = proxyAttributeProperties.validateAttribute

return GNetworkServer
