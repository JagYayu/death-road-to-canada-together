--- @class dr2c.SpriteTable : FRect
--- @field [0] ImageID

--- @class dr2c.CSprites
local CSprites = {}

local GTable = require("dr2c.shared.utils.Table")
local json = require("json")

local tonumber = tonumber

local reloadPending = true

--- @type table<string, dr2c.SpriteTable[]>
local spriteTablesMap = {}

events:add("DebugSnapshot", function(e)
	e.reloadPending = reloadPending
	e.spriteTablesMap = spriteTablesMap
end, nil, nil, scriptName)

local eventSpritesLoad = events:new(N_("SpritesLoad"), {
	"Register",
})

local function registerSpritesImpl(e, sprites)
	local dst = e.new

	for spriteName, spriteInfo in pairs(sprites) do
		dst[spriteName] = spriteInfo
	end
end

--- @param sprites table<string, dr2c.SpriteTable>
function CSprites.registerSprites(sprites)
	if type(sprites) ~= "table" then
		error(("bad argument #1 to 'tbl' (table expected, got %s)"):format(type(sprites)))
	end

	events:add(eventSpritesLoad, function(e)
		registerSpritesImpl(e, sprites)
	end)
end

--- @param filePath string
--- @param noLog boolean?
function CSprites.registerSpritesFromJsonFile(filePath, noLog)
	if type(filePath) ~= "string" then
		error(("bad argument #1 to 'filePath' (string expected, got %s)"):format(type(filePath)))
	end

	events:add(eventSpritesLoad, function(e)
		if not (noLog or vfs:exists(filePath)) then
			log.warn("File not found: " .. filePath)
		end

		local success, result = pcall(json.decode, vfs:readFile(filePath))
		if success then
			registerSpritesImpl(e, result)
		elseif not noLog then
			log.warn("Failed to load sprites from JSON file: " .. filePath)
		end
	end)
end

function CSprites.reloadImmediately()
	local e = {
		new = {},
		old = spriteTablesMap,
	}
	local tempSpriteTablesMap = e.new

	events:invoke(eventSpritesLoad, e, nil, EEventInvocation.None)

	-- print(tempSpriteTablesMap)

	reloadPending = false
end

function CSprites.reload()
	reloadPending = true
	engine:triggerLoadPending()
end

CSprites.reload()

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
		CSprites.registerSpritesFromJsonFile(entry.path, false)
	end
else
	error(('builtin directory "%s" does not exist!'):format(defaultSpritesDirectory))
end

--- @param spriteName string
--- @param spriteIndex integer?
--- @return dr2c.SpriteTable?
function CSprites.getSpriteTable(spriteName, spriteIndex)
	local spriteTables = spriteTablesMap[spriteName]
	if spriteTables then
		return spriteTables[tonumber(spriteIndex) or 1]
	end
end

-- print(network:getClient().sendReliable)

return CSprites
