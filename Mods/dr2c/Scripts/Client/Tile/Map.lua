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

local CTileSchema_getInfo = CTileSchema.getInfo
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
	return { -- 确保字段存到了数组部分，而不是哈希部分
		tonumber(x) or 0,
		tonumber(y) or 0,
		math.max(1, tonumber(width) or 0),
		math.max(1, tonumber(height) or 0),
		typeIDs,
	}
end

--- @type table<dr2c.WorldSceneID, dr2c.TileMap>
local sceneTileMaps = {}

sceneTileMaps = persist("sceneTileMaps", function()
	return sceneTileMaps
end)

--- @param sceneID dr2c.WorldSceneID
--- @return dr2c.TileMap?
function CTileMap.getTileMapOnScene(sceneID)
	return sceneTileMaps[sceneID]
end

--- @param width integer
--- @param height integer
--- @param typeIDs dr2c.TileMap
--- @param sceneID dr2c.WorldSceneID
--- @return dr2c.TileMap
function CTileMap.newTileMap(x, y, width, height, typeIDs, sceneID)
	--- @class dr2c.TileMap
	local tileMap = newTileMap(
		tonumber(x) or 0,
		tonumber(y) or 0,
		math.max(1, tonumber(width) or 0),
		math.max(1, tonumber(height) or 0),
		typeIDs
	)

	if sceneID then
		sceneTileMaps[sceneID] = tileMap
	end

	return tileMap
end

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
function CTileMap.getTileAtIndex(tileMap, ti)
	local typeID = tileMap[fieldTypeIDs][ti]
	return typeID and CTileSchema_getInfo(typeID)
end

local CTileMap_getTileAtIndex = CTileMap.getTileAtIndex

--- @param tileMap dr2c.TileMap
--- @param tx integer
--- @param ty integer
--- @return dr2c.TileInfo?
function CTileMap.getTileInfoAt(tileMap, tx, ty)
	return CTileMap_getTileAtIndex(tileMap, CTileMap_coord2index(tileMap, tx, ty))
end

--- @param e dr2c.E.CWorldSessionStart
TE.events:add(N_("CWorldSessionStart"), function(e)
	if log.canDebug() then
		log.debug("Loading tile maps from session attributes")
	end
end, "LoadTileMaps", "Map")

return CTileMap
