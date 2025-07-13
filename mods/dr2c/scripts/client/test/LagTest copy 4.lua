local times = require "dr2c.client.test.LagTest".times
local t = {}
for i = 1, times do
	t[#t + 1] = t
end
