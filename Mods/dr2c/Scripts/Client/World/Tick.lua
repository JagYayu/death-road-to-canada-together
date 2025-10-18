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

local CSnapshot = require("dr2c.Client.World.Snapshot")
local CClock = require("dr2c.Client.Network.Clock")
local CPlayerInputBuffers = require("dr2c.Client.World.PlayerInputBuffers")
local CWorldSession = require("dr2c.Client.World.Session")
local GWorldSession = require("dr2c.Shared.World.Session")

local CSnapshot_get = CSnapshot.getSnapshot
local CClock_getTime = CClock.getTime
local math_ceil = math.ceil
local math_floor = math.floor
local type = type

--- @class dr2c.WorldTick : integer

--- @class dr2c.CWorldTick
local CWorldTick = {}

CWorldTick.ticksPerSeconds = 2 ^ 3 -- 2 ^ 5
local deltaTime = 1 / CWorldTick.ticksPerSeconds

local currentTick = 0

--- @type integer?
local processingTick
--- @type integer
local processingTargetTick
--- @type integer
local processedTicks = 0

currentTick = persist("currentTick", function()
	return currentTick
end)

--- 获取刻增量时间
--- @return number
function CWorldTick.getDeltaTime()
	return deltaTime
end

--- 获取世界理论最新刻
--- @return integer tick
--- @nodiscard
function CWorldTick.getLatestTick()
	return math_floor(CClock_getTime() * CWorldTick.ticksPerSeconds)
end

local CWorldTick_getLatestTick = CWorldTick.getLatestTick

--- 获取世界当前刻
--- @param tick integer
--- @nodiscard
function CWorldTick.getCurrentTick(tick)
	return currentTick
end

--- 修改世界当前刻
--- @param tick integer
function CWorldTick.setCurrentTick(tick)
	currentTick = tonumber(tick) or 0
end

--- 获取正在执行的世界刻
--- @return integer? @若在事件`dr2c_CWorldTickProcess`之外调用则总是返回`nil`
--- @nodiscard
function CWorldTick.getProcessingTargetTick()
	return processingTargetTick
end

--- 获取正在执行的或上一次执行的世界目标刻
--- @return integer? @若在事件`dr2c_CWorldTickProcess`之外调用则总是返回`nil`
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
	"PlayerInputs",
	"SnapshotSave",
	"AccelThreadBegin",
	"Move",
	"Test",
	"AccelThreadEnd",
	"ECS",
})

--- @param targetTick integer
--- @return boolean
local function process(targetTick)
	processingTargetTick = targetTick
	processedTicks = 0

	--- @class dr2c.E.CWorldTickProcess
	--- @field tick dr2c.WorldTick
	--- @field targetTick dr2c.WorldTick
	--- @field processedTicks integer
	--- @field suppressed boolean
	local e

	while currentTick < targetTick do
		processedTicks = processedTicks + 1

		currentTick = currentTick + 1
		processingTick = currentTick

		if e then
			e.tick = processingTick
			e.processedTicks = processedTicks
		else
			e = {
				tick = processingTick,
				targetTick = targetTick,
				processedTicks = processedTicks,
				suppressed = false,
			}
		end

		TE.events:invoke(CWorldTick.eventCWorldTickProcess, e)

		if e.suppressed then
			break
		end
	end

	processingTick = nil

	return currentTick >= targetTick
end

--- 执行世界刻，直到当前世界刻大于或等于目标世界刻
--- @param targetTick integer?
--- @return boolean completed
function CWorldTick.process(targetTick)
	if targetTick == nil then
		targetTick = CWorldTick_getLatestTick()
	elseif type(targetTick) ~= "number" then
		error("Type mismatch, number or nil expected", 2)
	elseif targetTick ~= math.floor(targetTick) then
		error("Target tick must be an integer number", 2)
	end

	return process(targetTick)
end

TE.events:add(N_("CUpdate"), function(e)
	if CWorldSession.isPlaying() then
		local completed = process(CWorldTick_getLatestTick())

		if processedTicks > 0 and log.canTrace() then
			log.trace(("Processed %d ticks, %s"):format(processedTicks, completed and "complete" or "incomplete"))
		end
	end
end, "ProcessWorldTicks", "World")

TE.events:add(N_("CWorldRollback"), function(e)
	if not e.suppressed then
		currentTick = math.min(currentTick, e.tick - 1)
	end
end, "SetCurrentTick", "Tick")

return CWorldTick
