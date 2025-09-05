--- @class dr2c.TestSynchrony
local TestSynchrony = {}

function TestSynchrony.enable()
	local Function = require("tudov.Function")
	local Math = require("tudov.Math")

	local CECS = require("dr2c.client.ecs.ECS")
	local CNetworkClient = require("dr2c.client.network.Client")
	local CNetworkClock = require("dr2c.client.network.Clock")
	local CWorldSnapshot = require("dr2c.client.world.Snapshot")

	local SSnapshot = require("dr2c.server.world.Snapshot")

	TestSynchrony.enable = Function.empty

	local filter = CECS.filter({
		"GameObject",
	})

	events:add(N_("CRender"), function(e)
		local renderer = e.renderer

		local y = 50

		local function drawLine(text)
			renderer:drawDebugText(0, y, text)
			y = y + 10
		end

		local clientSnapshots = CWorldSnapshot.getAll()

		do
			local totalID = #clientSnapshots
			local firstID = clientSnapshots.first

			drawLine(
				("Client snapshots: %s, first=%s, last=%s"):format(
					totalID,
					firstID,
					firstID and (firstID + totalID - 1) or "nil"
				)
			)
		end

		drawLine("Clock " .. CNetworkClock.getTime())

		for index, id, typeID in CECS.iterateEntities(filter) do
			local gameObject = CECS.getComponent(id, "GameObject")

			drawLine(("entity %s position: x=%s, y=%s"):format(id, gameObject.x, gameObject.y))
		end
	end, "TestSynchronyInfo", "Debug")

	events:add(N_("CWorldTickProcess"), function(e)
		if CECS.countEntitiesByType("Character") < 2 then
			print("Spawn a Character")
			CECS.spawnEntity("Character")
		end
	end, "testSpawnCharacters", "Test")
end

TestSynchrony.enable()

return TestSynchrony
