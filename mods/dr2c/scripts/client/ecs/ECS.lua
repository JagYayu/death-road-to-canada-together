--[[
-- @module dr2c.client.ecs.ECS
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CECSSchema = require("dr2c.client.ecs.ECSSchema")
local Table = require("tudov.Table")
local String = require("tudov.String")

local CECSSchema_ComponentTrait_ArchetypeConstant = CECSSchema.ComponentTrait.ArchetypeConstant
local CECSSchema_ComponentTrait_ArchetypeSerializable = CECSSchema.ComponentTrait.ArchetypeSerializable
local CECSSchema_ComponentTrait_ArchetypeTransient = CECSSchema.ComponentTrait.ArchetypeTransient
local CECSSchema_ComponentTrait_EntityTransient = CECSSchema.ComponentTrait.EntityTransient
local CECSSchema_ComponentTrait_EntitySerializable = CECSSchema.ComponentTrait.EntitySerializable
local CECSSchema_entityHasAllComponents = CECSSchema.entityHasAllComponents
local CECSSchema_entityHasNonComponents = CECSSchema.entityHasNonComponents
local CECSSchema_getComponentTypeID = CECSSchema.getComponentTypeID
local CECSSchema_getComponentTrait = CECSSchema.getComponentTrait
local CECSSchema_getEntityTypeID = CECSSchema.getEntityTypeID
local assert = assert
local setmetatable = setmetatable
local type = type

--- @class dr2c.ECS
local CECS = {}

--- @class dr2c.EntityID : integer

--- @class dr2c.Entity
--- @field [1] dr2c.EntityID
--- @field [2] dr2c.EntityTypeID

--- @type dr2c.Entity[]
local entities = {}
--- @type table<dr2c.EntityID, dr2c.Entity>
local entitiesID2EntityMap = {}
--- @type dr2c.EntityID
local entitiesLatestID = 0
--- @type boolean
local entitiesSorted = true
--- @type table
local entitiesOperations = {}

--- @alias dr2c.Component dr2c.ECSSchema.Fields

--- @class dr2c.ComponentPoolTypeBased
--- @field [dr2c.EntityType] dr2c.Component

--- @class dr2c.ComponentPoolIDBased
--- @field [dr2c.EntityTypeID] dr2c.Component

--- @type dr2c.ComponentPoolTypeBased
local componentsPoolArchetypeConstant = {}
--- @type dr2c.ComponentPoolTypeBased
local componentsPoolArchetypeSerializable = {}
--- @type dr2c.ComponentPoolTypeBased
local componentsPoolArchetypeTransient = {}
--- @type dr2c.ComponentPoolIDBased
local componentsPoolEntitySerializable = {}
--- @type dr2c.ComponentPoolIDBased
local componentsPoolEntityTransient = {}

--- @class dr2c.EntityFilterToken : {}

--- @class dr2c.EntityFilterKey : string

--- @class dr2c.EntityFilter

--- @class dr2c.EntityFilters
--- @field [integer] dr2c.EntityFilter
--- @field [string] dr2c.EntityFilter
--- @field [dr2c.EntityFilterToken] dr2c.EntityFilter

--- @class dr2c.EntityIterationState
--- @field [1] dr2c.EntityFilter
--- @field [2] integer

--- @type dr2c.EntityFilters
local entityFilters = {}

--- @type boolean
local hasNewFilter = false

--- @type boolean
local isIteratingEntities = false

--- @type table<dr2c.EntityTypeID, integer>
local typedEntitiesCountCaches = {}

entities = persist("entities", function()
	return entities
end)
entitiesID2EntityMap = persist("entitiesID2EntityMap", function()
	return entitiesID2EntityMap
end)
entitiesLatestID = persist("entitiesLatestID", function()
	return entitiesLatestID
end)
entitiesSorted = persist("entitiesSorted", function()
	return entitiesSorted
end)
entitiesOperations = persist("entitiesOperations", function()
	return entitiesOperations
end)
componentsPoolArchetypeConstant = persist("componentsPoolArchetypeConstant", function()
	return componentsPoolArchetypeConstant
end)
componentsPoolArchetypeSerializable = persist("componentsPoolArchetypeSerializable", function()
	return componentsPoolArchetypeSerializable
end)
componentsPoolArchetypeTransient = persist("componentsPoolArchetypeTransient", function()
	return componentsPoolArchetypeTransient
end)
componentsPoolEntitySerializable = persist("componentsPoolEntitySerializable", function()
	return componentsPoolEntitySerializable
end)
componentsPoolEntityTransient = persist("componentsPoolEntityTransient", function()
	return componentsPoolEntityTransient
end)

local eventEntitySpawned = TE.events:new(N_("CEntitySpawn"), {
	"",
})

local eventEntityDespawned = TE.events:new(N_("CEntityDespawn"), {
	"",
})

--- @param l dr2c.Entity
--- @param r dr2c.Entity
local function compareEntityInstance(l, r)
	if l[2] ~= r[2] then
		return l[2] < r[2]
	else
		return l[1] < r[1]
	end
end

--- @return dr2c.Entity[]
local function getSortedEntities()
	if not entitiesSorted then
		table.sort(entities, compareEntityInstance)
		entitiesSorted = true
	end

	return entities
end

--- @param entityID dr2c.EntityID
--- @return boolean
--- @nodiscard
function CECS.entityExists(entityID)
	return not not entitiesID2EntityMap[entityID]
end

--- @param entityTypeID dr2c.EntityTypeID
--- @return dr2c.EntityID
function CECS.spawnEntityByID(entityTypeID, components, ...)
	local entityID = entitiesLatestID + 1
	entitiesLatestID = entityID

	entitiesOperations[#entitiesOperations + 1] = {
		operation = "spawn",
		entity = {
			entityID,
			entityTypeID,
		},
		components = components,
		...,
	}

	return entityID
end

--- @param entityType dr2c.EntityType
--- @return dr2c.EntityID
function CECS.spawnEntity(entityType)
	local entityTypeID = CECSSchema_getEntityTypeID(entityType)
	if entityTypeID then
		return CECS.spawnEntityByID(entityTypeID)
	end

	error("Invalid entity type " .. entityType, 2)
end

--- @param entityID dr2c.EntityID
--- @return boolean
function CECS.despawnEntity(entityID, ...)
	if not entitiesID2EntityMap[entityID] then
		return false
	end

	entitiesOperations[#entitiesOperations + 1] = {
		operation = "despawn",
		entityID = entityID,
		...,
	}

	return true
end

--- @param entityID any
--- @param entityTypeID any
--- @param ... unknown
--- @return boolean
function CECS.convertEntityByID(entityID, entityTypeID, ...)
	if entitiesID2EntityMap[entityID] == entityTypeID then
		return false
	end

	entitiesOperations[#entitiesOperations + 1] = {
		operation = "convert",
		entityID = entityID,
		entityTypeID = entityTypeID,
		...,
	}

	return true
end

--- @param entityID dr2c.EntityID
--- @param entityType dr2c.EntityType
--- @return boolean
function CECS.convertEntity(entityID, entityType, ...)
	local entityTypeID = CECSSchema.getEntityTypeID(entityType)
	return entityTypeID and CECS.convertEntityByID(entityID, entityType, ...) or false
end

local function spawnEntityImpl(entry)
	local entity = entry.entity
	local entityID = entity[1]
	local entityTypeID = entity[2]

	assert(entityID == #entities + 1)

	entities[entityID] = entity
	entitiesID2EntityMap[entityID] = entity
	entitiesSorted = false

	local bufferDecode = String.bufferDecode

	for _, component in ipairs(assert(CECSSchema.getEntityComponentsEntityTransient(entity[2]))) do
		componentsPoolEntityTransient[entityID] = bufferDecode(component.mergedFieldsBuffer)
	end

	for _, component in ipairs(assert(CECSSchema.getEntityComponentsEntitySerializable(entity[2]))) do
		componentsPoolEntitySerializable[entityID] = bufferDecode(component.mergedFieldsBuffer)
	end

	if type(entry.components) == "table" then
		-- TODO
		error("not implement yet", 2)

		for _, index, value in Table.sortedPairs(entry.components) do
			--
		end
	end

	--- @class dr2c.E.EntitySpawned
	local e = {
		entityID = entityID,
		entityTypeID = entityTypeID,
		components = entry.components,
		unpack(entry),
	}
	TE.events:invoke(eventEntitySpawned, e)
end

local function isEntityIDMatched(entity, entityID)
	return entity[1] == entityID
end

local function despawnEntityImpl(entry)
	local entity = entry.entity
	local entityID = entity[1]
	local entityTypeID = entity[2]

	local index = Table.listFindLastIf(entities, isEntityIDMatched, entityID)
	if not index then
		return
	end

	entities[index] = entities[#entities]
	entities[#entities] = nil
	entitiesSorted = false

	--- @class dr2c.E.EntityDespawned
	local e = {
		entityID = entityID,
		entityTypeID = entityTypeID,
		unpack(entry),
	}
	TE.events:invoke(eventEntityDespawned, e)
end

local function convertEntityImpl(entry)
	error("Entity conversion is not implement yet!")
	-- operation, entityID, entityTypeID, ...
end

function CECS.update()
	if isIteratingEntities then
		error("Attempt to update ECS while iterating entities", 2)
	end

	if entitiesOperations[1] then
		for _, entry in ipairs(entitiesOperations) do
			if entry.operation == "spawn" then
				spawnEntityImpl(entry)
			elseif entry.operation == "despawn" then
				despawnEntityImpl(entry)
			elseif entry.operation == "convert" then
				convertEntityImpl(entry)
			else
				error(("Invalid operation detected: %s"):format(entry.operation))
			end
		end

		entitiesOperations = {}

		return true
	else
		return false
	end
end

--- Get an entity's component by component type id.
--- @generic T : table
--- @param entityID dr2c.EntityID
--- @param componentID dr2c.ComponentTypeID
--- @return table
function CECS.getComponentByID(entityID, componentID)
	local trait = CECSSchema_getComponentTrait(componentID)
	if trait == CECSSchema_ComponentTrait_ArchetypeConstant then
		return componentsPoolArchetypeConstant[entitiesID2EntityMap[entityID][2]]
	elseif trait == CECSSchema_ComponentTrait_ArchetypeSerializable then
		return componentsPoolArchetypeSerializable[entitiesID2EntityMap[entityID][2]]
	elseif trait == CECSSchema_ComponentTrait_ArchetypeTransient then
		return componentsPoolArchetypeTransient[entitiesID2EntityMap[entityID][2]]
	elseif trait == CECSSchema_ComponentTrait_EntityTransient then
		return componentsPoolEntityTransient[entityID]
	elseif trait == CECSSchema_ComponentTrait_EntitySerializable then
		return componentsPoolEntitySerializable[entityID]
	else
		error("Invalid component", 2)
	end
end

local CECS_getComponentByID = CECS.getComponentByID

--- Get an entity's component by component type.
--- Prefer using `Entities.getComponentByID`, which has a *slightly* better performance, especially in large iterations.
--- @generic T : table
--- @param entityID dr2c.EntityID
--- @param componentType dr2c.ComponentType
--- @return T?
function CECS.getComponent(entityID, componentType)
	return CECS_getComponentByID(entityID, CECSSchema_getComponentTypeID(componentType) or 0)
end

--- @param key string
--- @param requires (dr2c.ComponentType | dr2c.ComponentTypeID)[]
--- @param excludes (dr2c.ComponentType | dr2c.ComponentTypeID)[]
--- @return dr2c.EntityFilter
local function createEntityFilter(key, requires, excludes)
	--- @class dr2c.EntityFilter
	local entityFilter = {}

	entityFilter.key = key

	if log.canWarn() then
		entityFilter.validation = {
			requires = requires,
			excludes = excludes,
			traceback = debug.traceback("", 3),
		}
	else
		entityFilter.validation = nil
	end

	--- @type boolean[]
	local filterEntityTypeIDCache = Table.new(CECSSchema.getComponentsCount(), 0)

	--- @param entityTypeID dr2c.EntityTypeID
	--- @return boolean
	function entityFilter.check(entityTypeID)
		local result = filterEntityTypeIDCache[entityTypeID]

		if result == nil then
			result = not not (
				CECSSchema_entityHasAllComponents(entityTypeID, requires)
				and CECSSchema_entityHasNonComponents(entityTypeID, excludes)
			)
			filterEntityTypeIDCache[entityTypeID] = result
		end --- @cast result boolean

		return result
	end

	return entityFilter
end

--- @param requiredComponents (dr2c.ComponentType | dr2c.ComponentTypeID)[]?
--- @param excludedComponents (dr2c.ComponentType | dr2c.ComponentTypeID)[]?
--- @return dr2c.EntityFilter
function CECS.filter(requiredComponents, excludedComponents)
	requiredComponents = requiredComponents or {}
	excludedComponents = excludedComponents or {}

	local requires = Table.new(#requiredComponents, 0)
	local excludes = Table.new(#excludedComponents, 0)
	for i, requiredComponent in ipairs(requiredComponents) do
		requires[i] = requiredComponent
	end
	for i, excludedComponent in ipairs(excludedComponents) do
		excludes[i] = excludedComponent
	end

	--- @type dr2c.EntityFilterKey
	local key = String.bufferEncode({ requires, "|", excludes })
	local entityFilter = entityFilters[key]

	if not entityFilter then
		entityFilter = createEntityFilter(key, requires, excludes)
		hasNewFilter = true

		entityFilters[#entityFilters + 1] = entityFilter
		entityFilters[key] = entityFilter
	end

	return entityFilter
end

--- @param entityFilter dr2c.EntityFilter
--- @param index integer
--- @return integer? index
--- @return dr2c.EntityID id
--- @return dr2c.EntityTypeID typeID
local function entitiesIterator(entityFilter, index)
	local check = entityFilter.check

	local entity = entities[index]
	while entity do
		if check(entity[2]) then
			return index + 1, entity[1], entity[2]
		else
			index = index + 1
			entity = entities[index]
		end
	end

	isIteratingEntities = false
	return nil, 0, 0
end

--- Iterate over entities that match the given filter.
--- You must use the returned values in a for loop iteration, otherwise a warning will pop up while update ECS.
--- @param entityFilter dr2c.EntityFilter
--- @return fun(entityFilter: dr2c.EntityFilter, index: integer): (index: integer, id: dr2c.EntityID, typeID: dr2c.EntityTypeID) iterator
--- @return dr2c.EntityFilter entityFilter
--- @return integer index
--- @nodiscard
function CECS.iterateEntities(entityFilter)
	getSortedEntities()
	isIteratingEntities = true
	return entitiesIterator, entityFilter, 1
end

--- @param entityTypeOrID dr2c.EntityTypeOrID
--- @return dr2c.EntityTypeID
local function toEntityTypeID(entityTypeOrID)
	if type(entityTypeOrID) == "number" then
		return entityTypeOrID
	elseif type(entityTypeOrID) == "string" then
		local typeID = CECSSchema_getEntityTypeID(entityTypeOrID)
		if typeID then
			return typeID
		else
			error("Invalid entity type", 2)
		end
	else
		error("Invalid parameter", 3)
	end
end

--- @param entityTypeID dr2c.EntityTypeID
--- @param index integer
--- @return integer? index
--- @return dr2c.EntityID id
local function entitiesTypedIterator(entityTypeID, index)
	local entity = entities[index]
	while entity do
		if entity[2] == entityTypeID then
			return index + 1, entity[1]
		else
			index = index + 1
			entity = entities[index]
		end
	end

	isIteratingEntities = false
	return nil, 0
end

--- @param entityTypeOrID any
--- @return fun(entityTypeID: dr2c.EntityTypeID, index: integer): (index: integer, id: dr2c.EntityID) iterator
--- @return integer
--- @return dr2c.EntityTypeID
--- @nodiscard
function CECS.iterateEntitiesByType(entityTypeOrID)
	getSortedEntities()
	isIteratingEntities = true
	return entitiesTypedIterator, toEntityTypeID(entityTypeOrID), 1
end

--- @return integer count
function CECS.countTotal()
	return #entities
end

--- Iterate over entities and return the entity counts.
--- @param entityFilter dr2c.EntityFilter
--- @nodiscard
function CECS.countEntities(entityFilter)
	local counter = 0
	local check = entityFilter.check

	for _, entity in ipairs(getSortedEntities()) do
		if check(entity[2]) then
			counter = counter + 1
		end
	end

	return counter
end

--- @param entityTypeOrID dr2c.EntityType
--- @return integer
--- @nodiscard
function CECS.countEntitiesByType(entityTypeOrID)
	local counter = 0
	local entityTypeID = toEntityTypeID(entityTypeOrID)

	for _, entity in ipairs(getSortedEntities()) do
		if entity[2] == entityTypeID then
			counter = counter + 1
		end
	end

	return counter
end

--- @param entitySchemaField string
--- @return table
local function allocateArchetypeComponentsPool(entitySchemaField)
	local entitiesSchema = CECSSchema.getEntitiesSchema()
	local componentsPool = Table.new(#entitiesSchema, 0)

	for entityTypeID, entitySchema in ipairs(entitiesSchema) do
		assert(entityTypeID == entitySchema.typeID)

		for _, component in ipairs(entitySchema[entitySchemaField]) do
			componentsPool[entityTypeID] = setmetatable({}, component.metatable)
		end
	end

	return componentsPool
end

function CECS.clearEntities()
	entities = {}
	entitiesID2EntityMap = {}
	entitiesLatestID = 0
	entitiesSorted = true
	entitiesOperations = {}
	componentsPoolArchetypeSerializable = allocateArchetypeComponentsPool("componentsArchetypeSerializable")
	componentsPoolArchetypeTransient = allocateArchetypeComponentsPool("componentsArchetypeTransient")
	componentsPoolEntitySerializable = {}
	componentsPoolEntityTransient = {}
end

--- @return table
--- @nodiscard
function CECS.getSerialTable()
	CECS.update()

	return {
		getSortedEntities(),
		entitiesLatestID,
		componentsPoolEntitySerializable,
		componentsPoolArchetypeSerializable,
	}
end

--- @param data table
function CECS.setSerialTable(data)
	entities = data[1]
	entitiesLatestID = data[2]
	componentsPoolEntitySerializable = data[3]
	componentsPoolArchetypeSerializable = data[4]
end

--- @param e dr2c.E.CWorldTickProcess
TE.events:add(N_("CWorldTickProcess"), function(e)
	if isIteratingEntities then
		isIteratingEntities = false

		if log.canWarn() then
			log.warn("Attempting to update ECS, but flag regarding entity iteration remains on.\
Did an error occurred while iterating entities? Or not calling iterators inside a loop?")
		end
	end

	e.entitiesChanged = CECS.update()
end, "UpdateECS", "ECS")

TE.events:add(N_("CConnect"), CECS.clearEntities, "InitializeECS", "Initialize")

TE.events:add(N_("CDisconnect"), CECS.clearEntities, "ResetECS", "Reset")

TE.events:add(N_("CSnapshotCollect"), function(e)
	e.snapshot.ecs = CECS.getSerialTable()
end, "CollectECSSerialTable", "ECS")

TE.events:add(N_("CSnapshotDispense"), function(e)
	CECS.setSerialTable(e.snapshot.ecs)
end, "DispenseECSSerialTable", "ECS")

--- @param e {}
TE.events:add(N_("CEntitySchemaLoaded"), function(e)
	componentsPoolArchetypeConstant = allocateArchetypeComponentsPool("componentsArchetypeConstant")
end, "ResetArchetypeConstantComponents", "Components")

--- @param e {}
TE.events:add(N_("CEntitySchemaLoaded"), function(e)
	if not hasNewFilter or not log.canWarn() then
		return
	end

	local componentsSchema = CECSSchema.getComponentsSchema()

	local function validate(validation, list)
		for _, componentTypeOrID in ipairs(list) do
			if not componentsSchema[componentTypeOrID] and log.canWarn() then
				local fmt = "Invalid component '%s' detected in entity filter%s"
				log.warn(fmt:format(componentTypeOrID, validation.traceback))
			end
		end
	end

	for _, entityFilter in ipairs(entityFilters) do
		if entityFilter.validation then
			validate(entityFilter.validation, entityFilter.validation.requires)
			validate(entityFilter.validation, entityFilter.validation.excludes)

			entityFilter.validation = nil
		end
	end
end, "ValidateEntityFilters", "Filters")

return CECS
