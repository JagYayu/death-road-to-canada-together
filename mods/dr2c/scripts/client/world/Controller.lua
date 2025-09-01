local CECS = require("dr2c.client.ecs.ECS")
local CInput = require("dr2c.client.system.Input")
local CWorldTick = require("dr2c.client.world.WorldTick")
local GPlayerInput = require("dr2c.shared.world.PlayerInput")

--- @class dr2c.CController
local CController = {}

local filterControllable = CECS.filter({
	"Controller",
	"GameObject",
})

events:add(N_("CWorldTickProcess"), function(e)
	-- print(e)

	local CECS_getComponent = CECS.getComponent

	for index, id, typeID in CECS.iterateEntities(filterControllable) do
		local gameObject = CECS_getComponent(id, "GameObject")

		local playerID = 1

		-- e.playerInputs.get(playerID, GPlayerInput.ID.Move)

		local input = e.playerInputs[playerID]
		local theta = input and input.entry[true] and input.entry[true][GPlayerInput.ID.Move]
		if theta then
			local delta = CWorldTick.getDeltaTime() * 100
			local dx = math.cos(theta)
			local dy = math.sin(theta)

			gameObject.x = gameObject.x + dx * delta
			gameObject.y = gameObject.y + dy * delta
		end
	end
end, "ControlObjects", "Move")

return CController
