--- [Sprites Definition]

local Walls = {}

local image = "gfx/tiles/walls.png"
local w = 16
local h = 16

local wallCeiling = {}

for frameX = 1, 16 do
	wallCeiling[frameX] = {
		image = image,
		x = (frameX - 1) * w,
		y = 0,
		w = w,
		h = h,
	}
end

Walls.WallCeiling = wallCeiling

local wallIndex = 0

--- @param frameX integer
--- @param frameY integer
local function addWall(frameX, frameY)
	--- @type any[]
	local wall = { false, false, false, false, false, false, false, false }

	for index = 1, 8 do
		wall[index] = {
			image = image,
			x = (frameX + index - 2) * w,
			y = (frameY - 1) * h,
			w = w,
			h = h,
		}
	end

	wallIndex = wallIndex + 1
	Walls["Wall" .. wallIndex] = wall
end

addWall(1, 2)

for x = 1, 2 do
	for y = 4, 16 do
		addWall((x - 1) * 8 + 1, y)
	end
end

return Walls
