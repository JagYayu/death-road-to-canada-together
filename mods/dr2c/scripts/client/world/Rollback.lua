--[[
-- @module dr2c.client.world.Rollback
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CSnapshot = require("dr2c.client.world.Snapshot")
local GMessage = require("dr2c.shared.network.Message")

--- @class dr2c.CRollback
local CRollback = {}

local eventClientWorldRollback = events:new(N_("CWorldRollback"), {
	"Snapshot",
	"Tick",
	"Reset",
})

--- @param worldTick dr2c.WorldTick
function CRollback.apply(worldTick)
	--- @class dr2c.E.CWorldRollback
	--- @field tick dr2c.WorldTick
	--- @field suppressed true?
	local e = {
		tick = worldTick,
		suppressed = nil,
	}
	events:invoke(eventClientWorldRollback, e)
end

--- @param e dr2c.E.ClientMessage
events:add(N_("CMessage"), function(e)
	CRollback.apply(e.content.worldTick)
end, "ApplyWorldRollback", "Rollback", GMessage.Type.PlayerInputs)

-- events:add(N_(""), func, name?, order?, key?, sequence?)

return CRollback
