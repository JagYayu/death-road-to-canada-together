--[[
-- @module dr2c.client.world.Physics
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("tudov.Enum")

local math_max = math.max
local math_min = math.min
local math_sqrt = math.sqrt

--- @class dr2c.CPhysics
local CPhysics = {}

CPhysics.Fields = Enum.immutable({
	X = 1,
	Y = 2,
})

local FX = CPhysics.Fields.X
local FY = CPhysics.Fields.Y

local physicalBodyLatest = 0
local physicalBodies = {}

local function vec2(x, y)
	return { [FX] = x, [FY] = y }
end

local function dot(a, b)
	return a[FX] * b[FX] + a[FY] * b[FY]
end

local function length2(v)
	return v[FX] * v[FX] + v[FY] * v[FY]
end

local function sub(a, b)
	return vec2(a[FX] - b[FX], a[FY] - b[FY])
end

local function add(a, b)
	return vec2(a[FX] + b[FX], a[FY] + b[FY])
end

local function mul(v, s)
	return vec2(v[FX] * s, v[FY] * s)
end

function CPhysics.addBody(args)
	local body = {
		shape = args.shape,
		mass = args.mass or 1,
		invMass = (args.mass == 0) and 0 or (1 / args.mass),
		pos = args.pos or vec2(0, 0),
		vel = args.vel or vec2(0, 0),
	}

	local id = physicalBodyLatest + 1
	physicalBodyLatest = id
	physicalBodies[id] = body

	return id
end

local function detectCircleVsCircle(a, b)
	local delta = sub(b.pos, a.pos)
	local r = a.shape.r + b.shape.r
	if length2(delta) <= r * r then
		return true, delta
	end
	return false
end

local function detectRectVsRect(a, b)
	local dx = math.abs(a.pos[FX] - b.pos[FX])
	local dy = math.abs(a.pos[FY] - b.pos[FY])
	local hw = (a.shape.w / 2 + b.shape.w / 2)
	local hh = (a.shape.h / 2 + b.shape.h / 2)
	if dx <= hw and dy <= hh then
		return true, vec2(dx - hw, dy - hh)
	end

	return false
end

local function detectCircleVsRect(circle, rect)
	local cx, cy = circle.pos[FX], circle.pos[FY]
	local rx, ry = rect.pos[FX], rect.pos[FY]
	local hw, hh = rect.shape.w / 2, rect.shape.h / 2

	local nearestX = math_max(rx - hw, math_min(cx, rx + hw))
	local nearestY = math_max(ry - hh, math_min(cy, ry + hh))

	local dx, dy = cx - nearestX, cy - nearestY
	if dx * dx + dy * dy <= circle.shape.r * circle.shape.r then
		return true, vec2(dx, dy)
	end

	return false
end

local function checkCollision(a, b)
	local shapeTypeA = a.shape.type
	local shapeTypeB = b.shape.type

	if shapeTypeA == "circle" then
		if shapeTypeB == "circle" then
			return detectCircleVsCircle(a, b)
		elseif shapeTypeB == "rect" then
			return detectCircleVsRect(a, b)
		end
	elseif shapeTypeA == "rect" then
		if shapeTypeB == "circle" then
			return detectCircleVsRect(b, a)
		elseif shapeTypeB == "rect" then
			return detectRectVsRect(a, b)
		end
	end

	return false
end

local function resolveCollision(a, b, normal)
	local relativeVelocity = sub(b.vel, a.vel)
	local velocityAlongNormal = dot(relativeVelocity, normal)
	if velocityAlongNormal > 0 then
		return
	end

	local e = 1.0
	local j = -(1 + e) * velocityAlongNormal
	j = j / (a.invMass + b.invMass)

	local impulse = mul(normal, j)
	a.vel = sub(a.vel, mul(impulse, a.invMass))
	b.vel = add(b.vel, mul(impulse, b.invMass))
end

function CPhysics.update(deltaTime)
	for _, body in ipairs(physicalBodies) do
		body.pos[FX] = body.pos[FX] + body.vel[FX] * deltaTime
		body.pos[FY] = body.pos[FY] + body.vel[FY] * deltaTime
	end

	local len = #physicalBodies
	for i = 1, len do
		local a = physicalBodies[i]

		for j = i + 1, len do
			local b = physicalBodies[j]
			local hit, normal = checkCollision(a, b)

			if hit then
				if normal then
					local l = math_sqrt(normal[FX] * normal[FX] + normal[FY] * normal[FY])
					if l > 0 then
						normal = vec2(normal[FX] / l, normal[FY] / l)
						resolveCollision(a, b, normal)
					end
				end
			end
		end
	end
end

return CPhysics
