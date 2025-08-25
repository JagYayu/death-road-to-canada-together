local CECSSchema = require("dr2c.client.ecs.ECSSchema")

--- @class dr2c.Components
local CComponents = {}

--- @param name string
--- @param fields table<Serializable, Serializable>
function CComponents.registerSerializable(name, fields, ...)
	local extras = { ... }

	events:add(CECSSchema.eventEntitySchemaLoadComponents, function(e)
		local CEntitySchema = require("dr2c.client.ecs.ECSSchema")

		e.new[name] = {
			fields = fields,
			trait = CEntitySchema.ComponentTrait.Serializable,
			unpack(extras),
		}
	end, N_("registerComponent" .. name), "Register")
end

--- target component depends on source component.
--- @param target string
--- @param source string
function CComponents.addDependency(target, source)
	events:add(N_("CEntitySchemaLoadComponents"), function(e)
		e.dependencies = e.dependencies or {}

		local dependencies = e.dependencies
		dependencies[target] = dependencies[target] or {}

		dependencies[source] = true
	end, nil, "Register")
end

return CComponents
