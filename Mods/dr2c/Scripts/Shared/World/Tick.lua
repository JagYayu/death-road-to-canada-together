--[[
-- @module dr2c..World.Tick
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local tps = 2 ^ 5 -- 2 ^ 3
local deltaTime = 1 / tps

local GWorldTick = {}

--- @return integer
function GWorldTick.getTPS()
	return tps
end

--- 获取刻增量时间
--- @return number
function GWorldTick.getDeltaTime()
	return deltaTime
end

return GWorldTick
