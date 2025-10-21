--[[
-- @module dr2c.Client.World.Tick
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("TE.Table")

local CNetworkClock = require("dr2c.Client.Network.Clock")
local CWorldSession = require("dr2c.Client.World.Session")
local GWorldSession = require("dr2c.Shared.World.Session")
local GWorldTick = require("dr2c.Shared.World.Tick")

local CWorldSession_getAttribute = CWorldSession.getAttribute
local GWorldSession_Attribute_TimeStart = GWorldSession.Attribute.TimeStart
local GWorldTick_getTPS = GWorldTick.getTPS
local CNetworkClock_getTime = CNetworkClock.getTime
local math_floor = math.floor
local math_max = math.max
local type = type

--- @class dr2c.WorldTick : integer

--- @class dr2c.CWorldTick
local CWorldTick = {}

--- @type dr2c.WorldTick
local currentTick = 0

--- @type integer?
local processingTick
--- @type integer?
local processingTargetTick
--- @type integer
local processedTicks = 0

currentTick = persist("currentTick", function()
	return currentTick
end)

--- @deprecated Use `GWorldTick.getTPS` instead
CWorldTick.getTPS = GWorldTick.getTPS
--- @deprecated Use `GWorldTick.getDeltaTime` instead
CWorldTick.getDeltaTime = GWorldTick.getDeltaTime

--- 获取世界理论最新刻
--- @return dr2c.WorldTick tick
--- @nodiscard
function CWorldTick.getLatestTick()
	local time = CNetworkClock_getTime() - CWorldSession_getAttribute(GWorldSession_Attribute_TimeStart)
	return math_max(0, math_floor(time * GWorldTick_getTPS()))
end

local CWorldTick_getLatestTick = CWorldTick.getLatestTick

--- 获取世界当前刻
--- @return dr2c.WorldTick
--- @nodiscard
function CWorldTick.getCurrentTick()
	return currentTick
end

--- 修改世界当前刻
--- @param tick dr2c.WorldTick
function CWorldTick.setCurrentTick(tick)
	currentTick = tonumber(tick) or 0
end

--- 获取正在执行的世界刻
--- @return dr2c.WorldTick? @若在事件`dr2c_CWorldTickProcess`之外调用则总是返回`nil`
--- @nodiscard
function CWorldTick.getProcessingTargetTick()
	return processingTargetTick
end

--- 获取正在执行的或上一次执行的世界目标刻
--- @return dr2c.WorldTick? @若在事件`dr2c_CWorldTickProcess`之外调用则总是返回`nil`
--- @nodiscard
function CWorldTick.getProcessingTick()
	return processingTick
end

--- 获取正在执行的或上一次执行的世界刻次数
--- @return integer
--- @nodiscard
function CWorldTick.getProcessedTicks()
	return processedTicks
end

--- 判断当前世界刻是否是理论最新世界刻
--- @return boolean
--- @nodiscard
function CWorldTick.isOnLatestTick()
	return currentTick >= CWorldTick_getLatestTick()
end

CWorldTick.eventCWorldTickProcess = TE.events:new(N_("CWorldTickProcess"), {
	"Rollback", -- end rollback state
	"Snapshot", -- Save tick initial snapshot
	"PlayerInputs", -- Collect players' inputs
	"BeginAccel", -- begin physics/... accelerations from cpp
	"Move",
	"Test",
	"EndAccel", -- end physics/... accelerations from cpp
	"ECS", -- post update ECS
})

--- @param targetTick integer
--- @param extras table?
local function process(targetTick, extras)
	processingTargetTick = targetTick
	processedTicks = 0

	if targetTick - currentTick > 1e5 and log.canWarn() then
		log.warn(("Too many ticks pending to execute: %d"):format(targetTick - currentTick))
	end

	--- @class dr2c.E.CWorldTickProcess
	--- @field tick dr2c.WorldTick
	--- @field targetTick dr2c.WorldTick
	--- @field processedTicks integer
	--- @field abort? boolean
	local e

	while currentTick < targetTick do
		currentTick = currentTick + 1
		processingTick = currentTick
		processedTicks = processedTicks + 1

		if e then
			e.tick = processingTick
			e.processedTicks = processedTicks
		else
			e = {
				tick = processingTick,
				targetTick = targetTick,
				processedTicks = processedTicks,
			}
			--TODO e = extras and Table.merge(e, extras) or e
		end

		TE.events:invoke(CWorldTick.eventCWorldTickProcess, e)

		if e.abort then
			break
		end
	end

	processingTargetTick = nil
	processingTick = nil
end

--- 执行世界刻，直到当前世界刻大于或等于目标世界刻，或被强制终止
--- @param targetTick? integer
--- @param extras? table @额外参数，会覆盖事件参数`e`的字段
--- @return boolean completed
function CWorldTick.process(targetTick, extras)
	if targetTick == nil then
		targetTick = CWorldTick_getLatestTick()
	elseif type(targetTick) ~= "number" then
		error("Type mismatch, number or nil expected", 2)
	elseif targetTick ~= math.floor(targetTick) then
		error("Target tick must be an integer number", 2)
	end

	process(targetTick, extras)
	return currentTick >= targetTick
end

function CWorldTick.reset()
	currentTick = 0
end

TE.events:add(N_("CConnect"), CWorldTick.reset, N_("ResetWorldTick"), "Reset")
TE.events:add(N_("CDisconnect"), CWorldTick.reset, N_("ResetWorldTick"), "Reset")
TE.events:add(N_("CWorldSessionStart"), CWorldTick.reset, N_("ResetWorldTick"), "Reset")
TE.events:add(N_("CWorldSessionFinish"), CWorldTick.reset, N_("ResetWorldTick"), "Reset")

--- @param e dr2c.E.CUpdate
TE.events:add(N_("CUpdate"), function(e)
	if CWorldSession.isPlaying() then
		local latestTick = CWorldTick_getLatestTick()
		process(latestTick)

		if processedTicks > 0 and log.canTrace() then
			log.trace(("Processed %d ticks, %s, current tick: %d"):format( --
				processedTicks,
				currentTick >= latestTick and "complete" or "incomplete",
				currentTick
			))
		end
	end
end, "ProcessWorldTick", "World")

--- @param e dr2c.E.CWorldRollback
TE.events:add(N_("CWorldRollback"), function(e)
	if not e.suppressed then
		currentTick = math.min(currentTick, e.tick - 1)
	end
end, "SetCurrentTick", "Tick")

return CWorldTick
