--[[
-- @module dr2c.Client.Tile.Data
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CWorldCollision = require("dr2c.Client.World.Collision")
local CTileSchema = require("dr2c.Client.Tile.Schema")

--- @class dr2c.CTileData
local CTileData = {}

TE.events:add(N_("CTileSchemaLoad"), function(e)
	for i = 1, 18 do
		e.new["Floor" .. i] = {
			tag = CTileSchema.Tag.Floor,
			collision = CWorldCollision.Type.None,
			sprite = "Floor" .. i,
		}
	end

	for i = 1, 23 do
		e.new["Wall" .. i] = {
			tag = CTileSchema.Tag.Wall,
			collision = CWorldCollision.Type.None,
			sprite = "Floor" .. i,
		}
	end
end, "RegisterTiles", "Register")

return CTileData
