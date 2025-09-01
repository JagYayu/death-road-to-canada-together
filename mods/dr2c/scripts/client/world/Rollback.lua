local CSnapshot = require("dr2c.client.world.Snapshot")
local GMessage = require("dr2c.shared.network.Message")

--- @class dr2c.CRollback
local CRollback = {}

local eventClientWorldRollback = events:new(N_("CWorldRollback"), {
	"Snapshot",
	"Tick",
})

--- @param worldTick dr2c.WorldTick
function CRollback.apply(worldTick)
	local e = {
		tick = worldTick,
	}
	events:invoke(eventClientWorldRollback, e)
end

--- @param e dr2c.E.ClientMessage
events:add(N_("CMessage"), function(e)
	print("rollback to " .. e.content.worldTick)
	CRollback.apply(e.content.worldTick)
end, "ApplyWorldRollback", "Rollback", GMessage.Type.PlayerInput)

return CRollback
