local CECSSchema = require("dr2c.client.ecs.ECSSchema")

--- @class dr2c.Components
local CComponents = {}

local function registerImpl(trait, name, fields, ...)
	local extras = { ... }

	events:add(CECSSchema.eventEntitySchemaLoadComponents, function(e)
		local CEntitySchema = require("dr2c.client.ecs.ECSSchema")

		e.new[name] = {
			fields = fields,
			trait = CEntitySchema.ComponentTrait[trait],
			unpack(extras),
		}
	end, N_("registerComponent" .. name), "Register")
end

--- @param name string
--- @param fields table<Serializable, Serializable>
function CComponents.registerConstant(name, fields, ...)
	return registerImpl("Constant", name, fields, ...)
end

--- @param name string
--- @param fields table<Serializable, Serializable>
function CComponents.registerMutable(name, fields, ...)
	return registerImpl("Mutable", name, fields, ...)
end

--- @param name string
--- @param fields table<Serializable, Serializable>
function CComponents.registerSerializable(name, fields, ...)
	return registerImpl("Serializable", name, fields, ...)
end

--- @param name string
--- @param fields table<Serializable, Serializable>
function CComponents.registerShared(name, fields, ...)
	return registerImpl("Shared", name, fields, ...)
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

function CComponents.registerComponentFromJsonFile()
	--
end

function CComponents.registerComponentFromJsonDirectory() end

return CComponents
