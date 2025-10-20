--[[
-- @module dr2c.Client.World.Rollback
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CWorldTick = require("dr2c.Client.World.Tick")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")

--- @class dr2c.CWorldRollback
local CWorldRollback = {}

--- @type dr2c.WorldTick?
local rollbackPending
local rollbackIsActivated = false

rollbackPending = persist("rollbackPending", function()
	return rollbackPending
end)
rollbackIsActivated = persist("rollbackIsApplied", function()
	return rollbackIsActivated
end)

CWorldRollback.eventClientWorldRollback = TE.events:new(N_("CWorldRollback"), {
	"Snapshot",
	"Tick",
	"Reset",
})

--- 检测当前是否应用了回滚
--- @return boolean
--- @nodiscard
function CWorldRollback.isActivated()
	return rollbackIsActivated
end

--- 应用回滚，将世界状态设置到过去的某一刻
--- @param worldTick dr2c.WorldTick
--- @param extras? any
--- @return boolean
function CWorldRollback.apply(worldTick, extras)
	if worldTick > CWorldTick.getCurrentTick() then -- 不允许滚到未来世界帧
		return false
	end

	--- @class dr2c.E.CWorldRollback
	--- @field tick dr2c.WorldTick
	--- @field extras? any
	--- @field suppressed? boolean
	local e = {
		tick = worldTick,
		extras = extras,
	}
	TE.events:invoke(CWorldRollback.eventClientWorldRollback, e)

	rollbackIsActivated = not e.suppressed

	return true
end

--- @param worldTick dr2c.WorldTick
function CWorldRollback.setPending(worldTick)
	if rollbackPending then
		rollbackPending = math.min(rollbackPending, worldTick)
	else
		rollbackPending = worldTick
	end
end

TE.events:add(N_("CUpdate"), function(e)
	if rollbackPending then
		CWorldRollback.apply(rollbackPending)

		rollbackPending = nil
	end
end, "ApplyRollback", "Rollback")

--- @param e dr2c.E.CWorldTickProcess
TE.events:add(N_("CWorldTickProcess"), function(e)
	if rollbackIsActivated and CWorldTick.isOnLatestTick() then
		rollbackIsActivated = false
	end
end, "DeactivateRollback", "Rollback")

--- @param e dr2c.E.CPlayerInputsPredictFailed
TE.events:add(N_("CPlayerInputsPredictFailed"), function(e)
	if log.canDebug() then
		log.debug(("Player inputs predict failed, set rollback pending to tick %s"):format(e.worldTick))
	end

	CWorldRollback.setPending(e.worldTick)
end, "RollbackPending", "Rollback", GNetworkMessage.Type.PlayerInputs)

return CWorldRollback
