--- @class dr2c.EntityType : string
--- @class dr2c.EntityTypeID : integer
--- @class dr2c.ComponentType : string
--- @class dr2c.ComponentTypeID : integer

--- @class dr2c.EntitySchema
local EntitySchema = {}

EntitySchema.ComponentTrait = {
	Local = 0,
	Serializable = 1,
	Constant = 2,
}

--- @type table<dr2c.ComponentType, { fields: table<Serializable, Serializable> }>
local components = {}

local eventEntitySchemaLoadComponents = events:new("EntitySchemaLoadComponents", {
	"",
})

local eventEntitySchemaLoadEntities = events:new("EntitySchemaLoadEntities", {
	"",
})

function EntitySchema.reload()
	local componentSchema = {}

	events:invoke(eventEntitySchemaLoadComponents, componentSchema, nil, true)

	
end

--- @param componentType string
function EntitySchema.componentTrait(componentType)
	
end

return EntitySchema
