--- @class dr2c.TestSynchrony
local TestSynchrony = {}

function TestSynchrony.enable()
	local Function = require("tudov.Function")

	local CClient = require("dr2c.client.network.Client")
	local CSnapshot = require("dr2c.client.world.Snapshot")

	TestSynchrony.enable = Function.empty

	events:add(N_("CRender"), function(e)
		local renderer = e.renderer

		local y = 50

		local function drawLine(text)
			renderer:drawDebugText(0, y, text)
			y = y + 10
		end

		local clientSnapshots = CSnapshot.getAll()

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
		drawLine("test test")
		drawLine("test test")
	end, "TestSynchronyInfo", "Debug")
end

TestSynchrony.enable()

return TestSynchrony
