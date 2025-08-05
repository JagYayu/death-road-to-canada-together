local CEntitySchema = require("dr2c.client.system.EntitySchema")
local STable = require("dr2c.shared.utils.Table")

local CEntitySchema_componentID = CEntitySchema.componentID
local STable_deepEquals = STable.deepEquals
local assert = assert
local pairs = pairs
local type = type

--- @class dr2c.Entities
local CEntities = {}

--- @class dr2c.EntityID : integer

--- @type table<dr2c.EntityID, dr2c.EntityType>
local entities = {}
--- @type table<dr2c.ComponentTypeID, table<dr2c.EntityID, table>>
local entityComponentsMap = {}
local entityComponentsWrappers = {}

--- @param entityID dr2c.EntityID
--- @return boolean
function CEntities.exists(entityID)
	
end

--- @param entityType any
--- @return dr2c.EntityID
function CEntities.spawn(entityType) end

--- @param entityID dr2c.EntityID
--- @return boolean
function CEntities.despawn(entityID) end

local function getComponentWrapper(entityComponent)
	local wrapper = entityComponentsWrappers[entityComponent]
	if not wrapper then
		local wrapperMetatable = {
			__add = function(_, v)
				if type(v) ~= "number" then
					error("number or string expected", 2)
				end

				for index, value in pairs(entityComponent) do
					if type(value) == "number" then
						entityComponent[index] = value + v
					end
				end
			end,
			__eq = function(_, t)
				return STable_deepEquals(entityComponent, t)
			end,
			__index = function(_, k)
				local field = entityComponent[k]
				if field ~= nil then
					return field
				end

				error("Field does not exists", 2)
			end,
			__newindex = function(_, k, v)
				local field = entityComponent[k]
				if field == nil then
					error("Field does not exists", 2)
				elseif type(field) ~= type(v) then
					error("Field type mismatch", 2)
				end

				entityComponent[k] = v
			end,
		}

		wrapper = setmetatable({}, wrapperMetatable)
		STable.lockMetatable(wrapper)
		entityComponentsWrappers[entityComponent] = wrapper
	end

	return wrapper
end

--- Get an entity's component by component type id.
--- @param entityID dr2c.EntityID
--- @param componentID dr2c.ComponentTypeID
--- @return table
function CEntities.getComponentByID(entityID, componentID)
	local components = entityComponentsMap[componentID]
	if components then
		local component = components[entityID]
		if component then
			return assert(entityComponentsWrappers[component])
		end
	end

	error("component does not exists")
end

local getComponentByID = CEntities.getComponentByID

--- Get an entity's component by component type.
--- Prefer using `Entities.getComponentByID`, which has a *slightly* better performance, especially in large iterations.
--- @param entityID dr2c.EntityID
--- @param componentType dr2c.ComponentType
--- @return table
function CEntities.getComponentByType(entityID, componentType)
	local id = CEntitySchema_componentID(componentType)
	if id then
		return getComponentByID(entityID, id)
	end

	error("Invalid component " .. componentType, 2)
end

function CEntities.iterate(filterID) end

function CEntities.filter() end

return CEntities
