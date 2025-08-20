--- @class dr2c.EntityType : string
--- @class dr2c.EntityTypeID : integer
--- @class dr2c.ComponentType : string
--- @class dr2c.ComponentTypeID : integer

--- @alias dr2c.ECSSchema.Fields  table<Serializable, Serializable>

--- @class dr2c.ECSSchema.Component
--- @field type dr2c.ComponentType
--- @field typeID dr2c.ComponentTypeID
--- @field fields dr2c.ECSSchema.Fields
--- @field trait dr2c.ComponentTrait

--- @class dr2c.ECSSchema.Components
--- @field [dr2c.ComponentType] dr2c.ECSSchema.Component
--- @field [dr2c.ComponentTypeID] dr2c.ECSSchema.Component

--- @class dr2c.ECSSchema.EntityComponent
--- @field componentSchema dr2c.ECSSchema.Component
--- @field component dr2c.Component
--- @field fields string | dr2c.Component @Type is string only when component's trait is "Mutable"

--- @class dr2c.ECSSchema.EntityComponents
--- @field [dr2c.ComponentType] dr2c.ECSSchema.EntityComponent
--- @field [dr2c.ComponentTypeID] dr2c.ECSSchema.EntityComponent

--- @class dr2c.ECSSchema.Entity
--- @field type dr2c.EntityType
--- @field typeID dr2c.EntityTypeID
--- @field components dr2c.ECSSchema.EntityComponents
--- @field componentsSerializable dr2c.ECSSchema.EntityComponents
--- @field componentsMutable dr2c.ECSSchema.EntityComponents
--- @field componentsConstant dr2c.ECSSchema.EntityComponents
--- @field componentsShared dr2c.ECSSchema.EntityComponents

--- @class dr2c.ECSSchema.Entities
--- @field [dr2c.EntityType] dr2c.ECSSchema.Entity
--- @field [dr2c.EntityTypeID] dr2c.ECSSchema.Entity

local GTable = require("dr2c.shared.utils.Table")
local stringBuffer = require("string.buffer")

--- @class dr2c.ECSSchema
local CECSSchema = {}

--- @enum dr2c.ComponentTrait
CECSSchema.ComponentTrait = {
	Constant = 0,
	Mutable = 1,
	Serializable = 2,
	Shared = 3,
}

local reloadPending = true

--- @type dr2c.ECSSchema.Components
local componentsSchema
componentsSchema = persist("componentsSchema", {}, function()
	return componentsSchema
end)

--- @type dr2c.ECSSchema.Entities
local entitiesSchema
persist("entitiesSchema", {}, function()
	return entitiesSchema
end)

--- @return dr2c.ECSSchema.Components @Readonly
function CECSSchema.getComponentsSchema()
	return componentsSchema
end

--- @return integer
function CECSSchema.getComponentsCount()
	return #componentsSchema
end

--- @return dr2c.ECSSchema.Entities @Readonly
function CECSSchema.getEntitiesSchema()
	return entitiesSchema
end

--- @return integer
function CECSSchema.getEntitiesCount()
	return #entitiesSchema
end

--- @param componentTypeID dr2c.ComponentTypeID
--- @return dr2c.ComponentType?
function CECSSchema.getComponentType(componentTypeID)
	local component = componentsSchema[componentTypeID]
	if component then
		return component.type
	end
end

--- @param componentType dr2c.ComponentType
--- @return dr2c.ComponentTypeID?
function CECSSchema.getComponentTypeID(componentType)
	local component = componentsSchema[componentType]
	if component then
		return component.typeID
	end
end

--- @param componentTypeOrID dr2c.EntityType | dr2c.EntityTypeID
--- @return dr2c.ECSSchema.Fields? fields @Readonly
function CECSSchema.getComponentFields(componentTypeOrID)
	local component = componentsSchema[componentTypeOrID]
	if component then
		return component.fields
	end
end

--- @param componentTypeOrID dr2c.ComponentType | dr2c.EntityTypeID
--- @return dr2c.ComponentTrait?
function CECSSchema.getComponentTrait(componentTypeOrID)
	local componentSchema = componentsSchema[componentTypeOrID]
	return componentSchema and componentSchema.trait or nil
