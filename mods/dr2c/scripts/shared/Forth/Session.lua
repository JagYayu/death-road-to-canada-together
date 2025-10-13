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

--- @class dr2c.GForthSession
local GForthSession = {}

--- @alias dr2c.ForthSessionAttribute dr2c.GForthSession.Attribute

GForthSession.Attribute = Enum.protocol({
	Version = 1,
	Mods = 2,
	GameMode = 3,
	CreateTime = 4,
	LastSaveTime = 5,
	Playtime = 6,
	Characters = 7,
	Vehicle = 8,
	Items = 9,
})

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

	return ForthSession
end

return GForthSession
