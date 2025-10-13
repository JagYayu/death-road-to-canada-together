--[[
-- @module dr2c.Shared.Forth.Perk
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require "TE.Enum"

local GForthPerk = {}

GForthPerk.Type = Enum.protocol({
	Athlete = 1,
	BigBruiser = 2,
	BowAndArrows = 3,
	CarNut = 4,
	Chairmaster = 5,
	ExWrestler = 6,
	Explorer = 7,
	Fighter = 8,
	FierceHitter = 9,
	FriendOfDog = 10,
	GunCollector = 11,
	Gungineer = 12,
	HealthCare = 13,
	HiddenPotential = 14,
	JackOfAllTrades = 15,
	Kensai = 16,
	MartialArtist = 17,
	Mechanic = 18,
	Megabuff = 19,
	NaturalShot = 20,
	Pathfinder = 21,
	Pyromaniac = 22,
	Ranger = 23,
	ShieldOfHope = 24,
	Supporter = 25,
	Surgeon = 26,
	Tstc = 27,
	TopSeller = 28,
	TrademarkWeapon = 29,
	Ultrafit = 30,
})

return GForthPerk
