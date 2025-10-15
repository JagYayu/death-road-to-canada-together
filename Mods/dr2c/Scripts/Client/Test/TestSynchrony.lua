--- @class dr2c.TestSynchrony
local TestSynchrony = {}

local drawTextArgs = DrawTextArgs()

drawTextArgs.font = TE.fonts:getID("Mods/dr2c/Fonts/Galmuri7.ttf")

function TestSynchrony.enable()
	local Function = require("TE.Function")
	local Math = require("TE.Math")

	local CEntityECS = require("dr2c.Client.Entity.ECS")
	local CNetworkClient = require("dr2c.Client.Network.Client")
	local CNetworkClock = require("dr2c.Client.Network.Clock")
	local CWorldSnapshot = require("dr2c.Client.World.Snapshot")

	local SSnapshot = require("dr2c.Server.World.Snapshot")

	TestSynchrony.enable = Function.empty

	local filter = CEntityECS.filter({
		"GameObject",
	})

	local i = 0

	--- @param e dr2c.E.CRender
	TE.events:add(N_("CRenderCamera"), function(e)
		local renderer = e.renderer

		i = i + 1
		drawTextArgs.x = -100
		drawTextArgs.y = 60

		local function drawLine(text)
			drawTextArgs.text = text

			local x, y, w, h = renderer:drawText(drawTextArgs)

			drawTextArgs.y = drawTextArgs.y + h + 2
		end

		-- drawLine("NEWS OPTIONS")
		-- drawLine("你好世界")
		-- drawLine("Death Road to Canada")

		-- local clientSnapshots = CWorldSnapshot.getAll()

		-- do
		-- 	local totalID = #clientSnapshots
		-- 	local firstID = clientSnapshots.first

		-- 	drawLine(
		-- 		("Client snapshots: %s, first=%s, last=%s"):format(
		-- 			totalID,
		-- 			firstID,
		-- 			firstID and (firstID + totalID - 1) or "nil"
		-- 		)
		-- 	)
		-- end

		-- drawLine("Clock " .. CNetworkClock.getTime())

		-- for index, id, typeID in CECS.iterateEntities(filter) do
		-- 	local gameObject = CECS.getComponent(id, "GameObject")

		-- 	drawLine(("entity %s position: x=%s, y=%s"):format(id, gameObject.x, gameObject.y))
		-- end
	end, "TestSynchronyInfo", "Debug")

	TE.events:add(N_("CWorldTickProcess"), function(e)
		if CEntityECS.countEntitiesByType("Character") < 2 then
			print("Spawn a Character")
			CEntityECS.spawnEntity("Character")
		end
	end, "testSpawnCharacters", "Test")
end

TestSynchrony.enable()

return TestSynchrony
