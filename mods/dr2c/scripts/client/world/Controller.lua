--[[
-- @module dr2c.client.world.Controller
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Math = require("tudov.Math")

local CECS = require("dr2c.client.ecs.ECS")
local CSystemInput = require("dr2c.client.system.Input")
local CWorldTick = require("dr2c.client.world.Tick")
local GWorldPlayerInput = require("dr2c.shared.world.PlayerInput")

--- @class dr2c.CController
local CController = {}

local filterControllable = CECS.filter({
	"Controller",
	"GameObject",
})

events:add(N_("CWorldTickProcess"), function(e)
	local CECS_getComponent = CECS.getComponent

	local playerInputs = e.playerInputs
	local delta = CWorldTick.getDeltaTime() * 100

	for index, entityID, entityTypeID in CECS.iterateEntities(filterControllable) do
		local gameObject = CECS_getComponent(entityID, "GameObject")

		-- local playerID = 1

		local inputs = playerInputs[entityID]
		local dir = inputs and inputs.entry[true] and inputs.entry[true][GWorldPlayerInput.ID.Move]
		if dir then
			local dx, dy
			if dir == 0 then
				dx, dy = 0, 0
			elseif dir == 1 then
				dx, dy = 1, 0
			elseif dir == 2 then
				dx, dy = 1, 1
			elseif dir == 3 then
				dx, dy = 0, 1
			elseif dir == 4 then
				dx, dy = -1, 1
			elseif dir == 5 then
				dx, dy = -1, 0
			elseif dir == 6 then
				dx, dy = -1, -1
			elseif dir == 7 then
				dx, dy = 0, -1
			elseif dir == 8 then
				dx, dy = 1, -1
			end

			gameObject.x = gameObject.x + dx * delta
			gameObject.y = gameObject.y + dy * delta
		end
	end
end, "ControlObjects", "Move")

return CController
