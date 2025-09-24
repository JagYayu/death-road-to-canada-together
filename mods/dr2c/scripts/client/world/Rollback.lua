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

local rollbackPending

local eventClientWorldRollback = TE.events:new(N_("CWorldRollback"), {
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
	TE.events:invoke(eventClientWorldRollback, e)
end

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	local worldTick = e.content.worldTick
	if not worldTick then
		return
	end

	if rollbackPending then
		rollbackPending = math.min(rollbackPending, worldTick)
	else
		rollbackPending = worldTick
	end
end, "RollbackPending", "Rollback", GMessage.Type.PlayerInputs)

TE.events:add(N_("CUpdate"), function(e)
	if rollbackPending then
		CRollback.apply(rollbackPending)

		rollbackPending = nil
	end
end, "ApplyRollback", "Rollback")

return CRollback
