--[[
-- @module dr2c.Shared.Forth.Stat
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")

--- Character's status
--- @class dr2c.GForthStat
local GForthStat = {}

GForthStat.Type = Enum.protocol({
	Morale = 1, -- 士气
	Vitality = 2, -- 活力
	Dexterity = 3, -- 灵敏
	Strength = 4, -- 力量
	Fitness = 5, -- 体力
	Shooting = 6, -- 射击
	Medical = 7, -- 医疗
	Mechanical = 8, -- 机械
	Wits = 9, -- 智慧
	Attitude = 10, -- 态度
	Composure = 11, -- 冷静
	Loyalty = 12, -- 忠诚
	CoolIt = 13, -- 别急
})

return GForthStat
