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

--- @type boolean
local isFastForwarding = false

CWorldRollback.eventClientWorldRollback = TE.events:new(N_("CWorldRollback"), {
	"Snapshot",
	"Tick",
	"Reset",
})

--- 检测当前是否应用了回滚
--- @return boolean
--- @nodiscard
function CWorldRollback.isFastForwarding()
	return isFastForwarding
end

--- 应用回滚，将世界状态设置到过去的某一刻，并快进回当前刻
--- @param worldTick dr2c.WorldTick
--- @param extras? table
--- @return boolean
function CWorldRollback.perform(worldTick, extras)
	local currentTick = CWorldTick.getCurrentTick()
	if currentTick <= 1 or worldTick > currentTick then -- 不允许滚到未来世界刻
		return false
	end

	--- @class dr2c.E.CWorldRollback
	--- @field tick dr2c.WorldTick
	--- @field extras? table
	--- @field suppressed? boolean
	local e = {
		tick = worldTick,
		extras = extras,
	}
	TE.events:invoke(CWorldRollback.eventClientWorldRollback, e)

	isFastForwarding = not e.suppressed
	if isFastForwarding then
		extras = e.extras
		if type(e.extras) == "table" then
			extras.fastForward = true
		else
			extras = { fastForwarding = true }
		end

		while not CWorldTick.process(currentTick, extras) do
		end

		isFastForwarding = false
	end

	return true
end

--- @param e dr2c.E.CPlayerInputsPredictFailed
TE.events:add(N_("CPlayerInputsPredictFailed"), function(e)
	if log.canDebug() then
		log.debug(("Perform rollback to tick %s"):format(e.worldTick))
	end

	CWorldRollback.perform(e.worldTick)
end, "PerformRollback", "Rollback")

return CWorldRollback
