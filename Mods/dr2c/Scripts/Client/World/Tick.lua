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

--- @class dr2c.WorldTick : integer

--- @class dr2c.CWorldTick
local CWorldTick = {}

CWorldTick.ticksPerSeconds = bit.lshift(1, 3)
local deltaTime = 1 / CWorldTick.ticksPerSeconds

local latestTick = 0

local processingTick = 0

latestTick = persist("latestTick", function()
	return latestTick
end)

--- @return number
function CWorldTick.getDeltaTime()
	return deltaTime
end

--- @return integer tick
function CWorldTick.getCurrentTick()
	return math_floor(CClock_getTime() * CWorldTick.ticksPerSeconds)
end

function CWorldTick.setLatestTick(tick)
	latestTick = tonumber(tick) or tick
end

function CWorldTick.getProcessingTick()
	return processingTick
end

CWorldTick.eventClientWorldTickProcess = TE.events:new(N_("CWorldTickProcess"), {
	"PlayerInputs",
	"SnapshotSave",
	"AccelThreadBegin",
	"Move",
	"Test",
	"AccelThreadEnd",
	"ECS",
})

--- @param targetTick integer?
function CWorldTick.process(targetTick)
	if targetTick == nil then
		targetTick = CWorldTick.getCurrentTick()
	elseif type(targetTick) ~= "number" then
		error("Type mismatch, number or nil expected", 2)
	elseif targetTick ~= math.floor(targetTick) then
		error("Target tick must be an integer number", 2)
	end

	local playersInputs = CPlayerInputBuffers.collectPlayersInputsInRange(latestTick, targetTick)
	local playerIDs = Table.getKeyList(playersInputs)

	local processedTicks = 0
	while latestTick < targetTick do
		processedTicks = processedTicks + 1

		latestTick = latestTick + 1
		processingTick = latestTick

		local playerInputs = Table.new(0, #playerIDs)
		for _, playerID in ipairs(playerIDs) do
			playerInputs[playerID] = playersInputs[playerID][processingTick]
		end

		--- @class dr2c.E.CWorldTickProcess
		--- @field entitiesChanged boolean?
		local e = {
			tick = processingTick,
			playerInputs = playerInputs,
		}
		TE.events:invoke(CWorldTick.eventClientWorldTickProcess, e)
	end

	-- print(("processed %s ticks"):format(ticks))
end

TE.events:add(N_("CUpdate"), function(e)
	if CWorldSession.getAttribute(GWorldSession.Attribute.State) == GWorldSession.State.Playing then
		CWorldTick.process()
	end
end, "ProcessWorldTicks", "World")

TE.events:add(N_("CWorldRollback"), function(e)
	if not e.suppressed then
		latestTick = math.min(latestTick, e.tick - 1)
	end
end, "SetLatestTick", "Tick")

return CWorldTick
