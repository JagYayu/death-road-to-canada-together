--- @class SpriteTable : FRect
--- @field [0] ImageID

local CSprites = {}

local Table = require "dr2c.shared.utils.Table"

local tonumber = tonumber

--- @type table<string, SpriteTable[]>
local spriteTablesMap = {}

--- @param spriteName string
--- @param index integer?
--- @return SpriteTable?
function CSprites.getSpriteTable(spriteName, index)
	index = tonumber(index) or 1

	local spriteTables = spriteTablesMap[spriteName]
	if spriteTables then
		return spriteTables[index]
	end

	return Table.empty
end

return CSprites
