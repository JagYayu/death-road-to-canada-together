--- @class dr2c.Entities
local Entities = {}

--- @class dr2c.EntityID : integer

--- @type table<dr2c.EntityID, dr2c.EntityTypeID>
local entityID2TypeMap = {}

--- @param entityID dr2c.EntityID
--- @return boolean
function Entities.exists(entityID) end

--- @param entityType any
--- @return dr2c.EntityID
function Entities.spawn(entityType) end

--- @param entityID dr2c.EntityID
--- @return boolean
function Entities.despawn(entityID) end

function Entities.component(entityID, componentName) end

function Entities.iterate(filterID) end

function Entities.filter() end

return Entities
