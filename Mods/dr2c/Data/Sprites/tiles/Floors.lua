--- [Sprites Definition]

local Floors = {}

local image = "gfx/tiles/floors.png"
local w = 16
local h = 16

for frameY = 1, 22 do
	local floor = {}
	Floors["Floor" .. frameY] = floor

	local y = (frameY - 1) * h
	for x = 0, 3 do
		floor[#floor + 1] = {
			image = image,
			x = x * w,
			y = y,
			w = w,
			h = h,
		}
	end
end

return Floors
