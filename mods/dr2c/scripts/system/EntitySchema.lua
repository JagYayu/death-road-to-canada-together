--- @class dr2c.EntitySchema
local EntitySchema = {}

--- @class dr2c.EntityType : string

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

return EntitySchema
