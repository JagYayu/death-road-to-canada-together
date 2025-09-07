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
local CRenderSprites = {}

local String = require("tudov.String")
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
			local imageID = fonts:getID(imageName)

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
function CRenderSprites.registerSprites(sprites)
	if type(sprites) ~= "table" then
		error(("bad argument #1 to 'tbl' (table expected, got %s)"):format(type(sprites)))
	end

	events:add(eventSpritesLoad, function(e)
		registerSpritesImpl(e, sprites)
	end)
end

local function registerSpritesFromFileImpl(filePath, noLog, hint, func)
	if type(filePath) ~= "string" then
		error(("bad argument #1 to 'filePath' (string expected, got %s)"):format(type(filePath)))
	end

	events:add(eventSpritesLoad, function(e)
		if not (noLog or vfs:exists(filePath)) then
			log.warn("File not found: " .. filePath)
		end

		local success, result = pcall(func, vfs:readFile(filePath))
		if success then
			registerSpritesImpl(e, result)
		elseif not noLog then
			log.warn(("Failed to load sprites from %s file: %s"):format(hint, filePath))
		end
	end)
end

--- @param filePath string
--- @param noLog boolean?
function CRenderSprites.registerSpritesFromJsonFile(filePath, noLog)
	registerSpritesFromFileImpl(filePath, noLog, "json", json.decode)
end

--- @param filePath string
--- @param noLog boolean?
function CRenderSprites.registerSpritesFromLuaFile(filePath, noLog, chunkname, mode, env, ...)
	local args = { ... }

	registerSpritesFromFileImpl(filePath, noLog, "lua", function(text)
		local func, errorMessage = load(text, chunkname, mode, env)
		if func then
			return func(unpack(args))
		else
			error(errorMessage, 2)
		end
	end)
end

local function registerSpritesFromDirectoryImpl(path, options, suffix, func)
	if not vfs:exists(path) then
		error(('Directory "%s" does not exist!'):format(path))
	end

	options = options
		or bit.bor(EPathListOption.File, EPathListOption.Recursed, EPathListOption.Sorted, EPathListOption.FullPath)
	local entries = vfs:list(path, options)

	for _, entry in ipairs(entries) do
		if String.hasSuffix(entry.path, suffix) then
			func(entry.path)
		end
	end
end

--- @param path string
--- @param options any
function CRenderSprites.registerSpritesFromJsonDirectory(path, options)
	registerSpritesFromDirectoryImpl(path, options, ".json", CRenderSprites.registerSpritesFromJsonFile)
end

--- @param path string
--- @param options any
function CRenderSprites.registerSpritesFromLuaDirectory(path, options)
	registerSpritesFromDirectoryImpl(path, options, ".lua", CRenderSprites.registerSpritesFromLuaFile)
end

function CRenderSprites.reloadImmediately()
	local e = {
		new = {},
		old = spriteTablesMap,
	}
	local new = e.new

	events:invoke(eventSpritesLoad, e, nil, EEventInvocation.None)

	spriteTablesMap = new

	reloadPending = false
end

function CRenderSprites.reload()
	reloadPending = true
	engine:triggerLoadPending()
end

events:add(N_("CLoad"), function(e)
	if reloadPending then
		CRenderSprites.reloadImmediately()
	end
end, "loadSprites", "Sprites")

--- @param spriteName string
--- @param spriteIndex integer?
--- @return dr2c.SpriteTable?
function CRenderSprites.getSpriteTable(spriteName, spriteIndex)
	local spriteTables = spriteTablesMap[spriteName]
	if spriteTables then
		return spriteTables[spriteIndex or 1]
	end
end

events:add("DebugSnapshot", function(e)
	e.reloadPending = reloadPending
	e.spriteTablesMap = spriteTablesMap
end, scriptName, nil, scriptName)

CRenderSprites.registerSpritesFromJsonDirectory("mods/dr2c/data/sprites")
CRenderSprites.registerSpritesFromLuaDirectory("mods/dr2c/data/sprites")

CRenderSprites.reload()

return CRenderSprites
