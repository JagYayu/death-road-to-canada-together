--[[
-- @module dr2c.Shared.Utils.Algorithms
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Math = require("TE.Math")
local Utility = require("TE.Utility")
--- @class dr2c.GUtilsAlgorithms
local GUtilsAlgorithms = {}

--- @param item table
--- @return number?
--- @return number?
local function directionalNavigationDefaultLocate(item)
	local x = tonumber(item[1])
	local y = tonumber(item[2])
	if x and y then
		return x, y
	end
end

--- @class dr2c.GUtilsAlgorithms.Args
--- @field angle number
--- @field items table[]
--- @field locate? fun(item: table): (x: number | { [1]: number, [2]: number }[]?, y: number?)

--- @param args dr2c.GUtilsAlgorithms.Args
--- @return table?
function GUtilsAlgorithms.directionalNavigation(args)
	local bestItem
	local bestAngle
	local bestSquareDistance

	local angle = Utility.evaluate(args.angle, 1)
	local locate = args.locate or directionalNavigationDefaultLocate

	--- @param x number
	--- @param y number
	--- @param item table
	local function selectItem(x, y, item)
		local angleDiff = math.abs(Math.vecAngBetDir(x, y, angle))
		local sqDist = x * x + y * y

		if
			Math.angDiff(angleDiff, math.pi) < (bestAngle or math.pi / 2)
			and sqDist < (bestSquareDistance or math.huge)
		then
			bestAngle = angleDiff
			bestSquareDistance = sqDist
			bestItem = item
		end
	end

	for _, item in ipairs(args.items) do
		local x, y = locate(item)
		if type(x) == "number" and y then
			selectItem(x, y, item)
		elseif type(x) == "table" then
			for _, position in ipairs(x) do
				selectItem(position[1], position[2], item)
			end
		end
	end

	return bestItem
end

return GUtilsAlgorithms
