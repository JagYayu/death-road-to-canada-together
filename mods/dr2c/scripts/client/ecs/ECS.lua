local CECSSchema = require("dr2c.client.ecs.ECSSchema")
local GTable = require("dr2c.shared.utils.Table")

local CECSSchema_getComponentTypeID = CECSSchema.getComponentTypeID
local CECSSchema_getComponentTrait = CECSSchema.getComponentTrait
local CECSSchema_ComponentTrait_Constant = CECSSchema.ComponentTrait.Constant
local CECSSchema_ComponentTrait_Mutable = CECSSchema.ComponentTrait.Mutable
local CECSSchema_ComponentTrait_Serializable = CECSSchema.ComponentTrait.Serializable
local CECSSchema_ComponentTrait_Shared = CECSSchema.ComponentTrait.Shared
local GTable_deepEquals = GTable.deepEquals
local assert = assert
local pairs = pairs
local stringBuffer = require("string.buffer")
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

--- @type table<dr2c.ComponentTypeID, table<dr2c.EntityID, table>>
-- local entityComponents = {}

-- local entityComponentsWrappers = {}

--- @class dr2c.EntityFilterToken : {}

--- @class dr2c.EntityFilterKey : string

--- @class dr2c.EntityFilter

--- @class dr2c.EntityFilters
--- @field [integer] dr2c.EntityFilter
--- @field [string] dr2c.EntityFilter
--- @field [dr2c.EntityFilterToken] dr2c.EntityFilter

--- @type dr2c.EntityFilters
local entityFilters = {}
local entityFilterLatestID = 0
local entityFilterIterating = false

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

	for _, component in ipairs(assert(CECSSchema.getEntityComponentsMutable(entity[1]))) do
		componentsPoolMutable[entity[1]] = stringBuffer.decode(component.data)
	end

	for _, component in ipairs(assert(CECSSchema.getEntityComponentsSerializable(entity[1]))) do
		componentsPoolSerializable[entity[1]] = stringBuffer.decode(component.data)
	end

	if type(entry.components) == "table" then
		-- TODO

		for index, value in GTable.sortedPairs(entry.components) do
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

	local index = GTable.listFindLastIf(entities, isEntityIDMatched, entityID)
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
--- @param requires string[]
--- @param excludes string[]
--- @return dr2c.EntityFilter
local function createEntityFilter(key, requires, excludes)
	--- @class dr2c.EntityFilter
	local entityFilter = {}

	entityFilter.key = key

	--- @type boolean[]
	local filterEntityTypeIDCache = GTable.new(CECSSchema.getComponentsCount(), 0)

	--- @param entityTypeID dr2c.EntityTypeID
	--- @return boolean
	function entityFilter.check(entityTypeID)
		local result = filterEntityTypeIDCache[entityTypeID]
		if result == nil then
			result = not not (
				CECSSchema.entityHasAllComponents(entityTypeID, requires)
				and CECSSchema.entityHasNonComponents(entityTypeID, excludes)
			)
			filterEntityTypeIDCache[entityTypeID] = result
		end --- @cast result boolean

		return result
	end

	return entityFilter
end

function CECS.filter(requiredComponents, excludedComponents)
	requiredComponents = requiredComponents or {}
	excludedComponents = excludedComponents or {}

	local requires = GTable.new(#requiredComponents, 0)
	local excludes = GTable.new(#excludedComponents, 0)
	for i, requiredComponent in ipairs(requiredComponents) do
		requires[i] = requiredComponent
	end
	for i, excludedComponent in ipairs(excludedComponents) do
		excludes[i] = excludedComponent
	end

	--- @type dr2c.EntityFilterKey
	local key = stringBuffer.encode({ requires, "|", excludes })
	local entityFilter = entityFilters[key]

	if not entityFilter then
		entityFilter = createEntityFilter(key, requires, excludes)

		entityFilters[#entityFilters + 1] = entityFilter
		entityFilters[key] = entityFilter
	end

	return entityFilter
end

--- @param entitiesList dr2c.Entity[]
--- @param entityFilter dr2c.EntityFilter
local function entitiesIterator(entitiesList, entityFilter, index)
	local check = entityFilter.check

	index = index + 1
	local entity = entitiesList[index]
	while entity do
		if check(entity[2]) then
			return index, entity[1], entity[2]
		else
			index = index + 1
			entity = entitiesList[index]
		end
	end
end

--- Iterate over entities that match the given filter.
--- @param entityFilter dr2c.EntityFilter
--- @return function
--- @return dr2c.Entity[]
--- @return dr2c.EntityFilter
function CECS.iterateEntities(entityFilter)
	return entitiesIterator, getSortedEntities(), entityFilter
end

function CECS.getSerialTable()
	CECS.update()

	return {
		entities = getSortedEntities(),
		entitiesLatestID = entitiesLatestID,
		components = componentsPoolSerializable,
	}
end

function CECS.setSerialTable(data)
	entities = data.entities
	entitiesLatestID = data.entitiesLatestID
	componentsPools[CECSSchema.ComponentTrait.Serializable] = data.components
end

-- events:add(event, func, name?, order?, key?, sequence?)

return CECS
