local CECS = require("dr2c.client.ecs.ECS")

-- print(scriptID, scriptName)
-- events:add(N_"CEntitySchemaLoadComponents", function(e)
-- end, "test", "testa")

local filterTest = CECS.filter({
	"GameObject",
})

events:add(N_("CUpdate"), function(e)
	for _ = 1, 9999 do
		if CECS.countEntities(filterTest) >= 1000 then
			break
		end

		CECS.spawnEntity("Zombie")
		-- print("count", CECS.countEntities(filterTest))
	end

	-- local getComponentByType = CECS.getComponentByType
	-- local random = math.random
	-- for state, id, typeID in CECS.iterateEntities(filterTest) do
	-- 	local gameObject = getComponentByType(id, "GameObject")
	-- 	gameObject.x = random(1, 100)
	-- 	gameObject.y = random(1, 100)
	-- end
end, "TestSpan1000ZombieEntities", "Control")