end

--- @param entityTypeID dr2c.EntityTypeID
--- @return dr2c.EntityType?
function CECSSchema.getEntityType(entityTypeID)
	local entitySchema = entitiesSchema[entityTypeID]
	return entitySchema and entitySchema.type or nil
end

--- @param entityType dr2c.EntityType
--- @return dr2c.EntityTypeID?
function CECSSchema.getEntityTypeID(entityType)
	local entitySchema = entitiesSchema[entityType]
	return entitySchema and entitySchema.typeID or nil
end

--- @param entityTypeOrID dr2c.EntityType | dr2c.EntityTypeID
--- @return dr2c.ECSSchema.EntityComponents?
function CECSSchema.getEntityComponents(entityTypeOrID)
	local entitySchema = entitiesSchema[entityTypeOrID]
	return entitySchema and entitySchema.components or nil
end

--- @param entityTypeOrID dr2c.EntityType | dr2c.EntityTypeID
--- @return dr2c.ECSSchema.EntityComponents?
function CECSSchema.getEntityComponentsConstants(entityTypeOrID)
	local entitySchema = entitiesSchema[entityTypeOrID]
	return entitySchema and entitySchema.componentsConstant or nil
end

--- @param entityTypeOrID dr2c.EntityType | dr2c.EntityTypeID
--- @return dr2c.ECSSchema.EntityComponents?
function CECSSchema.getEntityComponentsMutable(entityTypeOrID)
	local entitySchema = entitiesSchema[entityTypeOrID]
	return entitySchema and entitySchema.componentsMutable or nil
end

--- @param entityTypeOrID dr2c.EntityType | dr2c.EntityTypeID
--- @return dr2c.ECSSchema.EntityComponents?
function CECSSchema.getEntityComponentsSerializable(entityTypeOrID)
	local entitySchema = entitiesSchema[entityTypeOrID]
	return entitySchema and entitySchema.componentsSerializable or nil
end

--- @param entityTypeOrID dr2c.EntityType | dr2c.EntityTypeID
--- @return dr2c.ECSSchema.EntityComponents?
function CECSSchema.getEntityComponentsShared(entityTypeOrID)
	local entitySchema = entitiesSchema[entityTypeOrID]
	return entitySchema and entitySchema.componentsShared or nil
end

--- @param entityTypeOrID dr2c.EntityType | dr2c.EntityTypeID
--- @param componentTypeOrIDs (dr2c.ComponentType | dr2c.ComponentTypeID)[]
function CECSSchema.entityHasAllComponents(entityTypeOrID, componentTypeOrIDs) end

function CECSSchema.entityHasNonComponents(entityTypeOrID, componentTypeOrIDs) end

CECSSchema.eventEntitySchemaLoadComponents = events:new(N_("EntitySchemaLoadComponents"), {
	"Register",
	"Dependency",
	"Override",
	"Validate",
	"Finalize",
})

CECSSchema.eventEntitySchemaLoadEntities = events:new(N_("EntitySchemaLoadEntities"), {
	"Register",
	"Override",
	"Validate",
	"Finalize",
})

local function reloadComponentsSchema(oldComponentsSchema)
	local tempComponentsSchema = {}

	--- @class dr2c.E.EntitySchemaLoadComponents
	--- @field new table<string, { fields: table, trait: dr2c.ComponentTrait, [...]: any }>
	--- @field dependencies table?
	local e = {
		new = {},
		old = oldComponentsSchema,
	}
	local newComponentsSchema = e.new

	events:invoke(CECSSchema.eventEntitySchemaLoadComponents, e, nil, EEventInvocation.None)

	for state, componentType, entry in GTable.sortedPairs(newComponentsSchema) do
		local componentTypeID = state[2]

		local componentFields
		if type(entry.fields) == "table" then
			componentFields = entry.fields
		else
			componentFields = {}
		end

		local componentTrait = tonumber(entry.trait) or CECSSchema.ComponentTrait.Serializable

		--- @type dr2c.ECSSchema.Component
		local componentSchema = {
			type = componentType,
			typeID = componentTypeID,
			fields = componentFields,
			trait = componentTrait,
		}
		tempComponentsSchema[componentTypeID] = componentSchema
		tempComponentsSchema[componentType] = componentSchema
	end

	return tempComponentsSchema
