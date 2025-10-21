--[[
-- @module dr2c.Client.Entity.ECSSchema
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class dr2c.EntityType : string
--- @class dr2c.EntityTypeID : integer
--- @alias dr2c.EntityTypeOrID dr2c.EntityType | dr2c.EntityTypeID
--- @class dr2c.ComponentType : string
--- @class dr2c.ComponentTypeID : integer

--- @alias dr2c.ECSSchema.Fields  table<Serializable, Serializable>

--- @class dr2c.ECSSchema.Component
--- @field type dr2c.ComponentType
--- @field typeID dr2c.ComponentTypeID
--- @field fields dr2c.ECSSchema.Fields
--- @field trait dr2c.EntityComponentTrait

--- @class dr2c.ECSSchema.Components
--- @field [dr2c.ComponentType] dr2c.ECSSchema.Component
--- @field [dr2c.ComponentTypeID] dr2c.ECSSchema.Component

--- @class dr2c.ECSSchema.EntityComponent
--- @field componentSchema dr2c.ECSSchema.Component
--- @field componentFields dr2c.Component
--- @field mergedFields dr2c.Component
--- @field mergedFieldsBuffer string
--- @field metatable metatable

-- --- @field fields dr2c.Component

--- @class dr2c.ECSSchema.EntityComponents
--- @field [dr2c.ComponentType] dr2c.ECSSchema.EntityComponent
--- @field [dr2c.ComponentTypeID] dr2c.ECSSchema.EntityComponent

--- @class dr2c.ECSSchema.Entity
--- @field type dr2c.EntityType
--- @field typeID dr2c.EntityTypeID
--- @field components dr2c.ECSSchema.EntityComponents
--- @field componentsArchetypeConstant dr2c.ECSSchema.EntityComponents
--- @field componentsArchetypeSerializable dr2c.ECSSchema.EntityComponents
--- @field componentsArchetypeTransient dr2c.ECSSchema.EntityComponents
--- @field componentsEntitySerializable dr2c.ECSSchema.EntityComponents
--- @field componentsEntityTransient dr2c.ECSSchema.EntityComponents

--- @class dr2c.ECSSchema.Entities
--- @field [dr2c.EntityType] dr2c.ECSSchema.Entity
--- @field [dr2c.EntityTypeID] dr2c.ECSSchema.Entity

local Enum = require("TE.Enum")
local String = require("TE.String")
local Table = require("TE.Table")

--- @class dr2c.EntityECSSchema
local CEntityECSSchema = {}

--- @alias dr2c.EntityComponentTrait dr2c.CEntityECSSchema.ComponentTrait

CEntityECSSchema.ComponentTrait = Enum.immutable({
	ArchetypeConstant = 0,
	ArchetypeSerializable = 1,
	ArchetypeTransient = 2,
	EntitySerializable = 3,
	EntityTransient = 4,

	--- @deprecated
	Constant = 0,
	--- @deprecated
	Mutable = 4,
	--- @deprecated
	Serializable = 3,
	--- @deprecated
	Shared = 1,
})

local reloadPending = true

--- @type dr2c.ECSSchema.Components
local componentsSchema = {}
componentsSchema = persist("componentsSchema", function()
	return componentsSchema
end)

--- @type dr2c.ECSSchema.Entities
local entitiesSchema = {}
entitiesSchema = persist("entitiesSchema", function()
	return entitiesSchema
end)

--- @warn Do not modify returned table unless you know what it means
--- @return dr2c.ECSSchema.Components @Readonly
function CEntityECSSchema.getComponentsSchema()
	return componentsSchema
end

--- @return integer
function CEntityECSSchema.getComponentsCount()
	return #componentsSchema
end

--- @warn No not modify returned table unless you know what it means
--- @return dr2c.ECSSchema.Entities @Readonly
function CEntityECSSchema.getEntitiesSchema()
	return entitiesSchema
end

--- @return integer
function CEntityECSSchema.getEntitiesCount()
	return #entitiesSchema
end

--- @param componentTypeOrID dr2c.ComponentType | dr2c.ComponentTypeID
--- @return boolean
function CEntityECSSchema.hasComponent(componentTypeOrID)
	return not not componentsSchema[componentTypeOrID]
end

--- @param entityTypeOrID dr2c.EntityTypeOrID
--- @return boolean
function CEntityECSSchema.hasEntity(entityTypeOrID)
	return not not entitiesSchema[entityTypeOrID]
end

--- @param componentTypeID dr2c.ComponentTypeID
--- @return dr2c.ComponentType?
function CEntityECSSchema.getComponentType(componentTypeID)
	local component = componentsSchema[componentTypeID]
	if component then
		return component.type
	end
end

--- @param componentType dr2c.ComponentType
--- @return dr2c.ComponentTypeID?
function CEntityECSSchema.getComponentTypeID(componentType)
	local component = componentsSchema[componentType]
	if component then
		return component.typeID
	end
end

--- @param componentTypeOrID dr2c.EntityTypeID
--- @return dr2c.ECSSchema.Fields? fields @Readonly
function CEntityECSSchema.getComponentFields(componentTypeOrID)
	local component = componentsSchema[componentTypeOrID]
	if component then
		return component.fields
	end
end

--- @param componentTypeOrID dr2c.ComponentType | dr2c.EntityTypeID
--- @return dr2c.EntityComponentTrait?
function CEntityECSSchema.getComponentTrait(componentTypeOrID)
	local componentSchema = componentsSchema[componentTypeOrID]
	return componentSchema and componentSchema.trait or nil
end

--- @param entityTypeID dr2c.EntityTypeID
--- @return dr2c.EntityType?
function CEntityECSSchema.getEntityType(entityTypeID)
	local entitySchema = entitiesSchema[entityTypeID]
	return entitySchema and entitySchema.type or nil
end

--- @param entityType dr2c.EntityType
--- @return dr2c.EntityTypeID?
function CEntityECSSchema.getEntityTypeID(entityType)
	local entitySchema = entitiesSchema[entityType]
	return entitySchema and entitySchema.typeID or nil
end

--- @param entityTypeOrID dr2c.EntityTypeOrID
--- @return dr2c.ECSSchema.EntityComponents?
function CEntityECSSchema.getEntityComponents(entityTypeOrID)
	local entitySchema = entitiesSchema[entityTypeOrID]
	return entitySchema and entitySchema.components or nil
end

--- @param entityTypeOrID dr2c.EntityTypeOrID
--- @return dr2c.ECSSchema.EntityComponents?
function CEntityECSSchema.getEntityComponentsArchetypeConstant(entityTypeOrID)
	local entitySchema = entitiesSchema[entityTypeOrID]
	return entitySchema and entitySchema.componentsArchetypeConstant or nil
end

--- @param entityTypeOrID dr2c.EntityTypeOrID
--- @return dr2c.ECSSchema.EntityComponents?
function CEntityECSSchema.getEntityComponentsEntityTransient(entityTypeOrID)
	local entitySchema = entitiesSchema[entityTypeOrID]
	return entitySchema and entitySchema.componentsEntityTransient or nil
end

--- @param entityTypeOrID dr2c.EntityTypeOrID
--- @return dr2c.ECSSchema.EntityComponents?
function CEntityECSSchema.getEntityComponentsEntitySerializable(entityTypeOrID)
	local entitySchema = entitiesSchema[entityTypeOrID]
	return entitySchema and entitySchema.componentsEntitySerializable or nil
end

--- @param entityTypeOrID dr2c.EntityTypeOrID
--- @return dr2c.ECSSchema.EntityComponents?
function CEntityECSSchema.getEntityComponentsArchetypeSerializable(entityTypeOrID)
	local entitySchema = entitiesSchema[entityTypeOrID]
	return entitySchema and entitySchema.componentsArchetypeSerializable or nil
end

--- @param entityTypeOrID dr2c.EntityTypeOrID
--- @param componentTypeOrIDs (dr2c.ComponentType | dr2c.ComponentTypeID)[]
--- @return boolean?
function CEntityECSSchema.entityHasAllComponents(entityTypeOrID, componentTypeOrIDs)
	local entitySchema = entitiesSchema[entityTypeOrID]
	if not entitySchema then
		return
	end

	local components = entitySchema.components
	for _, componentTypeOrID in ipairs(componentTypeOrIDs) do
		if not components[componentTypeOrID] then
			return false
		end
	end

	return true
end

--- @param entityTypeOrID dr2c.EntityTypeOrID
--- @param componentTypeOrIDs (dr2c.ComponentType | dr2c.ComponentTypeID)[]
--- @return boolean?
function CEntityECSSchema.entityHasAnyComponents(entityTypeOrID, componentTypeOrIDs)
	local entitySchema = entitiesSchema[entityTypeOrID]
	if not entitySchema then
		return
	end

	local components = entitySchema.components
	for _, componentTypeOrID in ipairs(componentTypeOrIDs) do
		if components[componentTypeOrID] then
			return true
		end
	end

	return false
end

--- @param entityTypeOrID dr2c.EntityTypeOrID
--- @param componentTypeOrIDs (dr2c.ComponentType | dr2c.ComponentTypeID)[]
--- @return boolean?
function CEntityECSSchema.entityHasNonComponents(entityTypeOrID, componentTypeOrIDs)
	local entitySchema = entitiesSchema[entityTypeOrID]
	if not entitySchema then
		return
	end

	local components = entitySchema.components
	for _, componentTypeOrID in ipairs(componentTypeOrIDs) do
		if components[componentTypeOrID] then
			return false
		end
	end

	return true
end

CEntityECSSchema.eventEntitySchemaLoadComponents = TE.events:new(N_("CEntitySchemaLoadComponents"), {
	"Register",
	"Dependency",
	"Override",
	"Validate",
	"Finalize",
})

CEntityECSSchema.eventEntitySchemaLoadEntities = TE.events:new(N_("CEntitySchemaLoadEntities"), {
	"Register",
	"Override",
	"Validate",
	"Finalize",
})

local function reloadComponentsSchema(oldComponentsSchema)
	local tempComponentsSchema = {}

	--- @class dr2c.E.EntitySchemaLoadComponents
	--- @field new table<string, { fields: table, trait: dr2c.EntityComponentTrait, [...]: any }>
	--- @field dependencies table?
	local e = {
		new = {},
		old = oldComponentsSchema,
	}
	local newComponentsSchema = e.new

	TE.events:invoke(CEntityECSSchema.eventEntitySchemaLoadComponents, e, nil, EEventInvocation.None)

	for state, componentType, entry in Table.sortedPairs(newComponentsSchema) do
		local componentTypeID = state[2]

		local fields
		if type(entry.fields) == "table" then
			fields = Table.fastCopy(entry.fields)
		else
			fields = {}
		end

		local componentTrait = tonumber(entry.trait) or CEntityECSSchema.ComponentTrait.EntitySerializable
		--- @diagnostic disable-next-line: cast-type-mismatch
		--- @cast componentTrait dr2c.EntityComponentTrait

		--- @type dr2c.ECSSchema.Component
		local componentSchema = {
			type = componentType,
			typeID = componentTypeID,
			fields = fields,
			trait = componentTrait,
		}
		tempComponentsSchema[componentTypeID] = componentSchema
		tempComponentsSchema[componentType] = componentSchema
	end

	return tempComponentsSchema
end

local function processEntityComponents(entityComponents, tempComponentsSchema)
	local all = {}
	local entitySerializable = {}
	local entityTransient = {}
	local archetypeConstant = {}
	local archetypeSerializable = {}
	local archetypeTransient = {}

	local traitEntitySerializable = CEntityECSSchema.ComponentTrait.EntitySerializable
	local traitEntityTransient = CEntityECSSchema.ComponentTrait.EntityTransient
	local traitArchetypeConstant = CEntityECSSchema.ComponentTrait.ArchetypeConstant
	local traitArchetypeSerializable = CEntityECSSchema.ComponentTrait.ArchetypeSerializable
	local traitArchetypeTransient = CEntityECSSchema.ComponentTrait.ArchetypeTransient

	local mergeDepth = entityComponents.mergeDepth

	--- @param componentType dr2c.ComponentType
	--- @param componentFields dr2c.Component
	for state, componentType, componentFields in Table.sortedPairs(entityComponents.components) do
		local index = state[2]

		local componentSchema = tempComponentsSchema[componentType]
		if not componentSchema then
			if log.canError() then
				log.error(("Invalid component '%s'"):format(componentType))
			end

			goto continue
		end

		componentFields = Table.fastCopy(componentFields)

		local mergedFields = Table.fastCopy(componentSchema.fields)

		Table.merge(mergedFields, componentFields, tonumber(mergeDepth) or 1)

		--- @type dr2c.ECSSchema.EntityComponent
		local component = {
			componentSchema = componentSchema,
			componentFields = componentFields,
			mergedFields = mergedFields,
			mergedFieldsBuffer = String.bufferEncode(mergedFields),
			metatable = {
				__index = mergedFields,
			},
		}

		all[index] = component
		all[componentType] = component

		if componentSchema.trait == traitArchetypeConstant then
			archetypeConstant[#archetypeConstant + 1] = component
			archetypeConstant[componentType] = component
		elseif componentSchema.trait == traitEntityTransient then
			entityTransient[#entityTransient + 1] = component
			entityTransient[componentType] = component
		elseif componentSchema.trait == traitEntitySerializable then
			entitySerializable[#entitySerializable + 1] = component
			entitySerializable[componentType] = component
		elseif componentSchema.trait == traitArchetypeSerializable then
			archetypeSerializable[#archetypeSerializable + 1] = component
			archetypeSerializable[componentType] = component
		elseif componentSchema.trait == traitArchetypeTransient then
			archetypeTransient[#archetypeTransient + 1] = component
			archetypeTransient[componentType] = component
		end

		::continue::
	end

	return all, entitySerializable, entityTransient, archetypeConstant, archetypeSerializable, archetypeTransient
end

local function reloadEntitiesSchema(oldEntitiesSchema, tempComponentsSchema)
	local tempEntitiesSchema = {}

	--- @class dr2c.E.EntitySchemaLoadEntities
	local e = {
		new = {},
		old = oldEntitiesSchema,
	}
	local newEntitiesSchema = e.new

	TE.events:invoke(CEntityECSSchema.eventEntitySchemaLoadEntities, e, nil, EEventInvocation.None)

	for state, entityType, entityComponents in Table.sortedPairs(newEntitiesSchema) do
		local entityTypeID = state[2]

		local all, entitySerializable, entityTransient, archetypeConstant, archetypeSerializable, archetypeTransient =
			processEntityComponents(entityComponents, tempComponentsSchema)

		--- @type dr2c.ECSSchema.Entity
		local entitySchema = {
			type = entityType,
			typeID = entityTypeID,
			components = all,
			componentsArchetypeConstant = archetypeConstant,
			componentsEntityTransient = entityTransient,
			componentsEntitySerializable = entitySerializable,
			componentsArchetypeSerializable = archetypeSerializable,
			componentsArchetypeTransient = archetypeTransient,
		}

		tempEntitiesSchema[entityTypeID] = entitySchema
		tempEntitiesSchema[entityType] = entitySchema
	end

	return tempEntitiesSchema
end

CEntityECSSchema.eventCEntitySchemaLoaded = TE.events:new(N_("CEntitySchemaLoaded"), {
	"Components",
	"Filters",
})

function CEntityECSSchema.reloadImmediately()
	local tempComponentsSchema = reloadComponentsSchema(componentsSchema)
	local tempEntitiesSchema = reloadEntitiesSchema(entitiesSchema, tempComponentsSchema)

	componentsSchema = tempComponentsSchema
	entitiesSchema = tempEntitiesSchema

	TE.events:invoke(CEntityECSSchema.eventCEntitySchemaLoaded, {})

	reloadPending = false
end

function CEntityECSSchema.reload()
	reloadPending = true
	TE.engine:triggerLoadPending()
end

CEntityECSSchema.reload()

--- @param e dr2c.E.CLoad
TE.events:add(N_("CLoad"), function(e)
	if reloadPending then
		CEntityECSSchema.reloadImmediately()

		--- @class dr2c.E.CLoad
		--- @field ecsSchema? { components: dr2c.ECSSchema.Components, entities: dr2c.ECSSchema.Entities }
		e.ecsSchema = {
			components = componentsSchema,
			entities = entitiesSchema,
		}
	end
end, "loadECSSchema", "ECS")

return CEntityECSSchema
