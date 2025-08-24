local Enum = require("tudov.Enum")
local Function = require("tudov.Function")

--- @class dr2c.GClient
local GClient = {}

GClient.State = Enum.sequence({
	Disconnected = 0,
	LoggingIn = 1,
	
})

--- @enum dr2c.GClient.PublicAttribute
GClient.PublicAttribute = Enum.sequence({
	ID = 0,
	State = 1,
	Platform = 2,
	PlatformID = 3,
	Statistics = 4,
	DisplayName = 5,
	ServerRoom = 6,
	GameVersion = 7,
	ContentHash = 8,
	ModsHash = 9,
})

--- @enum dr2c.GClient.PrivateAttribute
GClient.PrivateAttribute = Enum.sequence({
	SecretToken = 1,
	SecretStatistics = 2,
})

--- @type table<dr2c.GClient.PublicAttribute, fun(value: any): boolean?>
local publicAttributeValidators = {
	ID = Function.isTypeInteger,
	State = Function.isTypeInteger,
	Platform = Function.isTypeString,
	PlatformID = Function.isTypeString,
	Statistics = Function.isTypeTable,
	DisplayName = Function.isTypeString,
	ServerRooms = Function.isTypeTable,
	GameVersion = Function.isTypeString,
	ContentHash = Function.isTypeNumber,
	ModsHash = Function.isTypeNumber,
}

--- @type table<dr2c.GClient.PrivateAttribute, fun(value: any): boolean?>
local privateAttributeValidators = {}

function GClient.validatePublicAttribute(attribute, value)
	return not not (publicAttributeValidators[attribute] or Function.alwaysTrue)(value)
end

function GClient.validatePrivateAttribute(attribute, value)
	return not not (privateAttributeValidators[attribute] or Function.alwaysTrue)(value)
end

return GClient
