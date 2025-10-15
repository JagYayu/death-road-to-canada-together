--[[
-- @module dr2c.Client.Tile.Schema
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local type = type

--- @class dr2c.CTileSchema
local CTileSchema = {}

--- @class dr2c.TileType : string

--- @class dr2c.TileTypeID : integer

--- @class dr2c.TileInfo
--- @field type dr2c.TileType
--- @field typeID dr2c.TileTypeID
--- @field collision dr2c.CWorldCollisionFlag
--- @field sprite { [1]: dr2c.SpriteName, [2]: dr2c.SpriteIndex? }

--- @class dr2c.CTileSchema.Tiles
--- @field [dr2c.TileTypeID] dr2c.TileInfo
--- @field [dr2c.TileType] dr2c.TileInfo

local reloadPending = true

--- @type dr2c.CTileSchema.Tiles
local tilesSchema = {}
tilesSchema = persist("tileSchema", function()
	return tilesSchema
end)

--- @param tileTypeOrID dr2c.TileType | dr2c.TileTypeID
--- @return dr2c.TileInfo? tileInfo
function CTileSchema.getInfo(tileTypeOrID)
	return tilesSchema[tileTypeOrID]
end

--- @param tileTypeOrID dr2c.TileType | dr2c.TileTypeID
--- @return dr2c.CWorldCollisionFlag? flags
function CTileSchema.getCollision(tileTypeOrID)
	local tileSchema = tilesSchema[tileTypeOrID]
	return tileSchema and tileSchema.collision or nil
end

--- @param tileTypeOrID dr2c.TileType | dr2c.TileTypeID
--- @return dr2c.SpriteName? spriteName
--- @return dr2c.SpriteIndex? spriteIndex
function CTileSchema.getSprite(tileTypeOrID)
	local tileSchema = tilesSchema[tileTypeOrID]
	if tileSchema then
		return tileSchema[1], tileSchema[2]
	end
end

CTileSchema.eventCTileSchemaLoaded = TE.events:new(N_("CTileSchemaLoaded"), {
	"",
})

function CTileSchema.reloadImmediately()
	TE.events:invoke(CTileSchema.eventCTileSchemaLoaded, {})

	reloadPending = false
end

function CTileSchema.reload()
	reloadPending = true

	TE.engine:triggerLoadPending()
end

CTileSchema.reload()

--- @param e dr2c.E.CLoad
TE.events:add(N_("CLoad"), function(e)
	if reloadPending then
		CTileSchema.reloadImmediately()

		--- @class dr2c.E.CLoad
		--- @field tilesSchema? table
		e.tilesSchema = tilesSchema
	end
end, "loadTileSchema", "Tile")

return CTileSchema
