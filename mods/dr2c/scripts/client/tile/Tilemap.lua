--[[
-- @module dr2c.client.tile.Tilemap
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CTilemap = {}

--- @type integer
local tilemapNextID
tilemapNextID = persist("tilemapNextID", 1, function()
	return tilemapNextID
end)

function CTilemap.newTilemap() end

return CTilemap
