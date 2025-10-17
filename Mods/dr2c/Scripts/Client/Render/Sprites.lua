--[[
-- @module dr2c.Client.render.Sprites
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local String = require("TE.String")
local Table = require("TE.Table")
local json = require("Lib.json")

local tonumber = tonumber

--- @class dr2c.CSprites
local CRenderSprites = {}

--- @class dr2c.SpriteName : string

--- @class dr2c.SpriteIndex : integer

--- @class dr2c.SpriteTable : TE.RectangleF
--- @field [0] TE.ImageID

--- @class dr2c.Sprites
--- @field image string
--- @field x number
--- @field y number
--- @field w number
--- @field h number

local reloadPending = true

--- @type table<string, dr2c.SpriteTable[]>
local spriteTablesMap = {}

local eventCSpritesLoad = TE.events:new(N_("CSpritesLoad"), {
	"Register",
})

--- @param imageID integer
--- @param x integer
--- @param y integer
--- @param w integer
--- @param h integer
--- @return dr2c.SpriteTable
local function newSpriteTable(imageID, x, y, w, h)
	--- @diagnostic disable-next-line: return-type-mismatch
	return {
		[0] = imageID,
		x,
		y,
		w,
		h,
	}
end

CRenderSprites.Field = Table.readonly({
	Texture = 0,
	X = 1,
	Y = 2,
	Width = 3,
	Height = 4,
})

--- @param e dr2c.E.CSpritesLoad
--- @param sprites table<string, dr2c.Sprites>
local function registerSpritesImpl(e, sprites)
	local dst = e.new

	for spriteName, spriteInfos in pairs(sprites) do
		for index, spriteInfo in ipairs(spriteInfos) do
			local imageName = spriteInfo.image
			local imageID = TE.images:getID(imageName)

			if imageID then
				local spriteTable = newSpriteTable(imageID, spriteInfo.x, spriteInfo.y, spriteInfo.w, spriteInfo.h)

				dst[spriteName] = dst[spriteName] or {}
				dst[spriteName][index] = spriteTable
			elseif log.canWarn() then
				log.warn(('Cannot find image "%s"'):format(imageName))
			end
		end
	end
end

--- @param sprites table<string, dr2c.Sprites>
function CRenderSprites.registerSprites(sprites)
	if type(sprites) ~= "table" then
		error(("bad argument #1 to 'tbl' (table expected, got %s)"):format(type(sprites)))
	end

	--- @param e dr2c.E.CSpritesLoad
	TE.events:add(eventCSpritesLoad, function(e)
		registerSpritesImpl(e, sprites)
	end)
end

local function registerSpritesFromFileImpl(filePath, noLog, hint, func)
	if type(filePath) ~= "string" then
		error(("bad argument #1 to 'filePath' (string expected, got %s)"):format(type(filePath)))
	end

	TE.events:add(eventCSpritesLoad, function(e)
		if not (noLog or TE.vfs:exists(filePath)) then
			log.warn("File not found: " .. filePath)
		end

		local success, result = pcall(func, TE.vfs:readFile(filePath))
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
--- @param noLog? boolean
--- @param chunkname? string
--- @param mode? any
--- @param env? table
--- @param ... any
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
	if not TE.vfs:exists(path) then
		error(('Directory "%s" does not exist!'):format(path))
	end

	if not options then
		options = bit.bor( --
			EPathListOption.File,
			EPathListOption.Recursed,
			EPathListOption.Sorted,
			EPathListOption.FullPath
		)
	end

	local entries = TE.vfs:list(path, options)

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
	--- @class dr2c.E.CSpritesLoad
	local e = {
		new = {},
		old = spriteTablesMap,
	}
	local new = e.new

	TE.events:invoke(eventCSpritesLoad, e, nil, EEventInvocation.None)

	spriteTablesMap = new

	reloadPending = false
end

function CRenderSprites.reload()
	reloadPending = true

	TE.engine:triggerLoadPending()
end

TE.events:add(N_("CLoad"), function(e)
	if reloadPending then
		CRenderSprites.reloadImmediately()
	end
end, "loadSprites", "Sprites")

--- @param spriteName dr2c.SpriteName
--- @param spriteIndex dr2c.SpriteIndex?
--- @return dr2c.SpriteTable?
function CRenderSprites.getSpriteTable(spriteName, spriteIndex)
	local spriteTables = spriteTablesMap[spriteName]
	if spriteTables then
		return spriteTables[spriteIndex or 1]
	end
end

TE.events:add("DebugSnapshot", function(e)
	e.reloadPending = reloadPending
	e.spriteTablesMap = spriteTablesMap
end, scriptName, nil, scriptName)

CRenderSprites.registerSpritesFromLuaDirectory("Mods/dr2c/Data/Sprites")

CRenderSprites.reload()

return CRenderSprites
