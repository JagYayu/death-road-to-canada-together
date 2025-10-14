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

local Enum = require("TE.Enum")
local String = require("TE.String")

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

--- @type string[]?
local maleNames
--- @type string[]?
local femaleNames

--- @param names string[]?
--- @param file string
--- @return string[]
local function tryLoadCharacterNames(names, file)
	if not names then
		if TE.vfs:exists(file) then
			names = String.split(TE.vfs:readFile(file), "\r\n")
			table.sort(names)
		else
			names = {}
		end
	end

	return names
end

--- @return string
function GForthCharacter.randomMaleName()
	maleNames = tryLoadCharacterNames(maleNames, "data/mnames.txt")
	return maleNames[1] or ""
end

--- @return string
function GForthCharacter.randomFemaleName()
	maleNames = tryLoadCharacterNames(maleNames, "data/mnames.txt")
	return maleNames[1] or ""
end

function GForthCharacter.randomCharacter()
	--- @class dr2c.ForthCharacter
	--- @field [dr2c.GForthCharacterData] any
	local forthCharacter = {}

	-- GForthCharacter.Data.Perks

	return forthCharacter
end

function GForthCharacter.randomFamilyCharacter()
	error("not implement yet")
end

return GForthCharacter
