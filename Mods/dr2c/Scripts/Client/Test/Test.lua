local CEntityECS = require("dr2c.Client.Entity.ECS")

-- print(scriptID, scriptName)
-- TE.events:add(N_"CEntitySchemaLoadComponents", function(e)
-- end, "test", "testa")

local filterTest = CEntityECS.filter({
	"GameObject",
})

TE.events:add(N_("CUpdate"), function(e)
	-- for _ = 1, 100 do
	-- 	local zombieCount = CECS.countEntitiesByType("Zombie")
	-- 	if zombieCount >= 1000 then
	-- 		break
	-- 	end

	-- 	-- print("Spawn a Zombie")
	-- 	CECS.spawnEntity("Zombie")
	-- end

	-- CECS.update()

	-- local getComponent = CECS.getComponent
	-- local random = math.random
	-- for state, id, typeID in CECS.iterateEntities(filterTest) do
	-- 	local gameObject = getComponent(id, "GameObject")
	-- 	gameObject.x = gameObject.x + random(-5, 5)
	-- 	gameObject.y = gameObject.y + random(-5, 5)
	-- end
end, "TestSpawnEntities", "Control")
