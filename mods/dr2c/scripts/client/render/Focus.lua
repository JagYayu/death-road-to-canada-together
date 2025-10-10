--[[
-- @module dr2c.client.render.Focus
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CECS = require("dr2c.Client.ECS.ECS")

local CRenderFocus = {}

--- @return dr2c.EntityID?
function CRenderFocus.getFocusedEntityID()
	return 1
end

--- @return dr2c.NetworkRoomID
function CRenderFocus.getFocusedRoomID()
	local entityID = CRenderFocus.getFocusedEntityID()
	if entityID then
		local gameObject = CECS.getComponent(entityID, "GameObject")
		if gameObject then
			return gameObject.room
		end
	end

	return 1
end

return CRenderFocus
