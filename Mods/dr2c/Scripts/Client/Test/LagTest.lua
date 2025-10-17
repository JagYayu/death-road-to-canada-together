local times = 0 -- 5e6

local function test()
	local t = {}
	for i = 1, times do
		t[#t + 1] = i
	end
end

-- local imageID = TE.images:getID("gfx/cars/cars_unique_110x96.png")

-- TE.events:add(N_("CRenderCamera"), function(e)
-- 	local drawRectArgs = DrawRectArgs()

-- 	drawRectArgs.texture = imageID

-- 	for x = -1, 2 do
-- 		drawRectArgs.destination = {
-- 			x = x * 100,
-- 			y = x * 100,
-- 			w = 80,
-- 			h = 57,
-- 		}
-- 		drawRectArgs.source = {
-- 			x = 14,
-- 			y = 302,
-- 			w = 80,
-- 			h = 57,
-- 		}
-- 		e.renderer:drawRect(drawRectArgs)
-- 	end
-- end)

return {
	test = test,
	setTimes = function(value)
		times = value
	end,
}
