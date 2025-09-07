local w = 24
local h = 24

local femaleHead = {}

for frameY = 1, 8 do
	for frameX = 1, 8 do
		femaleHead[#femaleHead + 1] = {
			image = "gfx/tiles/street.png",
			x = (frameX - 1) * w,
			y = (frameY - 1) * h,
			w = w,
			h = h,
		}
	end
end

return {
	FemaleHead = femaleHead,
}
