local CECSSchema = require("dr2c.client.ecs.ECSSchema")
local Table = require("tudov.Table")
local String = require("tudov.String")

local CECSSchema_ComponentTrait_Constant = CECSSchema.ComponentTrait.Constant
local CECSSchema_ComponentTrait_Mutable = CECSSchema.ComponentTrait.Mutable
local CECSSchema_ComponentTrait_Serializable = CECSSchema.ComponentTrait.Serializable
local CECSSchema_ComponentTrait_Shared = CECSSchema.ComponentTrait.Shared
local CECSSchema_entityHasAllComponents = CECSSchema.entityHasAllComponents
local CECSSchema_entityHasNonComponents = CECSSchema.entityHasNonComponents
local CECSSchema_getComponentTypeID = CECSSchema.getComponentTypeID
local CECSSchema_getComponentTrait = CECSSchema.getComponentTrait
local Table_deepEquals = Table.deepEquals
local assert = assert
local pairs = pairs
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
--- @type dr2c.EntityID
local entitiesLatestID = 0
--- @type boolean
local entitiesSorted = true
--- @type table<dr2c.EntityID, dr2c.Entity>
local entitiesID2EntityMap = {}
--- @type table
local entitiesOperations = {}

--- @class dr2c.Component
--- @field [Serializable] Serializable

--- @class dr2c.ComponentPoolTypeBased
--- @field [dr2c.EntityType] dr2c.Component

--- @class dr2c.ComponentPoolIDBased
--- @field [dr2c.EntityTypeID] dr2c.Component

--- @type dr2c.ComponentPoolTypeBased
local componentsPoolConstant = {}
--- @type dr2c.ComponentPoolIDBased
local componentsPoolMutable = {}
--- @type dr2c.ComponentPoolIDBased
local componentsPoolSerializable = {}
--- @type dr2c.ComponentPoolTypeBased
local componentsPoolShared = {}

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

entities = persist("entities", function()
	return entities
end)
entitiesLatestID = persist("entitiesLatestID", function()
	return entitiesLatestID
end)
entitiesSorted = persist("entitiesSorted", function()
	return entitiesSorted
end)
entitiesID2EntityMap = persist("entitiesID2EntityMap", function()
	return entitiesID2EntityMap
end)
entitiesOperations = persist("entitiesOperations", function()
	return entitiesOperations
end)
componentsPoolConstant = persist("componentsPoolConstant", function()
	return componentsPoolConstant
end)
componentsPoolMutable = persist("componentsPoolMutable", function()
	return componentsPoolMutable
end)
componentsPoolSerializable = persist("componentsPoolSerializable", function()
	return componentsPoolSerializable
end)
componentsPoolShared = persist("componentsPoolShared", function()
	return componentsPoolShared
end)

local eventEntitySpawned = events:new(N_("EntitySpawn"), {
	"",
})