end

local function processEntityComponents(entityComponents, tempComponentsSchema)
	local components = {}
	local componentsSerializable = {}
	local componentsMutable = {}
	local componentsConstant = {}
	local componentsShared = {}

	local mergeDepth = entityComponents.mergeDepth

	--- @param componentType dr2c.ComponentType
	--- @param componentFields dr2c.Component
	for state, componentType, componentFields in GTable.sortedPairs(entityComponents.components) do
		local index = state[2]

		local componentSchema = tempComponentsSchema[componentType]
		if not componentSchema then
			error("Invalid component " .. componentType, 3)
		end

		local fields = GTable.fastCopy(componentSchema.fields)
		GTable.merge(fields, componentFields, tonumber(mergeDepth) or 1)

		if componentSchema.trait == CECSSchema.ComponentTrait.Mutable then
			fields = stringBuffer.encode(fields)
		end

		local component = {
			componentSchema = componentSchema,
			component = componentFields,
			fields = fields,
		}

		components[index] = component
		components[componentType] = component

		if componentSchema.trait == CECSSchema.ComponentTrait.Constant then
			componentsConstant[#componentsConstant + 1] = component
			componentsConstant[componentType] = component
		elseif componentSchema.trait == CECSSchema.ComponentTrait.Mutable then
			componentsMutable[#componentsMutable + 1] = component
			componentsMutable[componentType] = component
		elseif componentSchema.trait == CECSSchema.ComponentTrait.Serializable then
			componentsSerializable[#componentsSerializable + 1] = component
			componentsSerializable[componentType] = component
		elseif componentSchema.trait == CECSSchema.ComponentTrait.Shared then
			componentsShared[#componentsShared + 1] = component
			componentsShared[componentType] = component
		end
	end

	return components, componentsSerializable, componentsMutable, componentsConstant, componentsShared
end

local function reloadEntitiesSchema(oldEntitiesSchema, tempComponentsSchema)
	local tempEntitiesSchema = {}

	--- @class dr2c.E.EntitySchemaLoadEntities
	local e = {
		new = {},
		old = oldEntitiesSchema,
	}
	local newEntitiesSchema = e.new

	events:invoke(CECSSchema.eventEntitySchemaLoadEntities, e, nil, EEventInvocation.None)

	for state, entityType, entityComponents in GTable.sortedPairs(newEntitiesSchema) do
		local entityTypeID = state[2]

		local components, componentsSerializable, componentsMutable, componentsConstant, componentsShared =
			processEntityComponents(entityComponents, tempComponentsSchema)

		--- @type dr2c.ECSSchema.Entity
		local entitySchema = {
			type = entityType,
			typeID = entityTypeID,
			components = components,
			componentsConstant = componentsConstant,
			componentsMutable = componentsMutable,
			componentsSerializable = componentsSerializable,
			componentsShared = componentsShared,
		}

		tempEntitiesSchema[entityTypeID] = entitySchema
		tempEntitiesSchema[entityType] = entitySchema
	end

	return tempEntitiesSchema
end

function CECSSchema.reloadImmediately()
	local tempComponentsSchema = reloadComponentsSchema(componentsSchema)
	local tempEntitiesSchema = reloadEntitiesSchema(entitiesSchema, tempComponentsSchema)

	componentsSchema = tempComponentsSchema
	entitiesSchema = tempEntitiesSchema
	reloadPending = false

	-- print("ECS Schema reloaded", componentsSchema, entitiesSchema)
end

function CECSSchema.reload()
	reloadPending = true
	engine:triggerLoadPending()
end

CECSSchema.reload()

events:add(N_("ContentLoad"), function(e)
	if reloadPending then
		CECSSchema.reloadImmediately()
	end
end, "loadECSSchema", "ECS")

return CECSSchema
