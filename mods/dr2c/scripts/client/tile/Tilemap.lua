local CTilemap = {}

--- @type integer
local tilemapNextID
tilemapNextID = persist("tilemapNextID", 1, function()
	return tilemapNextID
end)

function CTilemap.newTilemap() end

return CTilemap
