local CEntities = {}

--- @param name string
--- @param components dr2c.Components
function CEntities.registerEntity(name, components, mergeDepth, ...)
	local extras = { ... }

	events:add(N_("EntitySchemaLoadEntities"), function(e)
		e.schema[name] = {
			components = components,
			mergeDepth = mergeDepth,
			unpack(extras),
		}
	end, N_("registerEntity" .. name), "Register")
end

--- @param e dr2c.E.EntitySchemaLoadComponents
events:add(N_("EntitySchemaLoadEntities"), function(e)
	if e.dependencies then
		-- todo
	end
end, "registerDependencies", "Dependency")

return CEntities
