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
local rollbackIsApplied = false

rollbackPending = persist("rollbackPending", function()
	return rollbackPending
end)
rollbackIsApplied = persist("rollbackIsApplied", function()
	return rollbackIsApplied
end)

CWorldRollback.eventClientWorldRollback = TE.events:new(N_("CWorldRollback"), {
	"Snapshot",
	"Tick",
	"Reset",
})

--- 检测当前是否应用了回滚
--- @return boolean
--- @nodiscard
function CWorldRollback.isApplied()
	return rollbackIsApplied
end

--- 应用回滚，将世界状态设置到过去的某一刻
--- @param worldTick dr2c.WorldTick
--- @return boolean
function CWorldRollback.apply(worldTick)
	if worldTick > CWorldTick.getCurrentTick() then -- 不允许滚到未来世界帧
		return false
	end

	--- @class dr2c.E.CWorldRollback
	--- @field tick dr2c.WorldTick
	--- @field suppressed true?
	local e = {
		tick = worldTick,
		suppressed = nil,
	}
	TE.events:invoke(CWorldRollback.eventClientWorldRollback, e)

	rollbackIsApplied = not e.suppressed

	return true
end

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	local fields = GNetworkMessageFields.PlayerInputs
	local worldTick = e.content[fields.worldTick]

	if worldTick then
		if rollbackPending then
			rollbackPending = math.min(rollbackPending, worldTick)
		else
			rollbackPending = worldTick
		end
	end
end, "RollbackPending", "Rollback", GNetworkMessage.Type.PlayerInputs)

TE.events:add(N_("CUpdate"), function(e)
	if rollbackPending then
		CWorldRollback.apply(rollbackPending)

		rollbackPending = nil
	end
end, "ApplyRollback", "Rollback")

--- @param e dr2c.E.CWorldTickProcess
TE.events:add(N_("CWorldTickProcess"), function(e)
	if rollbackIsApplied and CWorldTick.isOnLatestTick() then
		rollbackIsApplied = false
	end
end, "DeactivateRollback", "Rollback")

return CWorldRollback
