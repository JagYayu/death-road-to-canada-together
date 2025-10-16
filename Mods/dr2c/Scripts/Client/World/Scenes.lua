--[[
-- @module dr2c.Client.World.Scene
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("TE.Table")

local CEntityECS = require("dr2c.Client.Entity.ECS")
local GWorldScenes = require("dr2c.Shared.World.Scenes")
local GWorldSession = require("dr2c.Shared.World.Session")

--- @class dr2c.CWorldScenes : dr2c.WorldScenes
local CWorldScenes = GWorldScenes.new()

local entityIDsInSceneCache = {}

local filterGameObject = { "GameObject" }

--- @param sceneID dr2c.WorldSceneName
--- @return dr2c.EntityID[]
function CWorldScenes.getEntityIDsInScene(sceneID)
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
	CWorldScenes.store(e.attributes[GWorldSession.Attribute.Scenes])
end, "StoreScenes", "Scene")

TE.events:add(N_("CWorldSessionFinish"), function(e)
	CWorldScenes.clear()
end, "ClearScenes", "Scene")

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

return CWorldScenes
