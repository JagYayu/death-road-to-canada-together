local EntitySchema = {}

local eventEntitySchemaLoadComponents = events:new(N_("EntitySchemaLoadComponents"), {
	"",
})

local eventEntitySchemaLoadEntities = events:new(N_("EntitySchemaLoadEntities"), {
	"",
})

function EntitySchema.reload()
	local componentSchema = {}

	events:invoke(eventEntitySchemaLoadComponents, componentSchema, nil, true)
end

return EntitySchema
