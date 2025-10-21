--- @class dr2c.TestSynchrony
local TestSynchrony = {}

local Function = require("TE.Function")
local Math = require("TE.Math")

local CEntityComponents = require("dr2c.Client.Entity.Components")
local CEntityECS = require("dr2c.Client.Entity.ECS")
local CEntityEntities = require("dr2c.Client.Entity.Entities")
local CNetworkClient = require("dr2c.Client.Network.Client")
local CNetworkClock = require("dr2c.Client.Network.Clock")
local CWorldSnapshot = require("dr2c.Client.World.Snapshot")

CEntityComponents.registerEntitySerializable("RandomMove", {}, {
	"GameObject",
})

CEntityEntities.registerEntity("Dummy", {
	DebugVisual = {},
	DebugVisualRectangle = {},
	GameObject = {},
	RandomMove = {},
})

local dummyFilter = CEntityECS.filter({
	"RandomMove",
})

--- @param e dr2c.E.CWorldTickProcess
TE.events:add(N_("CWorldTickProcess"), function(e)
	CNetworkClient.simulateLatency(0.2) -- 设置200ms延迟

	local characterCount = CEntityECS.countEntitiesByType("Character")
	for _ = characterCount + 1, 3 do -- 创建2个玩家
		CEntityECS.spawnEntity("Character")
	end

	-- local zombieCount = CEntityECS.countEntitiesByType("Dummy")
	-- for _ = zombieCount + 1, 2 do -- 创建1000个实体，随机移动
	-- 	CEntityECS.spawnEntity("Dummy")
	-- end

	-- local getComponent = CEntityECS.getComponent
	-- local random = PerlinNoiseRandom(e.tick)
	-- for _, id in CEntityECS.iterateEntities(dummyFilter) do
	-- 	local gameObject = getComponent(id, "GameObject") --- @cast gameObject dr2c.Component.GameObject

	-- 	gameObject.x = gameObject.x + random:noise2(id, gameObject.x) * 20
	-- 	gameObject.y = gameObject.y + random:noise2(gameObject.y, id) * 20
	-- end
end, "testSpawnCharacters", "Test")

return TestSynchrony
