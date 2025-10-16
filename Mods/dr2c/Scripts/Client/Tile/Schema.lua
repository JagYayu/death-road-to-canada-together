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

local Enum = require("TE.Enum")
local Table = require("TE.Table")
local stringBuffer = require("string.buffer")

local CWorldCollision = require("dr2c.Client.World.Collision")

local type = type

--- @class dr2c.CTileSchema
local CTileSchema = {}

--- @alias dr2c.TileTag dr2c.CTileSchema.Tag

CTileSchema.Tag = Enum.immutable({
	Floor = 1,
	Wall = 2,
})

--- @class dr2c.TileType : string

--- @class dr2c.TileTypeID : integer

--- @class dr2c.TileInfo
--- @field type dr2c.TileType
--- @field typeID dr2c.TileTypeID
--- @field tag? dr2c.TileTag
--- @field collision? dr2c.CWorldCollisionFlag
--- @field sprite dr2c.SpriteName

--- @class dr2c.CTileSchema.Tiles : table
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

--- @param tileTypeID dr2c.TileTypeID
--- @return dr2c.TileType tileType
function CTileSchema.getTileType(tileTypeID)
	local tileSchema = tilesSchema[tileTypeID]
	return tileSchema and tileSchema.type or nil
end

--- @param tileType dr2c.TileType
--- @return dr2c.TileTypeID tileTypeID
function CTileSchema.getTileTypeID(tileType)
	local tileSchema = tilesSchema[tileType]
	return tileSchema and tileSchema.typeID or nil
end

CTileSchema.eventCTileSchemaLoad = TE.events:new(N_("CTileSchemaLoad"), {
	"Register",
	"Override",
	"Validate",
	"Finalize",
})

CTileSchema.eventCTileSchemaLoaded = TE.events:new(N_("CTileSchemaLoaded"), {
	"",
})

function CTileSchema.reloadImmediately()
	local e = {
		new = {},
		old = tilesSchema,
	}
	TE.events:invoke(CTileSchema.eventCTileSchemaLoad, e)

	tilesSchema = {
		[0] = {
			type = "",
			typeID = 0,
			collision = CWorldCollision.Type.All,
		},
	}

	for state, tileType, tileDef in Table.sortedPairs(e.new) do
		local tileTypeID = state[2]
		if not tilesSchema[tileTypeID] then
			tileType = tostring(tileType)

			--- @type dr2c.TileInfo
			local tileInfo = Table.copy(tileDef)
			tileInfo.type = tileType
			tileInfo.typeID = tileTypeID

			tilesSchema[tileType] = tileInfo
			tilesSchema[tileTypeID] = tileInfo
		end
	end

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
		--- @field tilesSchema? dr2c.CTileSchema.Tiles
		e.tilesSchema = tilesSchema
	end
end, "loadTileSchema", "Tile")

return CTileSchema
