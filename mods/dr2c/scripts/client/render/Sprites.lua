--- @class SpriteTable : FRect
--- @field [0] ImageID

local CSprites = {}

local GTable = require("dr2c.shared.utils.Table")
local json = require("json")

local tonumber = tonumber

local reloadPending = true

--- @type table<string, SpriteTable[]>
local spriteTablesMap = {}

function CSprites.registerSprites()
	--
end

local eventSpritesLoadName = N_("SpritesLoad")

function CSprites.registerSpritesFromJsonFile(filePath)
	-- events:add(eventSpritesLoadName, func, name?, order?, key?, sequence?)
	local t = json.decode(vfs:readFile(filePath))
end

local eventSpritesLoad = events:new(eventSpritesLoadName, {
	"Register",
})

function CSprites.reloadImmediately()
	reloadPending = false
end

function CSprites.reload()
	reloadPending = true
end

events:add(N_("ContentLoad"), function(e)
	if reloadPending then
		CSprites.reloadImmediately()
	end
end, "loadSprites", "Sprites")

local defaultSpritesDirectory = "mods/dr2c/data/sprites"
if vfs:exists(defaultSpritesDirectory) then
	local entries = vfs:list(
		defaultSpritesDirectory,
		bit.bor(EPathListOption.File, EPathListOption.Recursed, EPathListOption.Sorted, EPathListOption.FullPath)
	)

	for _, entry in ipairs(entries) do
		local success, errorMessage = pcall(CSprites.registerSpritesFromJsonFile, entry.path)
		if not success then
			log.error(('Failed to load sprite registration file "%s": %s'):format(entry.path, errorMessage))
		end
	end
else
	error(('Builtin directory "%s" does not exist!'):format(defaultSpritesDirectory))
end

--- @param spriteName string
--- @param index integer?
--- @return SpriteTable?
function CSprites.getSpriteTable(spriteName, index)
	index = tonumber(index) or 1

	local spriteTables = spriteTablesMap[spriteName]
	if spriteTables then
		return spriteTables[index]
	end
end

return CSprites