local eventEntityDespawned = events:new(N_("EntityDespawn"), {
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

local function getSortedEntities()
	if not entitiesSorted then
		table.sort(entities, compareEntityInstance)
		entitiesSorted = true
	end

	return entities
end

--- @param entityID dr2c.EntityID
--- @return boolean
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
	local entityTypeID = CECSSchema.getEntityTypeID(entityType)
	return entityTypeID and CECS.spawnEntityByID(entityTypeID) or 0
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

	entities[#entities + 1] = entity
	entitiesSorted = false

	for _, component in ipairs(assert(CECSSchema.getEntityComponentsEntityTransient(entity[2]))) do
		componentsPoolMutable[entityID] = setmetatable({}, {
			__index = component.mergedFields,
		})
	end

	for _, component in ipairs(assert(CECSSchema.getEntityComponentsEntitySerializable(entity[2]))) do
		componentsPoolSerializable[entityID] = setmetatable({}, {
			__index = component.mergedFields,
		})
	end

	if type(entry.components) == "table" then
		-- TODO

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
	events:invoke(eventEntitySpawned, e)
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
	events:invoke(eventEntityDespawned, e)
end

local function convertEntityImpl(entry)
	-- operation, entityID, entityTypeID, ...
end

function CECS.update()
	if entitiesOperations[1] then
		for _, entry in ipairs(entitiesOperations) do
			if entry.operation == "spawn" then
				spawnEntityImpl(entry)
			elseif entry.operation == "despawn" then
				despawnEntityImpl(entry)
			elseif entry.operation == "convert" then
				convertEntityImpl(entry)
			else
				error("Invalid operation detected: " .. entry.operation)
			end
		end

		entitiesOperations = {}
	end
end

--- Get an entity's component by component type id.
--- @param entityID dr2c.EntityID
--- @param componentID dr2c.ComponentTypeID
--- @return table
function CECS.getComponentByID(entityID, componentID)
	local trait = CECSSchema_getComponentTrait(componentID)
	if trait == CECSSchema_ComponentTrait_Constant then
		return componentsPoolConstant[entitiesID2EntityMap[entityID][2]]
	elseif trait == CECSSchema_ComponentTrait_Mutable then
		return componentsPoolMutable[entityID]
	elseif trait == CECSSchema_ComponentTrait_Serializable then
		return componentsPoolSerializable[entityID]
	elseif trait == CECSSchema_ComponentTrait_Shared then
		return componentsPoolShared[entitiesID2EntityMap[entityID][2]]
	else
		error("Invalid component", 2)
	end
end

local CECS_getComponentByID = CECS.getComponentByID

--- Get an entity's component by component type.
--- Prefer using `Entities.getComponentByID`, which has a *slightly* better performance, especially in large iterations.
--- @param entityID dr2c.EntityID
--- @param componentType dr2c.ComponentType
--- @return table
function CECS.getComponentByType(entityID, componentType)
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

		entityFilters[#entityFilters + 1] = entityFilter
		entityFilters[key] = entityFilter
	end

	return entityFilter
end

--- @param entitiesList dr2c.Entity[]
--- @param iterationState dr2c.EntityIterationState
local function entitiesIterator(entitiesList, iterationState)
	local check = iterationState[1].check
	local index = iterationState[2] + 1

	local entity = entitiesList[index]
	while entity do
		if check(entity[2]) then
			iterationState[2] = index
			return iterationState, entity[1], entity[2]
		else
			index = index + 1
			entity = entitiesList[index]
		end
	end

	iterationState[2] = index
end

--- Iterate over entities that match the given filter.
--- @param entityFilter dr2c.EntityFilter
--- @return function
--- @return dr2c.Entity[]
--- @return dr2c.EntityIterationState
function CECS.iterateEntities(entityFilter)
	return entitiesIterator, getSortedEntities(), { entityFilter, 0 }
end

--- Iterate over entities and return the entity counts.
--- @param entityFilter dr2c.EntityFilter
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

--- @return table
function CECS.getSerialTable()
	CECS.update()

	return {
		entities = getSortedEntities(),
		entitiesLatestID = entitiesLatestID,
		componentsPoolSerializable = componentsPoolSerializable,
	}
end

--- @param data table
function CECS.setSerialTable(data)
	entities = data.entities
	entitiesLatestID = data.entitiesLatestID
	componentsPoolSerializable = data.componentsPoolSerializable
end

events:add(N_("CUpdate"), function(e)
	CECS.update()
end, "UpdateECS", "ECS")

events:add(N_("CSnapshotCollect"), function(e)
	e.snapshot.ecs = CECS.getSerialTable()
end, "CollectECSSerialTable", "ECS")

events:add(N_("CSnapshotDispense"), function(e)
	CECS.setSerialTable(e.snapshot.ecs)
end, "DispenseECSSerialTable", "ECS")

--- @param e dr2c.E.CLoad
events:add(N_("CLoad"), function(e)
	if not e.ecsSchema or not log.canWarn() then
		return
	end

	local componentsSchema = e.ecsSchema.components

	local function validate(validation, list, schema)
		for _, componentTypeOrID in ipairs(list) do
			if not componentsSchema[componentTypeOrID] then
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
end, "ValidateEntityFilters", "Validate")

return CECS
