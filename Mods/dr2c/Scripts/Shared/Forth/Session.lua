--[[
-- @module dr2c.Shared.Forth.Session
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")

--- Road Traveling Session (The name Forth is actually absolutely incorrect)
--- 公路旅行会话（实际上Forth这个命名完全不正确）
--- @class dr2c.GForthSession
local GForthSession = {}

--- @alias dr2c.ForthSessionAttribute dr2c.GForthSession.Attribute

GForthSession.Attribute = Enum.protocol({
	-- 是否正在进行
	Active = 1,
	-- App版本
	Version = 2,
	-- 游戏Mods
	Mods = 3,
	-- 游戏Mods哈希值，用于快速判断
	ModsHash = 4,
	-- 游戏模式
	GameMode = 5,
	-- 创建时间
	CreateTime = 6,
	-- 上次保存时间
	LastSaveTime = 7,
	-- 游玩时间
	Playtime = 8,
	-- 小队角色
	Characters = 9,
	-- 小队载具
	Vehicle = 10,
	-- 小队背包
	Inventory = 11,
})

local GForthSession_Attribute_Active = GForthSession.Attribute.Active

--- @type table<string, dr2c.ForthSession>
local forthSessionModules = {}

forthSessionModules = persist("forthSessionModules", function()
	return forthSessionModules
end)

--- @return dr2c.ForthSession ForthSession
function GForthSession.new()
	--- @class dr2c.ForthSession
	local ForthSession

	local scriptName = TE.scriptLoader:getLoadingScriptName()
	if scriptName ~= "" then
		if forthSessionModules[scriptName] then
			return forthSessionModules[scriptName]
		else
			ForthSession = {}
			forthSessionModules[scriptName] = ForthSession
		end
	else
		ForthSession = {}
	end

	--- @class dr2c.ForthSessionAttributes
	local forthSessionAttributes = {}

	--- @return dr2c.ForthSessionAttributes
	function ForthSession.getAttributes()
		return forthSessionAttributes
	end

	--- @param attributes dr2c.ForthSessionAttributes
	function ForthSession.setAttributes(attributes)
		forthSessionAttributes = attributes
	end

	--- @param attribute dr2c.ForthSessionAttribute
	--- @return any?
	function ForthSession.getAttribute(attribute)
		return forthSessionAttributes[attribute]
	end

	--- @param attribute dr2c.ForthSessionAttribute
	--- @param value any?
	function ForthSession.setAttribute(attribute, value)
		forthSessionAttributes[attribute] = value
	end

	function ForthSession.isActive()
		return forthSessionAttributes[GForthSession_Attribute_Active]
	end

	return ForthSession
end

return GForthSession
