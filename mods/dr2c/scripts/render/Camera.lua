--- @class Camera
local Camera = {}
local Matrix3x3 = require "dr2c.utils.Matrix3x3"

local centerX = 0
local centerY = 0
local rotation = 0
local scaleX = 1
local scaleY = 1
local viewWidth = 1280
local viewHeight = 768

--- @param x number
--- @param y number
function Camera.set(x, y)
	centerX = x
	centerY = y
end

local targetCenterX = 0
local targetCenterY = 0
local targetScaleX = 0
local targetScaleY = 0

local transform = Matrix3x3.new()

function Camera.getTransform()
	return transform
end

local timer = 0

-- Events.add("KeyDown", function(e)
-- 	print(e)
-- end)

events:add("RenderGame", function(e)
	-- -- TODO LERP POSITION
	-- timer = timer + 1

	-- local windowWidth, windowHeight = Window.getSize()
	-- local scale = math.max(windowWidth / viewWidth, windowHeight / viewHeight)
	-- scaleX = scale
	-- scaleY = scale

	-- transform:reset()
	-- transform:scale(scaleX, scaleY)
	-- transform:translate(viewWidth / 2, viewHeight / 2)
	-- transform:translate(-centerX, -centerY)
end, N_ "updateCamera", "Camera")

return Camera;
