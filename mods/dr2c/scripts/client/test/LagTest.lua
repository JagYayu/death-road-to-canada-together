local times = 0 -- 10000000
local t = {}
for i = 1, times do
	t[#t + 1] = t
end
return { times = times }
