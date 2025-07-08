local EntitySchema = {}

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
