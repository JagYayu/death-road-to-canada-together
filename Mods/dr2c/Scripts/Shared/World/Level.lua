--[[
-- @module dr2c.Shared.World.Level
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")

local GWorldSession = require("dr2c.Shared.World.Session")

local GWorldSession_Attribute_Level = GWorldSession.Attribute.Level

--- @class dr2c.GWorldLevel
local GWorldLevel = {}

--- @alias dr2c.WorldLevelType dr2c.GWorldLevel.Type

GWorldLevel.Type = Enum.sequence({
	None = 0,
	ArcadeGym = 1,
	BigOffice = 2,
	Bigstore = 3,
	Cabin = 4,
	City = 5,
	Clinic = 6,
	Duodenum = 7,
	Eyeball = 8,
	Industrial = 9,
	Junkyard = 10,
	Mall = 11,
	Office = 12,
	Oldmega = 13,
	Police = 14,
	Restroom = 15,
	RestStop = 16,
	Shop = 17,
	Tutorial = 18,
})

--- @param worldSession dr2c.MWorldSession
--- @return dr2c.WoldLevel module
function GWorldLevel.new(worldSession)
	local worldSession_getAttribute = worldSession.getAttribute
	local worldSession_setAttribute = worldSession.setAttribute

	--- @class dr2c.WoldLevel
	local WorldLevel = {}

	--- @return dr2c.GWorldLevel.Type
	function WorldLevel.getType()
		return worldSession_getAttribute(GWorldSession_Attribute_Level)
	end

	--- @param levelType dr2c.WorldLevelType
	function WorldLevel.setType(levelType)
		return worldSession_setAttribute(GWorldSession_Attribute_Level, levelType)
	end

	return WorldLevel
end

return GWorldLevel
