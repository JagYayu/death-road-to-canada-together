local CECSSchema = require("dr2c.client.ecs.ECSSchema")

local CEntities = {}

--- @param name string
--- @param components dr2c.Components
function CEntities.registerEntity(name, components, mergeDepth, ...)
	local extras = { ... }

	events:add(CECSSchema.eventEntitySchemaLoadEntities, function(e)
		e.new[name] = {
			components = components,
			mergeDepth = mergeDepth,
			unpack(extras),
		}
	end, N_("registerEntity" .. name), "Register")

	CECSSchema.reload()
end

return CEntities
