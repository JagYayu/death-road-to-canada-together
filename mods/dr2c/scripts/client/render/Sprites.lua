--[[
-- @module dr2c.client.render.Sprites
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class dr2c.SpriteTable : FRect
--- @field [0] ImageID

--- @class dr2c.CSprites
local CSprites = {}

local Table = require("tudov.Table")
local json = require("json")

local tonumber = tonumber

local reloadPending = true

--- @type table<string, dr2c.SpriteTable[]>
local spriteTablesMap = {}

local eventSpritesLoad = events:new(N_("SpritesLoad"), {
	"Register",
})

local function registerSpritesImpl(e, sprites)
	local dst = e.new

	for spriteName, spriteInfos in pairs(sprites) do
		for index, spriteInfo in ipairs(spriteInfos) do
			local imageName = spriteInfo.image
			local imageID = images:getID(imageName)

			if imageID then
				local spriteTable = {
					spriteInfo.x,
					spriteInfo.y,
					spriteInfo.w,
					spriteInfo.h,
					[0] = imageID,
				}

				dst[spriteName] = dst[spriteName] or {}
				dst[spriteName][index] = spriteTable
			elseif log.canWarn() then
				log.warn(('Cannot find image "%s"'):format(imageName))
			end
		end
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

function CSprites.registerSpritesFromJsonDirectory(path, options)
	if not vfs:exists(path) then
		error(('Directory "%s" does not exist!'):format(path))
	end

	options = options
		or bit.bor(EPathListOption.File, EPathListOption.Recursed, EPathListOption.Sorted, EPathListOption.FullPath)
	local entries = vfs:list(path, options)

	for _, entry in ipairs(entries) do
		CSprites.registerSpritesFromJsonFile(entry.path)
	end
end

CSprites.registerSpritesFromJsonDirectory("mods/dr2c/data/sprites")

function CSprites.reloadImmediately()
	local e = {
		new = {},
		old = spriteTablesMap,
	}
	local new = e.new

	events:invoke(eventSpritesLoad, e, nil, EEventInvocation.None)

	spriteTablesMap = new

	reloadPending = false
end

function CSprites.reload()
	reloadPending = true
	engine:triggerLoadPending()
end

CSprites.reload()

events:add(N_("CLoad"), function(e)
	if reloadPending then
		CSprites.reloadImmediately()
	end
end, "loadSprites", "Sprites")

--- @param spriteName string
--- @param spriteIndex integer?
--- @return dr2c.SpriteTable?
function CSprites.getSpriteTable(spriteName, spriteIndex)
	local spriteTables = spriteTablesMap[spriteName]
	if spriteTables then
		return spriteTables[spriteIndex or 1]
	end
end

events:add("DebugSnapshot", function(e)
	e.reloadPending = reloadPending
	e.spriteTablesMap = spriteTablesMap
end, scriptName, nil, scriptName)

return CSprites
