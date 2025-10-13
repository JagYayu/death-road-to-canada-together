--[[
-- @module dr2c.Shared.Forth.Character
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require "TE.Enum"

--- @class dr2c.GForthCharacter
local GForthCharacter = {}

--- @alias dr2c.GForthCharacterData dr2c.GForthCharacter.Data

GForthCharacter.Data = Enum.protocol({
	Name = 1,
	Appearance = 2,
	Perks = 3,
	Stats = 4,
	Traits = 5,
})

function GForthCharacter.random()
	--- @class dr2c.ForthCharacter
	--- @field [dr2c.GForthCharacterData] any
	local forthCharacter = {}

	-- GForthCharacter.Data.Perks

	return forthCharacter
end

return GForthCharacter
