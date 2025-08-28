local CECS = require("dr2c.client.ecs.ECS")
local CInput = require("dr2c.client.system.Input")

--- @class dr2c.CController
local CController = {}

local filterControllable = CECS.filter({
	"Controller",
})

events:add(N_("CUpdate"), function(e)
	-- local dx = 0
	-- local dy = 0
	-- local speed = 4

	-- if CInput.isScanCodeHeld(EScanCode.A) then
	-- 	dx = dx - 1
	-- end
	-- if CInput.isScanCodeHeld(EScanCode.D) then
	-- 	dx = dx + 1
	-- end
	-- if CInput.isScanCodeHeld(EScanCode.W) then
	-- 	dy = dy - 1
	-- end
	-- if CInput.isScanCodeHeld(EScanCode.S) then
	-- 	dy = dy + 1
	-- end

	-- local CECS_getComponent = CECS.getComponent

	-- for index, id, typeID in CECS.iterateEntities(filterControllable) do
	-- 	local gameObject = CECS_getComponent(id, "GameObject")
	-- 	gameObject.x = gameObject.x + dx * speed
	-- 	gameObject.y = gameObject.y + dy * speed
	-- end
end)

return CController
