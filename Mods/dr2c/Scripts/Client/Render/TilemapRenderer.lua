--[[
-- @module dr2c.Client.render.TilemapRenderer
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
local CRenderSprites = require("dr2c.Client.Render.Sprites")
local CTileMap = require("dr2c.Client.Tile.Map")
local CTileSchema = require("dr2c.Client.Tile.Schema")

local CRenderSprites_getSpriteTable = CRenderSprites.getSpriteTable

local noiseRandom

local tileSize = 16

local drawTileFunctions = {
	--- @param renderer TE.Renderer
	--- @param args TE.DrawRectArgs
	--- @param tx integer
	--- @param ty integer
	--- @param info dr2c.TileInfo
	[CTileSchema.Tag.Floor] = function(renderer, args, tx, ty, info)
		local sprite = info.sprite
		if not sprite then
			return
		end

		local spriteTable = CRenderSprites_getSpriteTable(sprite, noiseRandom:int2(tx, ty, 1, 4))
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
	end,
	--- @param renderer TE.Renderer
	--- @param args TE.DrawRectArgs
	--- @param tx integer
	--- @param ty integer
	--- @param info dr2c.TileInfo
	[CTileSchema.Tag.Wall] = function(renderer, args, tx, ty, info)
		local sprite = info.sprite
		if not sprite then
			return
		end

		local spriteTable = CRenderSprites_getSpriteTable(sprite, noiseRandom:int2(tx, ty, 1, 4))
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
	end,
}

--- @param renderer TE.Renderer
--- @param tileMap dr2c.TileMap
local function drawTileMap(renderer, tileMap)
	if not noiseRandom then
		noiseRandom = PerlinNoiseRandom()
		noiseRandom:setSeed(67223)
		noiseRandom:setFrequency(0.875)
	end

	local CTileMap_getTileInfoAt = CTileMap.getTileInfoAt

	local mapX, mapY, mapWidth, mapHeight = CTileMap.getBounds(tileMap)
	local args = DrawRectArgs()
	args.source = { x = 0, y = 0, w = 0, h = 0 }

	for ty = mapY, mapY + mapHeight do
		for tx = mapX, mapX + mapWidth do
			local tileInfo = CTileMap_getTileInfoAt(tileMap, tx, ty)
			local drawTile = tileInfo and drawTileFunctions[tileInfo.tag or false]

			if drawTile then
				--- @cast tileInfo dr2c.TileInfo
				drawTile(renderer, args, tx, ty, tileInfo)
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
