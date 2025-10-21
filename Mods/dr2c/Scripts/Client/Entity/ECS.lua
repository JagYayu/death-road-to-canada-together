--[[
-- @module dr2c.Client.Entity.ECS
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local List = require("TE.List")
local Table = require("TE.Table")
local String = require("TE.String")

local CEntityECSSchema = require("dr2c.Client.Entity.ECSSchema")

local CEntityECSSchema_ComponentTrait_ArchetypeConstant = CEntityECSSchema.ComponentTrait.ArchetypeConstant
local CEntityECSSchema_ComponentTrait_ArchetypeSerializable = CEntityECSSchema.ComponentTrait.ArchetypeSerializable
local CEntityECSSchema_ComponentTrait_ArchetypeTransient = CEntityECSSchema.ComponentTrait.ArchetypeTransient
local CEntityECSSchema_ComponentTrait_EntityTransient = CEntityECSSchema.ComponentTrait.EntityTransient
local CEntityECSSchema_ComponentTrait_EntitySerializable = CEntityECSSchema.ComponentTrait.EntitySerializable
local CEntityECSSchema_entityHasAllComponents = CEntityECSSchema.entityHasAllComponents
local CEntityECSSchema_entityHasNonComponents = CEntityECSSchema.entityHasNonComponents
local CEntityECSSchema_getComponentTypeID = CEntityECSSchema.getComponentTypeID
local CEntityECSSchema_getComponentTrait = CEntityECSSchema.getComponentTrait
local CEntityECSSchema_getEntityTypeID = CEntityECSSchema.getEntityTypeID
local String_bufferDecode = String.bufferDecode
local assert = assert
local ipairs = ipairs
local setmetatable = setmetatable
local type = type

--- @class dr2c.EntityECS
local CEntityECS = {}

--- @class dr2c.EntityID : integer

--- @class dr2c.Entity
--- @field [1] dr2c.EntityID
--- @field [2] dr2c.EntityTypeID

local entityFieldID = 1
local entityFieldTypeID = 2

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

--- @class dr2c.EntityFilterKey : string

--- @class dr2c.EntityFilter
--- @field key string

--- @class dr2c.EntityFilters
--- @field [integer] dr2c.EntityFilter
--- @field [string] dr2c.EntityFilter

--- @class dr2c.EntityIterationState
--- @field [1] dr2c.EntityFilter
--- @field [2] integer

--- @class dr2c.ECSSerialTable

--- @type table<dr2c.ComponentTypeID, dr2c.ComponentPoolTypeBased>
local componentsPoolArchetypeConstant = {}
--- @type table<dr2c.ComponentTypeID, dr2c.ComponentPoolTypeBased>
local componentsPoolArchetypeSerializable = {}
--- @type table<dr2c.ComponentTypeID, dr2c.ComponentPoolTypeBased>
local componentsPoolArchetypeTransient = {}
--- @type table<dr2c.ComponentTypeID, dr2c.ComponentPoolIDBased>
local componentsPoolEntitySerializable = {}
--- @type table<dr2c.ComponentTypeID, dr2c.ComponentPoolIDBased>
local componentsPoolEntityTransient = {}

--- @type dr2c.EntityFilters
local entityFilters = {}
--- @type table<dr2c.EntityFilter, table>
local entityFilterValidations = setmetatable({}, { __mode = "k" })
--- @type table<dr2c.EntityFilter, fun(entityTypeID: dr2c.EntityTypeID): boolean>
local entityFilterCheckFunctions = setmetatable({}, { __mode = "k" })

--- @type boolean
local isIteratingEntities = false

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

CEntityECS.eventEntitySpawned = TE.events:new(N_("CEntitySpawn"), {
	"", -- TODO
})

CEntityECS.eventEntityDespawned = TE.events:new(N_("CEntityDespawn"), {
	"", -- TODO
})

local function newIDBasedComponentPool()
	return {}
end

--- @param l dr2c.Entity
--- @param r dr2c.Entity
--- @nodiscard
local function compareEntityInstance(l, r)
	if l[entityFieldTypeID] ~= r[entityFieldTypeID] then
		return l[entityFieldTypeID] < r[entityFieldTypeID]
	else
		return l[entityFieldID] < r[entityFieldID]
	end
end

--- @return dr2c.Entity[]
local function sortAndGetEntities()
	if not entitiesSorted then
		table.sort(entities, compareEntityInstance)
		entitiesSorted = true
	end

	return entities
end

--- @param entityID dr2c.EntityID
--- @return boolean
--- @nodiscard
function CEntityECS.entityExists(entityID)
	return not not entitiesID2EntityMap[entityID]
end

--- @param entityTypeID dr2c.EntityTypeID
--- @return dr2c.EntityID
function CEntityECS.spawnEntityByID(entityTypeID, components, ...)
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
function CEntityECS.spawnEntity(entityType, components, ...)
	local entityTypeID = CEntityECSSchema_getEntityTypeID(entityType)
	if entityTypeID then
		return CEntityECS.spawnEntityByID(entityTypeID, components, ...)
	end

	error("Invalid entity type " .. entityType, 2)
end

--- @param entityID dr2c.EntityID
--- @return boolean
function CEntityECS.despawnEntity(entityID, ...)
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
function CEntityECS.convertEntityByID(entityID, entityTypeID, ...)
	if entitiesID2EntityMap[entityID][entityFieldTypeID] == entityTypeID then
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

local CEntityECS_convertEntityByID = CEntityECS.convertEntityByID

--- @param entityID dr2c.EntityID
--- @param entityType dr2c.EntityType
--- @return boolean
function CEntityECS.convertEntity(entityID, entityType, ...)
	local entityTypeID = CEntityECSSchema.getEntityTypeID(entityType)
	return entityTypeID and CEntityECS_convertEntityByID(entityID, entityType, ...) or false
end

local function spawnEntityImpl(entry)
	local entity = entry.entity
	local entityID = entity[entityFieldID]
	local entityTypeID = entity[entityFieldTypeID]

	assert(entityID == #entities + 1)

	entities[entityID] = entity
	entitiesID2EntityMap[entityID] = entity
	entitiesSorted = false

	for _, component in ipairs(assert(CEntityECSSchema.getEntityComponentsEntityTransient(entity[2]))) do
		componentsPoolEntityTransient[component.componentSchema.typeID][entityID] =
			String_bufferDecode(component.mergedFieldsBuffer)
	end

	for _, component in ipairs(assert(CEntityECSSchema.getEntityComponentsEntitySerializable(entity[2]))) do
		componentsPoolEntitySerializable[component.componentSchema.typeID][entityID] =
			String_bufferDecode(component.mergedFieldsBuffer)
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
	TE.events:invoke(CEntityECS.eventEntitySpawned, e)
end

local function isEntityIDMatched(entity, entityID)
	return entity[entityFieldID] == entityID
end

local function despawnEntityImpl(entry)
	local entity = entry.entity
	local entityID = entity[entityFieldID]
	local entityTypeID = entity[entityFieldTypeID]

	local index = List.findLastIfV(entities, isEntityIDMatched, entityID)
	if not index then
		return
	end

	for _, component in ipairs(assert(CEntityECSSchema.getEntityComponentsEntityTransient(entity[2]))) do
		componentsPoolEntityTransient[component.componentSchema.typeID][entityID] = nil
	end

	for _, component in ipairs(assert(CEntityECSSchema.getEntityComponentsEntitySerializable(entity[2]))) do
		componentsPoolEntitySerializable[component.componentSchema.typeID][entityID] = nil
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
	TE.events:invoke(CEntityECS.eventEntityDespawned, e)
end

local function convertEntityImpl(entry)
	error("Entity conversion is not implement yet!")
	-- operation, entityID, entityTypeID, ...
end

--- Update entities, execute all pending operations in queue.
--- @return boolean operated
function CEntityECS.update()
	if isIteratingEntities then
		error("Attempt to update ECS while iterating entities", 2)
	end

	-- print("componentsPoolArchetypeConstant", componentsPoolArchetypeConstant)
	-- print("componentsPoolArchetypeSerializable", componentsPoolArchetypeSerializable)
	-- print("componentsPoolArchetypeTransient", componentsPoolArchetypeTransient)
	-- print("componentsPoolEntitySerializable", componentsPoolEntitySerializable)
	-- print("componentsPoolEntityTransient", componentsPoolEntityTransient)

	if entitiesOperations[1] then
		for _, entry in ipairs(entitiesOperations) do
			if entry.operation == "spawn" then
				spawnEntityImpl(entry)
			elseif entry.operation == "despawn" then
				despawnEntityImpl(entry)
			elseif entry.operation == "convert" then
				convertEntityImpl(entry)
			elseif log.canWarn() then
				log.warn(("Invalid operation detected: %s"):format(entry.operation))
			end
		end

		entitiesOperations = {}

		return true
	else
		return false
	end
end

local getComponentByIDHandles = {
	[CEntityECSSchema_ComponentTrait_ArchetypeConstant] = function(entityID, componentTypeID)
		return componentsPoolArchetypeConstant[componentTypeID][entitiesID2EntityMap[entityID][2]]
	end,
	[CEntityECSSchema_ComponentTrait_ArchetypeSerializable] = function(entityID, componentTypeID)
		return componentsPoolArchetypeSerializable[componentTypeID][entitiesID2EntityMap[entityID][2]]
	end,
	[CEntityECSSchema_ComponentTrait_ArchetypeTransient] = function(entityID, componentTypeID)
		return componentsPoolArchetypeTransient[componentTypeID][entitiesID2EntityMap[entityID][2]]
	end,
	[CEntityECSSchema_ComponentTrait_EntityTransient] = function(entityID, componentTypeID)
		return componentsPoolEntityTransient[componentTypeID][entityID]
	end,
	[CEntityECSSchema_ComponentTrait_EntitySerializable] = function(entityID, componentTypeID)
		return componentsPoolEntitySerializable[componentTypeID][entityID]
	end,
}

--- Get an entity's component by component type id.
--- @generic T : table
--- @param entityID dr2c.EntityID
--- @param componentTypeID dr2c.ComponentTypeID
--- @return table
function CEntityECS.getComponentByID(entityID, componentTypeID)
	local trait = CEntityECSSchema_getComponentTrait(componentTypeID)
	local handle = getComponentByIDHandles[trait]

	if handle then
		return handle(entityID, componentTypeID)
	else
		error(("Invalid component id: %d"):format(componentTypeID), 2)
	end
end

local CECS_getComponentByID = CEntityECS.getComponentByID

--- Get an entity's component by component type.
--- Prefer using `Entities.getComponentByID`, which has a *slightly* better performance, especially in large iterations.
--- @generic T : table
--- @param entityID dr2c.EntityID
--- @param componentType dr2c.ComponentType
--- @return T?
function CEntityECS.getComponent(entityID, componentType)
	local typeID = CEntityECSSchema_getComponentTypeID(componentType)
	if typeID then
		return CECS_getComponentByID(entityID, typeID)
	else
		error(("Invalid component %s"):format(componentType), 2)
	end
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
		entityFilterValidations[entityFilter] = {
			requires = requires,
			excludes = excludes,
			traceback = debug.traceback("", 3),
		}
	end

	--- @type boolean[]
	local filterEntityTypeIDCache = Table.new(CEntityECSSchema.getComponentsCount(), 0)

	--- @param entityTypeID dr2c.EntityTypeID
	--- @return boolean
	entityFilterCheckFunctions[entityFilter] = function(entityTypeID)
		local result = filterEntityTypeIDCache[entityTypeID]

		if result == nil then
			result = not not (
				CEntityECSSchema_entityHasAllComponents(entityTypeID, requires)
				and CEntityECSSchema_entityHasNonComponents(entityTypeID, excludes)
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
function CEntityECS.filter(requiredComponents, excludedComponents)
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

--- @param entityFilter dr2c.EntityFilter
--- @param index integer
--- @return integer? index
--- @return dr2c.EntityID id
--- @return dr2c.EntityTypeID typeID
local function entitiesIterator(entityFilter, index)
	local check = entityFilterCheckFunctions[entityFilter]

	local entity = entities[index]
	while entity do
		if check(entity[2]) then
			return index + 1, entity[entityFieldID], entity[entityFieldTypeID]
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
function CEntityECS.iterateEntities(entityFilter)
	sortAndGetEntities()
	isIteratingEntities = true
	return entitiesIterator, entityFilter, 1
end

--- @param entityTypeOrID dr2c.EntityTypeOrID
--- @return dr2c.EntityTypeID
local function toEntityTypeID(entityTypeOrID)
	if type(entityTypeOrID) == "number" then
		return entityTypeOrID
	elseif type(entityTypeOrID) == "string" then
		local typeID = CEntityECSSchema_getEntityTypeID(entityTypeOrID)
		if typeID then
			return typeID
		else
			error("Invalid entity type", 3)
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
		if entity[entityFieldTypeID] == entityTypeID then
			return index + 1, entity[entityFieldID]
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
function CEntityECS.iterateEntitiesByType(entityTypeOrID)
	sortAndGetEntities()
	isIteratingEntities = true
	return entitiesTypedIterator, toEntityTypeID(entityTypeOrID), 1
end

--- @return integer count
--- @nodiscard
function CEntityECS.countTotal()
	return #entities
end

--- Iterate over entities and return the entity counts.
--- @param entityFilter dr2c.EntityFilter
--- @nodiscard
function CEntityECS.countEntities(entityFilter)
	local counter = 0
	local check = entityFilterCheckFunctions[entityFilter]

	for _, entity in ipairs(sortAndGetEntities()) do
		if check(entity[2]) then
			counter = counter + 1
		end
	end

	return counter
end

--- @param entityTypeOrID dr2c.EntityType
--- @return integer
--- @nodiscard
function CEntityECS.countEntitiesByType(entityTypeOrID)
	local counter = 0
	local entityTypeID = toEntityTypeID(entityTypeOrID)

	for _, entity in ipairs(sortAndGetEntities()) do
		if entity[2] == entityTypeID then
			counter = counter + 1
		end
	end

	return counter
end

--- @param entitySchemaField string
--- @return dr2c.ComponentPoolTypeBased
local function newTypeBasedComponentPool(entitySchemaField)
	local entitiesSchema = CEntityECSSchema.getEntitiesSchema()
	local componentsPool = Table.new(#entitiesSchema, 0)

	for entityTypeID, entitySchema in ipairs(entitiesSchema) do
		assert(entityTypeID == entitySchema.typeID)

		for _, component in ipairs(entitySchema[entitySchemaField]) do
			componentsPool[entityTypeID] = setmetatable({}, component.metatable)
		end
	end

	return componentsPool
end

function CEntityECS.clearEntities()
	entities = {}
	entitiesID2EntityMap = {}
	entitiesLatestID = 0
	entitiesSorted = true
	entitiesOperations = {}

	for typeID in pairs(componentsPoolArchetypeSerializable) do
		componentsPoolArchetypeSerializable[typeID] = newTypeBasedComponentPool("componentsArchetypeSerializable")
	end
	for typeID in pairs(componentsPoolArchetypeTransient) do
		componentsPoolArchetypeTransient[typeID] = newTypeBasedComponentPool("componentsArchetypeTransient")
	end
	for typeID in pairs(componentsPoolEntitySerializable) do
		componentsPoolEntitySerializable[typeID] = newIDBasedComponentPool()
	end
	for typeID in pairs(componentsPoolEntityTransient) do
		componentsPoolEntityTransient[typeID] = newIDBasedComponentPool()
	end
end

--- @return dr2c.ECSSerialTable
--- @nodiscard
function CEntityECS.getSerialTable()
	CEntityECS.update()

	return {
		sortAndGetEntities(),
		entitiesLatestID,
		componentsPoolEntitySerializable,
		componentsPoolArchetypeSerializable,
	}
end

--- @param data dr2c.ECSSerialTable
function CEntityECS.setSerialTable(data)
	entities = data[1]
	entitiesLatestID = data[2]
	componentsPoolEntitySerializable = data[3]
	componentsPoolArchetypeSerializable = data[4]

	entitiesSorted = true
	entitiesID2EntityMap = {}
	for _, entity in ipairs(entities) do
		entitiesID2EntityMap[entity[entityFieldID]] = entity
	end
end

local utilityFieldPool = 1
local utilityFieldNewPool = 2

local componentTrait2Utility = {
	[CEntityECSSchema_ComponentTrait_ArchetypeConstant] = {
		componentsPoolArchetypeConstant,
		function()
			return newTypeBasedComponentPool("componentsArchetypeConstant")
		end,
	},
	[CEntityECSSchema_ComponentTrait_ArchetypeSerializable] = {
		componentsPoolArchetypeSerializable,
		function()
			return newTypeBasedComponentPool("componentsArchetypeSerializable")
		end,
	},
	[CEntityECSSchema_ComponentTrait_ArchetypeTransient] = {
		componentsPoolArchetypeTransient,
		function()
			return newTypeBasedComponentPool("componentsArchetypeTransient")
		end,
	},
	[CEntityECSSchema_ComponentTrait_EntitySerializable] = {
		componentsPoolEntitySerializable,
		newIDBasedComponentPool,
	},
	[CEntityECSSchema_ComponentTrait_EntityTransient] = {
		componentsPoolEntityTransient,
		newIDBasedComponentPool,
	},
}

--#region Events

--- @param e dr2c.E.CWorldTickProcess
TE.events:add(N_("CWorldTickProcess"), function(e)
	if isIteratingEntities then
		isIteratingEntities = false

		if log.canWarn() then
			log.warn("Attempting to update ECS, but flag regarding entity iteration remains on.\
Did an error occurred while iterating entities? Or not calling iterators inside a loop?")
		end
	end

	--- @class dr2c.E.CWorldTickProcess
	--- @field entitiesChanged boolean?
	e = e

	e.entitiesChanged = CEntityECS.update()
end, "UpdateECS", "ECS")

TE.events:add(N_("CConnect"), CEntityECS.clearEntities, "ResetECS", "Reset")
TE.events:add(N_("CDisconnect"), CEntityECS.clearEntities, "ResetECS", "Reset")
TE.events:add(N_("CWorldSessionStart"), CEntityECS.clearEntities, "ResetECS", "Reset")
TE.events:add(N_("CWorldSessionFinish"), CEntityECS.clearEntities, "ResetECS", "Reset")

TE.events:add(N_("CSnapshotCollect"), function(e)
	e.snapshot.ecs = CEntityECS.getSerialTable()
end, "CollectECSSerialTable", "ECS")

TE.events:add(N_("CSnapshotDispense"), function(e)
	CEntityECS.setSerialTable(e.snapshot.ecs)
end, "DispenseECSSerialTable", "ECS")

--- @param e {}
TE.events:add(N_("CEntitySchemaLoaded"), function(e)
	local componentsSchema = CEntityECSSchema.getComponentsSchema()

	local function removeComponentPoolIfNotExists(pool)
		for componentTypeID in pairs(pool) do
			if not componentsSchema[componentTypeID] then
				pool[componentTypeID] = nil
			end
		end
	end
	removeComponentPoolIfNotExists(componentsPoolArchetypeConstant)
	removeComponentPoolIfNotExists(componentsPoolArchetypeSerializable)
	removeComponentPoolIfNotExists(componentsPoolArchetypeTransient)
	removeComponentPoolIfNotExists(componentsPoolEntityTransient)
	removeComponentPoolIfNotExists(componentsPoolEntitySerializable)

	for componentTypeID, componentSchema in ipairs(componentsSchema) do
		local trait = componentSchema.trait
		if trait then
			local utility = componentTrait2Utility[trait]
			local pool = utility[utilityFieldPool]
			--- @diagnostic disable-next-line: need-check-nil
			pool[componentTypeID] = pool[componentTypeID] or utility[utilityFieldNewPool]()
		end
	end

	if not entityFilters[1] or not log.canWarn() then
		return
	end

	local function validate(validation, list)
		for _, componentTypeOrID in ipairs(list) do
			if not componentsSchema[componentTypeOrID] and log.canWarn() then
				local fmt = "Invalid component '%s' detected in entity filter%s"
				log.warn(fmt:format(componentTypeOrID, validation.traceback))
			end
		end
	end

	for _, entityFilter in ipairs(entityFilters) do
		local validation = entityFilterValidations[entityFilter]
		if validation then
			validate(validation, validation.requires)
			validate(validation, validation.excludes)

			entityFilterValidations[entityFilter] = nil
		end
	end
end, "ValidateEntityFilters", "ECS")

--#endregion

return CEntityECS
