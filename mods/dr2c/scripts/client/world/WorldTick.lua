local Table = require("tudov.Table")

local CSnapshot = require("dr2c.client.world.Snapshot")
local CWorldTime = require("dr2c.client.world.WorldTime")
local CPlayerInputBuffers = require("dr2c.client.world.PlayerInputBuffers")

local CSnapshot_get = CSnapshot.getSnapshot
local CWorldTime_getTime = CWorldTime.getTime
local math_ceil = math.ceil
local math_floor = math.floor

--- @class dr2c.WorldTick : integer

--- @class dr2c.CWorldTick
local CWorldTick = {}

CWorldTick.tps = 8
local deltaTime = 1 / CWorldTick.tps

-- local snapshotCurrentTick = CSnapshot.register("currentTick", 0)
local latestTick = 0
local processingTick = 0

function CWorldTick.getDeltaTime()
	return deltaTime
end

--- @return integer tick
function CWorldTick.getCurrentTick()
	return math_floor(CWorldTime_getTime() * CWorldTick.tps)
end

function CWorldTick.getNextTick()
	return math_ceil(CWorldTime_getTime() * CWorldTick.tps)
end

function CWorldTick.setLatestTick(tick)
	latestTick = tonumber(tick) or tick
end

function CWorldTick.getProcessingTick()
	return processingTick
end

local eventClientWorldTickProcess = events:new(N_("CWorldTickProcess"), {
	"PlayerInputs",
	"SnapshotSave",
	"AccelThreadBegin",
	"Move",
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
		error("target tick must be an integer number", 2)
	end

	local playersInputs = CPlayerInputBuffers.collectPlayersInputsInRange(latestTick, targetTick)
	local playerIDs = Table.getKeyList(playersInputs)

	while latestTick < targetTick do
		latestTick = latestTick + 1
		processingTick = latestTick

		local playerInputs = Table.new(0, #playerIDs)
		for _, playerID in ipairs(playerIDs) do
			playerInputs[playerID] = playersInputs[playerID][processingTick]
		end

		local e = {
			tick = processingTick,
			playerInputs = playerInputs,
		}
		-- print(e)
		events:invoke(eventClientWorldTickProcess, e)
	end
end

events:add(N_("CUpdate"), function(e)
	CWorldTick.process()
end, "ProcessWorldTicks", "World")

events:add(N_("CWorldRollback"), function(e)
	latestTick = math.min(latestTick, e.snapshotID)
end, "SetLatestTick", "Tick")

return CWorldTick
