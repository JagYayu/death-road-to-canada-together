local Table = require("TE.Table")

local CEntityECS = require("dr2c.Client.Entity.ECS")
local GWorldSession = require("dr2c.Shared.World.Session")

--- @class dr2c.CWorldScene
local CWorldScene = {}

--- @class dr2c.WorldSceneID : integer

--- @type dr2c.WorldSceneID[]
local worldSceneList = {}
--- @type table<dr2c.WorldSceneID, integer>
local worldSceneMap = {}

worldSceneList = persist("worldSceneList", function()
	return worldSceneList
end)
worldSceneMap = persist("worldSceneMap", function()
	return worldSceneMap
end)

function CWorldScene.getSceneList()
	return worldSceneList
end

function CWorldScene.getSceneMap()
	return worldSceneMap
end

--- @param sceneID dr2c.WorldSceneID
--- @return boolean
function CWorldScene.contains(sceneID)
	return not not worldSceneMap[sceneID]
end

local entityIDsInSceneCache = {}

local filterGameObject = { "GameObject" }

--- @param sceneID dr2c.WorldSceneID
--- @return dr2c.EntityID[]
function CWorldScene.getEntityIDsInScene(sceneID)
	local entityIDs = entityIDsInSceneCache[sceneID]
	if not entityIDs then
		entityIDs = {}

		for _, entityID in CEntityECS.iterateEntities(filterGameObject) do
			entityIDs[#entityIDs + 1] = entityID
		end

		entityIDsInSceneCache[sceneID] = entityIDs
	end

	return entityIDs
end

TE.events:add(N_("CWorldSessionStart"), function(e)
	worldSceneList = e.attributes[GWorldSession.Attribute.Scenes]
	worldSceneMap = Table.new(0, #worldSceneList)

	for index, sceneID in ipairs(worldSceneList) do
		worldSceneMap[sceneID] = index
	end
end, "LoadScenes", "Scene")

TE.events:add(N_("CWorldSessionFinish"), function(e)
	worldSceneList = {}
	worldSceneMap = {}
end, "LoadScenes", "Scene")

--- @param e dr2c.E.CWorldTickProcess
TE.events:add(N_("CWorldTickProcess"), function(e)
	if e.entitiesChanged then
		entityIDsInSceneCache = {}
	end
end, "ClearEntityIDsInSceneCache", "ECS", nil, 1)

--- @param e dr2c.E.CWorldRollback
TE.events:add(N_("CWorldRollback"), function(e)
	if not e.suppressed then
		entityIDsInSceneCache = {}
	end
end, "ClearEntityIDsInSceneCache", "Reset")

return CWorldScene
