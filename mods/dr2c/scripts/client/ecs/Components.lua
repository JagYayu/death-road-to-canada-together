--[[
-- @module dr2c.client.ecs.Components
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CECSSchema = require("dr2c.client.ecs.ECSSchema")

--- @class dr2c.CComponents
local CComponents = {}

--- @param trait string
--- @param name string
--- @param fields table<Serializable, Serializable>
--- @param dependencies (table<string, any> | string[])?
--- @param ... unknown
local function registerImpl(trait, name, fields, dependencies, ...)
	local extras = { ... }

	events:add(CECSSchema.eventEntitySchemaLoadComponents, function(e)
		local CEntitySchema = require("dr2c.client.ecs.ECSSchema")

		e.new[name] = {
			fields = fields,
			trait = CEntitySchema.ComponentTrait[trait],
			dependencies = dependencies,
			unpack(extras),
		}
	end, nil, "Register")
end

--- @param name string
--- @param fields table<Serializable, Serializable>
--- @param dependencies (table<string, any> | string[])?
function CComponents.registerArchetypeConstant(name, fields, dependencies, ...)
	return registerImpl("ArchetypeConstant", name, fields, dependencies, ...)
end

--- @param name string
--- @param fields table<Serializable, Serializable>
--- @param dependencies (table<string, any> | string[])?
function CComponents.registerArchetypeSerializable(name, fields, dependencies, ...)
	return registerImpl("ArchetypeSerializable", name, fields, dependencies, ...)
end

--- @param name string
--- @param fields table<Serializable, Serializable>
--- @param dependencies (table<string, any> | string[])?
function CComponents.registerArchetypeTransient(name, fields, dependencies, ...)
	return registerImpl("ArchetypeTransient", name, fields, dependencies, ...)
end

--- @param name string
--- @param fields table<Serializable, Serializable>
--- @param dependencies (table<string, any> | string[])?
function CComponents.registerEntitySerializable(name, fields, dependencies, ...)
	return registerImpl("EntitySerializable", name, fields, dependencies, ...)
end

--- @param name string
--- @param fields table<Serializable, Serializable>
--- @param dependencies (table<string, any> | string[])?
function CComponents.registerEntityTransient(name, fields, dependencies, ...)
	return registerImpl("EntityTransient", name, fields, dependencies, ...)
end

--- `target` component depends on `source` component.
--- @param target string
--- @param source string
function CComponents.addDependency(target, source)
	--- @param e dr2c.E.ClientEntitySchemaLoadComponents
	events:add(N_("CEntitySchemaLoadComponents"), function(e)
		--- @class dr2c.E.ClientEntitySchemaLoadComponents
		--- @field dependencies table<string, table<string, true>>

		local dependencies = e.dependencies
		e.dependencies = dependencies or {}

		dependencies[target] = dependencies[target] or {}
		dependencies[target][source] = true
	end, nil, "Register")
end

function CComponents.registerComponentFromJsonFile() end

function CComponents.registerComponentFromJsonDirectory() end

function CComponents.registerComponentFromLuaFile() end

function CComponents.registerComponentFromLuaDirectory() end

return CComponents
