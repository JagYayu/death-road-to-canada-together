local times = 0 --, 5e6, 1.5e7

local function test()
	local t = {}
	for i = 1, times do
		t[#t + 1] = i
	end
end

return {
	test = test,
	setTimes = function(value)
		times = value
	end,
}
