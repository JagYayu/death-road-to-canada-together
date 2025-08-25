local function test()
	local times = 0 --, 5e6, 1.5e7
	local t = {}
	for i = 1, times do
		t[#t + 1] = i
	end
end

test()

return {
	test = test,
}
