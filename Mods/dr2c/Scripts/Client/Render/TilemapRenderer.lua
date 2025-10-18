--[[
-- @module dr2c.Client.Render.TilemapRenderer
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CEntityECS = require("dr2c.Client.Entity.ECS")
local CRenderFocus = require("dr2c.Client.Render.Focus")
local CWorldScenes = require("dr2c.Client.World.Scenes")
local CRenderCamera      = require("dr2c.Client.Render.Camera")
local CRenderSprites = require("dr2c.Client.Render.Sprites")
local CTileMap = require("dr2c.Client.Tile.Map")
local CTileSchema = require("dr2c.Client.Tile.Schema")

local CRenderSprites_getSpriteTable = CRenderSprites.getSpriteTable
local CTileMap_getTileInfoAt = CTileMap.getTileInfoAt
local CTileMap_getTileTagAt = CTileMap.getTileTagAt
local CTileSchema_Tag_Floor = CTileSchema.Tag.Floor
local CTileSchema_Tag_Wall = CTileSchema.Tag.Wall

--- @type TE.NoiseRandom
local noiseRandom

local tileSize = 16
local ceilSize = 8
local halfTileSize = tileSize * 0.5

--- @param tx integer
--- @param ty integer
--- @param info dr2c.TileInfo
local function getFloorSpriteTable(tx, ty, info)
	local sprite = info.sprite
	if sprite then
		return CRenderSprites_getSpriteTable(sprite, noiseRandom:int2(tx, ty, 1, 4))
	end
end

--- @param renderer TE.Renderer
--- @param args TE.DrawRectArgs
--- @param tx integer
--- @param ty integer
--- @param info dr2c.TileInfo
local function drawTileFloor(renderer, args, tileMap, tx, ty, info)
	local spriteTable = getFloorSpriteTable(tx, ty, info)
	if not spriteTable then
		return
	end

	local argsSrc = args.source
	local argsDst = args.destination

	args.texture = spriteTable[0]
	argsSrc.x = spriteTable[1]
	argsSrc.y = spriteTable[2]
	argsSrc.w = spriteTable[3]
	argsSrc.h = spriteTable[4]
	argsDst.x = (tx - 1) * tileSize
	argsDst.y = (ty - 1) * tileSize
	argsDst.w = tileSize
	argsDst.h = tileSize

	renderer:drawRect(args)
end

--- @param renderer TE.Renderer
--- @param args TE.DrawRectArgs
--- @param tileMap dr2c.TileMap
--- @param tx integer
--- @param ty integer
--- @param info dr2c.TileInfo
local function drawTileFloorLeftPart(renderer, args, tileMap, tx, ty, info)
	local spriteTable = getFloorSpriteTable(tx, ty, info)
	if not spriteTable then
		return
	end

	local argsSrc = args.source
	local argsDst = args.destination

	args.texture = spriteTable[0]
	argsSrc.x = spriteTable[1]
	argsSrc.y = spriteTable[2]
	argsSrc.w = spriteTable[3] * 0.5
	argsSrc.h = spriteTable[4]
	argsDst.x = (tx - 1) * tileSize
	argsDst.y = (ty - 1) * tileSize
	argsDst.w = halfTileSize
	argsDst.h = tileSize

	renderer:drawRect(args)
end

--- @param renderer TE.Renderer
--- @param args TE.DrawRectArgs
--- @param tileMap dr2c.TileMap
--- @param tx integer
--- @param ty integer
--- @param info dr2c.TileInfo
local function drawTileFloorRightPart(renderer, args, tileMap, tx, ty, info)
	local spriteTable = getFloorSpriteTable(tx, ty, info)
	if not spriteTable then
		return
	end

	local argsSrc = args.source
	local argsDst = args.destination

	local srcW = spriteTable[3] * 0.5

	args.texture = spriteTable[0]
	argsSrc.x = spriteTable[1] + srcW
	argsSrc.y = spriteTable[2]
	argsSrc.w = srcW
	argsSrc.h = spriteTable[4]
	argsDst.x = (tx - 1) * tileSize + halfTileSize
	argsDst.y = (ty - 1) * tileSize
	argsDst.w = halfTileSize
	argsDst.h = tileSize

	renderer:drawRect(args)
end

--- @param renderer TE.Renderer
--- @param args TE.DrawRectArgs
--- @param tileMap dr2c.TileMap
--- @param tx integer
--- @param ty integer
--- @param info dr2c.TileInfo
local function drawTileFloorUnderWall(renderer, args, tileMap, tx, ty, info)
	if not info.floor then
		return
	end

	local floorInfo = CTileSchema.getTypeInfo(info.floor)
	if not floorInfo then
		return
	end

	local left = CTileMap_getTileTagAt(tileMap, tx - 1, ty)
	local right = CTileMap_getTileTagAt(tileMap, tx + 1, ty)
	if left then
		if right then
			drawTileFloor(renderer, args, tileMap, tx, ty, floorInfo)
		else
			drawTileFloorLeftPart(renderer, args, tileMap, tx, ty, floorInfo)
		end
	else
		if right then
			drawTileFloorRightPart(renderer, args, tileMap, tx, ty, floorInfo)
		end
	end
end

--- @param tileMap dr2c.TileMap
--- @param tx integer
--- @param ty integer
--- @return integer? wallIndex
--- @return integer wallCeilingIndex
local function getWallSpriteIndices(tileMap, tx, ty)
	local up = CTileMap_getTileTagAt(tileMap, tx, ty - 1) == CTileSchema_Tag_Wall
	local right = CTileMap_getTileTagAt(tileMap, tx + 1, ty) == CTileSchema_Tag_Wall
	local down = CTileMap_getTileTagAt(tileMap, tx, ty + 1) == CTileSchema_Tag_Wall
	local left = CTileMap_getTileTagAt(tileMap, tx - 1, ty) == CTileSchema_Tag_Wall

	if up then
		if right then
			if down then
				if left then
					return noiseRandom:int2(tx, ty, 4, 8), 16
				else
					return 2, 8
				end
			else
				if left then
					return noiseRandom:int2(tx, ty, 4, 8), 12
				else
					return 2, 4
				end
			end
		else
			if down then
				if left then
					return 3, 14
				else
					return 1, 6
				end
			else
				if left then
					return 3, 10
				else
					return 1, 2
				end
			end
		end
	else
		if right then
			if down then
				if left then
					return noiseRandom:int2(tx, ty, 4, 8), 15
				else
					return 2, 7
				end
			else
				if left then
					return noiseRandom:int2(tx, ty, 4, 8), 11
				else
					return 2, 3
				end
			end
		else
			if down then
				if left then
					return 3, 13
				else
					return 1, 5
				end
			else
				if left then
					return 3, 9
				else
					return 1, 1
				end
			end
		end
	end
end

--- @param renderer TE.Renderer
--- @param args TE.DrawRectArgs
--- @param tx integer
--- @param ty integer
--- @param info dr2c.TileInfo
local function drawTileWall(renderer, args, tileMap, tx, ty, info)
	drawTileFloorUnderWall(renderer, args, tileMap, tx, ty, info)

	local spriteTable
	local argsSrc = args.source
	local argsDst = args.destination
	local wallIndex, wellCeilingIndex = getWallSpriteIndices(tileMap, tx, ty)

	spriteTable = wallIndex and info.sprite and CRenderSprites_getSpriteTable(info.sprite, wallIndex)
	if spriteTable then
		args.texture = spriteTable[0]
		argsSrc.x = spriteTable[1]
		argsSrc.y = spriteTable[2]
		argsSrc.w = spriteTable[3]
		argsSrc.h = spriteTable[4]
		argsDst.x = (tx - 1) * tileSize
		argsDst.y = (ty - 1) * tileSize
		argsDst.w = tileSize
		argsDst.h = tileSize

		renderer:drawRect(args)
	end

	spriteTable = CRenderSprites_getSpriteTable("WallCeiling", wellCeilingIndex)
	if spriteTable then
		args.texture = spriteTable[0]
		argsSrc.x = spriteTable[1]
		argsSrc.y = spriteTable[2]
		argsSrc.w = spriteTable[3]
		argsSrc.h = spriteTable[4]
		argsDst.x = (tx - 1) * tileSize
		argsDst.y = (ty - 2) * tileSize
		argsDst.w = tileSize
		argsDst.h = tileSize

		renderer:drawRect(args)
	end
end

local drawTileFunctions = {
	[CTileSchema.Tag.Floor] = drawTileFloor,
	[CTileSchema_Tag_Wall] = drawTileWall,
}

--- @param renderer TE.Renderer
--- @param tileMap dr2c.TileMap
local function drawTileMap(renderer, tileMap)
	if not noiseRandom then
		noiseRandom = PerlinNoiseRandom()
		noiseRandom:setSeed(67223) -- TODO
		noiseRandom:setFrequency(0.875)
	end

	local mapX, mapY, mapWidth, mapHeight = CTileMap.getBounds(tileMap)
	local args = DrawRectArgs()
	args.source = { x = 0, y = 0, w = 0, h = 0 }

	for ty = mapY, mapY + mapHeight - 1 do
		for tx = mapX, mapX + mapWidth - 1 do
			local tileInfo = CTileMap_getTileInfoAt(tileMap, tx, ty)
			local drawTile = tileInfo and drawTileFunctions[tileInfo.tag or false]

			if drawTile then
				--- @cast tileInfo dr2c.TileInfo
				drawTile(renderer, args, tileMap, tx, ty, tileInfo)
			end
		end
	end
end

--- @param e dr2c.E.CRender
TE.events:add(N_("CRenderCamera"), function(e)
	local sceneIndex, sceneName = CRenderFocus.getFocusedScene()
	local tileMap = CTileMap.getTileMapOnScene(sceneIndex)

	if tileMap then
		drawTileMap(e.renderer, tileMap)
	end
end, "RenderTilemap", "Tilemap")

--- @param e dr2c.E.CRender
TE.events:add(N_("CRenderCamera"), function(e)
	local cameraX, cameraY = CRenderCamera.getCenter()
	-- e.renderer:
end, "", "Debug")
