local Render = {}

local eventRenderGame = events:new(N_("RenderGame"), {
	"Camera",
	"Sprites",
	"Rooms",
	"UI",
})

print(N_("RenderGame"))

local len = 10000

-- local eventTest1 = events:new(N_("Test1"), {})
-- local eventTest2 = events:new(N_("Test2"), {})

-- events:add(N_("Test1"), function(e)
-- 	for i = 1, len do
-- 		i = i * math.max(math.random(), math.random())
-- 	end
-- end, N_("test1"))

-- events:add(N_("Test2"), function()
-- 	local max = math.max
-- 	local random = math.random
-- 	for i = 1, len do
-- 		i = i * max(random(), random())
-- 	end
-- end, N_("test2"))

events:add("TickRender", function(e)
	-- events:invoke(eventTest1, {})
	-- events:invoke(eventTest2, {})
	events:invoke(eventRenderGame, e)
end, N_("renderGame"), nil, "Main")

return Render
