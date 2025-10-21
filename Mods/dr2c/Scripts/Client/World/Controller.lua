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
local GWorldTick = require("dr2c.Shared.World.Tick")
local GWorldPlayerInput = require("dr2c.Shared.World.PlayerInput")

--- @class dr2c.CController
local CController = {}

local filterControllable = CEntityECS.filter({
	"Controller",
	"GameObject",
})

--- @param e dr2c.E.CWorldTickProcess
TE.events:add(N_("CWorldTickProcess"), function(e)
	local playersTicksInputs = e.playersTicksInputs
	if not playersTicksInputs then
		return
	end

	local getComponent = CEntityECS.getComponent

	local delta = GWorldTick.getDeltaTime() * 50

	for index, entityID, entityTypeID in CEntityECS.iterateEntities(filterControllable) do
		local gameObject = getComponent(entityID, "GameObject") --- @cast gameObject dr2c.Component.GameObject

		local ticksInputs = playersTicksInputs[entityID]
		local inputs = ticksInputs and ticksInputs[e.tick]
		local dir = inputs and inputs.map[GWorldPlayerInput.ID.Move]
		if dir then
			local dx, dy
			if dir == 1 then
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

			if dx then
				gameObject.x = gameObject.x + dx * delta
			end
			if dy then
				gameObject.y = gameObject.y + dy * delta
			end
		end
	end
end, "ControlObjects", "Move")

return CController
