local Enum = require("tudov.Enum")
local Function = require("tudov.Function")

--- @class dr2c.GClient
local GClient = {}

GClient.State = Enum.sequence({
	Disconnected = 0,
	LoggingIn = 1,
	Synchronizing = 2,
	Playing = 3,
})

--- @enum dr2c.ClientPublicAttribute
GClient.PublicAttribute = Enum.sequence({
	ID = 0,
	State = 1,
	Platform = 2,
	PlatformID = 3,
	Statistics = 4,
	DisplayName = 5,
	ServerRoom = 6,
	Version = 7,
	ContentHash = 8,
	ModsHash = 9,
})

--- @enum dr2c.ClientPrivateAttribute
GClient.PrivateAttribute = Enum.sequence({
	SecretToken = 1,
	SecretStatistics = 2,
})

--- @type table<dr2c.ClientPublicAttribute, fun(value: any): boolean?>
local publicAttributeValidators = {
	[GClient.PublicAttribute.ID] = Function.isTypeInteger,
	[GClient.PublicAttribute.State] = Function.isTypeInteger,
	[GClient.PublicAttribute.Platform] = Function.isTypeString,
	[GClient.PublicAttribute.PlatformID] = Function.isTypeString,
	[GClient.PublicAttribute.Statistics] = Function.isTypeTable,
	[GClient.PublicAttribute.DisplayName] = Function.isTypeString,
	[GClient.PublicAttribute.ServerRoom] = Function.isTypeTable,
	[GClient.PublicAttribute.Version] = Function.isTypeString,
	[GClient.PublicAttribute.ContentHash] = Function.isTypeNumber,
	[GClient.PublicAttribute.ModsHash] = Function.isTypeNumber,
}

--- @type table<dr2c.ClientPrivateAttribute, fun(value: any): boolean?>
local privateAttributeValidators = {
	[GClient.PrivateAttribute.SecretToken] = Function.isTypeNumber,
	[GClient.PrivateAttribute.SecretStatistics] = Function.isTypeTable,
}

function GClient.validatePublicAttribute(attribute, value)
	return not not (publicAttributeValidators[attribute] or Function.alwaysTrue)(value)
end

function GClient.validatePrivateAttribute(attribute, value)
	return not not (privateAttributeValidators[attribute] or Function.alwaysTrue)(value)
end

return GClient
