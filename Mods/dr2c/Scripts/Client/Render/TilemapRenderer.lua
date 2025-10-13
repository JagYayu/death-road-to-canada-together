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

local CECS = require("dr2c.Client.Entity.ECS")
local CRenderFocus = require("dr2c.Client.Render.Focus")
local CRoom = require("dr2c.Client.World.Room")
local CRenderSprites = require("dr2c.Client.Render.Sprites")

-- TE.events:new("", {
-- 	"",
-- }, {
-- 	"",
-- })

--- @param renderer TE.Renderer
--- @param tilemap table
local function draw(renderer, tilemap)
	local CRenderSprites_getSpriteTable = CRenderSprites.getSpriteTable

	local sizeX = tilemap.sizeX
	local sizeY = tilemap.sizeY
	local tiles = tilemap.tiles

	local drawArgs = {
		destination = {},
	}
	local drawArgsDst = drawArgs.destination

	for y = 1, sizeY do
		for x = 1, sizeX do
			local tileID = tiles[(y - 1) * sizeX + x] or 0

			local spriteTable = CRenderSprites_getSpriteTable("StreetGrass")
			if spriteTable then
				drawArgs.image = spriteTable[0]
				drawArgs.source = spriteTable
				drawArgsDst[1] = (x - 1) * 32
				drawArgsDst[2] = (y - 1) * 32
				drawArgsDst[3] = 32
				drawArgsDst[4] = 32

				renderer:drawRect(drawArgs)
			end
		end
	end
end

local testTilemap = {
	sizeX = 4,
	sizeY = 4,
	tiles = {},
}

--- @param e dr2c.E.CRender
TE.events:add(N_("CRenderCamera"), function(e)
	local room = CRoom.getRoom(CRenderFocus.getFocusedRoomID())
	if room then
		draw(e.renderer, room.tilemap)
	elseif true then
		draw(e.renderer, testTilemap)
	end
end, "RenderTilemap", "Tilemap")
