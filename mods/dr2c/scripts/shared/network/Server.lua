local Enum = require("tudov.Enum")
local Function = require("tudov.Function")

--- @class dr2c.GServer
local GServer = {}

--- @enum dr2c.GServer.Attribute
GServer.Attribute = Enum.sequence({
	Name = 1,
	Clients = 2,
	MaxClients = 3,
	Duration = 4,
	Version = 5,
	Mods = 6,
	HasPassword = 7,
})

--- @type table<dr2c.ClientPrivateAttribute, fun(value: any): boolean?>
local attributeValidators = {
	[GServer.Attribute.Name] = Function.isTypeString,
	[GServer.Attribute.Clients] = Function.isTypeInteger,
	[GServer.Attribute.MaxClients] = Function.isTypeInteger,
	[GServer.Attribute.Duration] = Function.isTypeNumber,
	[GServer.Attribute.Version] = Function.isTypeString,
	[GServer.Attribute.Mods] = Function.isTypeTable,
	[GServer.Attribute.HasPassword] = Function.isTypeBoolean,
}

function GServer.validateAttribute(attribute, value)
	return not not (attributeValidators[attribute] or Function.alwaysTrue)(value)
end

return GServer
