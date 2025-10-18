--[[
-- @module dr2c.Client.World.Controller
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Math = require("TE.Math")

local CEntityECS = require("dr2c.Client.Entity.ECS")
local CSystemInput = require("dr2c.Client.System.Input")
local CWorldTick = require("dr2c.Client.World.Tick")
local GWorldPlayerInput = require("dr2c.Shared.World.PlayerInput")

--- @class dr2c.CController
local CController = {}

local filterControllable = CEntityECS.filter({
	"Controller",
	"GameObject",
})

--- @param e dr2c.E.CWorldTickProcess
TE.events:add(N_("CWorldTickProcess"), function(e)
	local playerInputs = e.playerInputs
	if not playerInputs then
		return
	end

	local getComponent = CEntityECS.getComponent

	local delta = CWorldTick.getDeltaTime() * 100

	for index, entityID, entityTypeID in CEntityECS.iterateEntities(filterControllable) do
		-- print(entityID, 2)
		local gameObject = getComponent(entityID, "GameObject") --- @cast gameObject dr2c.Component.GameObject

		-- local playerID = 1

		local inputs = playerInputs[entityID]
		local dir = inputs and inputs.map[GWorldPlayerInput.ID.Move]
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
