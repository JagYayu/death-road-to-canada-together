--[[
-- @module dr2c.Shared.Forth.GameMode
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")

--- @class dr2c.GForthGameMode
local GForthGameMode = {}

--- @alias dr2c.ForthGameModeType dr2c.GForthGameMode.Type

GForthGameMode.Type = Enum.protocol({
	Unknown = 0,

	Normal = 1, -- Death Road Normal Mode
	Family = 2, -- Familiar Characters Mode
	Rare = 3, -- Rare Characters Mode
	Short = 4, -- Short Trip to Heck Mode
	Long = 5, -- Long Winding Road Mode
	Group = 6, -- Four Jerks Mode
	Hard = 7, -- Deadlier Road Mode
	HardFamily = 8, -- Familiar EXTREME
	HardRare = 9, -- Rare EXTREME
	HardShort = 10, -- Quick Death Mode
	HardLong = 11, -- Marathon Mode
	HardGroup = 12, -- Four Jerks EXTREME
	Oop = 13, -- O*O*P Mode
	Kepa = 14, -- K*E*P*A Mode
	Endless = 15, -- ENDLESS Mode
})

return GForthGameMode
