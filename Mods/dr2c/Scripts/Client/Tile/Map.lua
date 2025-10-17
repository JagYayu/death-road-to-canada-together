--[[
-- @module dr2c.Client.Tile.Map
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CTileSchema = require("dr2c.Client.Tile.Schema")
local CWorldScenes = require("dr2c.Client.World.Scenes")
local CWorldSession = require("dr2c.Shared.World.Session")
local Table = require("TE.Table")

local CTileSchema_getInfo = CTileSchema.getTypeInfo
local math_floor = math.floor

--- TODO 无JIT时测试，对高频访问的函数进行内联展开，如果性能几乎没影响则无需改动
--- @class dr2c.CTileMap
local CTileMap = {}

local fieldX = 1
local fieldY = 2
local fieldWidth = 3
local fieldHeight = 4
local fieldTypeIDs = 5

local function newTileMap(x, y, width, height, typeIDs)
	--- @class dr2c.TileMap
	--- @field [1] integer
	--- @field [2] integer
	--- @field [3] integer
	--- @field [4] integer
	--- @field [5] dr2c.TileTypeID[]
	return { -- 确保字段存到了数组部分，而不是哈希部分
		tonumber(x) or 0,
		tonumber(y) or 0,
		math.max(1, tonumber(width) or 0),
		math.max(1, tonumber(height) or 0),
		typeIDs,
	}
end

--- @type table<dr2c.WorldSceneName, dr2c.TileMap>
local sceneTileMaps = {}

sceneTileMaps = persist("sceneTileMaps", function()
	return sceneTileMaps
end)

--- @param tileMap dr2c.TileMap
--- @param sceneID dr2c.WorldSceneName
function CTileMap.setTileMapOnScene(tileMap, sceneID)
	sceneTileMaps[sceneID] = tileMap
end

--- @param sceneIndexOrName dr2c.WorldSceneIndex | dr2c.WorldSceneName
--- @return dr2c.TileMap?
function CTileMap.getTileMapOnScene(sceneIndexOrName)
	return sceneTileMaps[sceneIndexOrName]
end

--- @param width integer
--- @param height integer
--- @param types (dr2c.TileType | dr2c.TileTypeID)[]
--- @return dr2c.TileMap
function CTileMap.newTileMap(x, y, width, height, types)
	local typeIDs = Table.new(#types, 0)
	for index, tileType in ipairs(types) do
		--- @diagnostic disable-next-line: param-type-mismatch
		typeIDs[index] = CTileSchema.getTileTypeID(tileType) or 0
	end

	return newTileMap(
		tonumber(x) or 0,
		tonumber(y) or 0,
		math.max(1, tonumber(width) or 0),
		math.max(1, tonumber(height) or 0),
		typeIDs
	)
end

--- @param tileMap dr2c.TileMap
--- @return integer x
--- @return integer y
--- @return integer width
--- @return integer height
function CTileMap.getBounds(tileMap)
	return tileMap[fieldX], tileMap[fieldY], tileMap[fieldWidth], tileMap[fieldHeight]
end

--- @param tileMap dr2c.TileMap
--- @return dr2c.TileTypeID[]
function CTileMap.getTileTypeIDs(tileMap)
	return tileMap[fieldTypeIDs]
end

--- @param tileMap dr2c.TileMap
--- @param tx integer
--- @param ty integer
--- @return boolean
function CTileMap.isInBound(tileMap, tx, ty)
	local boundX = tileMap[fieldX]
	local boundY = tileMap[fieldY]
	return tx >= boundX and tx < boundX + tileMap[fieldWidth] and ty >= boundY and ty < boundY + tileMap[fieldHeight]
end

local CTileMap_isInBound = CTileMap.isInBound

--- @param tileMap dr2c.TileMap
--- @param tx integer
--- @param ty integer
--- @return integer ti
function CTileMap.coord2index(tileMap, tx, ty)
	return (ty - tileMap[fieldY]) * tileMap[fieldWidth] + (tx - tileMap[fieldX])
end

local CTileMap_coord2index = CTileMap.coord2index

--- @param tileMap dr2c.TileMap
--- @param ti integer
--- @return integer tx
--- @return integer ty
function CTileMap.index2coord(tileMap, ti)
	local width = tileMap[fieldWidth]
	return tileMap[fieldX] + ti % width, tileMap[fieldY] + math_floor(ti / width)
end

--- @param tileMap dr2c.TileMap
--- @param ti integer
--- @return dr2c.TileInfo?
function CTileMap.getTileInfoAtIndex(tileMap, ti)
	local typeID = tileMap[fieldTypeIDs][ti + 1]
	return typeID and CTileSchema_getInfo(typeID)
end

local CTileMap_getTileInfoAtIndex = CTileMap.getTileInfoAtIndex

--- @param tileMap dr2c.TileMap
--- @param tx integer
--- @param ty integer
--- @return dr2c.TileInfo?
function CTileMap.getTileInfoAt(tileMap, tx, ty)
	if CTileMap_isInBound(tileMap, tx, ty) then
		return CTileMap_getTileInfoAtIndex(tileMap, CTileMap_coord2index(tileMap, tx, ty))
	end
end

local CTileMap_getTileInfoAt = CTileMap.getTileInfoAt

--- @param tileMap dr2c.TileMap
--- @param tx integer
--- @param ty integer
--- @return dr2c.TileTag?
function CTileMap.getTileTagAt(tileMap, tx, ty)
	local tileInfo = CTileMap_getTileInfoAt(tileMap, tx, ty)
	return tileInfo and tileInfo.tag or nil
end

--- @param e dr2c.E.CWorldSessionStart
TE.events:add(N_("CWorldSessionStart"), function(e)
	if log.canDebug() then
		log.debug("Loading tile maps from session attributes")
	end

	local tileMaps = e.attributes[CWorldSession.Attribute.TileMaps]
	if type(tileMaps) ~= "table" then
		return
	end

	for _, sceneName in ipairs(CWorldScenes.getSceneList()) do
		local tileMap = tileMaps[sceneName]
		if tileMap then
			local sceneIndex = assert(CWorldScenes.invert(sceneName))

			local new = newTileMap(
				tileMap[fieldX],
				tileMap[fieldY],
				tileMap[fieldWidth],
				tileMap[fieldHeight],
				tileMap[fieldTypeIDs]
			)

			sceneTileMaps[sceneIndex] = new
			sceneTileMaps[sceneName] = new
		end
	end
end, "LoadTileMaps", "Map")

return CTileMap
