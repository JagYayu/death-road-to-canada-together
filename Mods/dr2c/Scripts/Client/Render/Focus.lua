--[[
-- @module dr2c.Client.render.Focus
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CEntityECS = require("dr2c.Client.Entity.ECS")

local CRenderFocus = {}

--- @return dr2c.EntityID?
function CRenderFocus.getFocusedEntityID()
	return 1
end

function CRenderFocus.updateFocusedEntityID()
	-- TODO
end

--- @return dr2c.WorldSceneIndex
--- @return dr2c.WorldSceneName?
function CRenderFocus.getFocusedScene()
	-- local entityID = CRenderFocus.getFocusedEntityID()
	-- if entityID then
	-- 	local gameObject = CEntityECS.getComponent(entityID, "GameObject")
	-- 	if gameObject then
	-- 		return gameObject.room
	-- 	end
	-- end

	return 1
end

function CRenderFocus.updateFocusedSceneID()
	-- TODO
end

return CRenderFocus
