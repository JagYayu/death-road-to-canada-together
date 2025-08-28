local CSnapshot = require("dr2c.client.world.Snapshot")
local CWorldTime = require("dr2c.client.world.WorldTime")
local CPlayerInputBuffers = require("dr2c.client.world.PlayerInputBuffers")

local CSnapshot_get = CSnapshot.get

--- @class dr2c.CWorldTick
local CWorldTick = {}

CWorldTick.tps = 32

--- @return integer tick
function CWorldTick.getCurrentTick()
	return math.ceil(CWorldTime.getTime() * CWorldTick.tps)
end

-- local snapshotCurrentTick = CSnapshot.register("currentTick", 0)
local processingTick = 0

function CWorldTick.getProcessingTick()
	return processingTick
end

local eventProcessWorldTick = events:new(N_("CWorldTickProcess"), {
	"SaveSnapshot",
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

	-- print(CPlayerInputBuffers.collectInputsInRange(1))
end

--- @param e dr2c.CUpdate
events:add(N_("CUpdate"), function(e)
	CWorldTick.process()
end, "ProcessWorldTicks", "World")

return CWorldTick
