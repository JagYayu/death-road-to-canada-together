--[[
-- @module dr2c.Shared.Forth.Trait
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")

--- @class dr2c.GForthTrait
local GForthTrait = {}

GForthTrait.Type = Enum.protocol({
	AnimeFan = 1,
	BackToTheWall = 2,
	Bandit = 3,
	Berserk = 4,
	CalmUnderFire = 5,
	Charming = 6,
	CitySeeker = 7,
	Civilized = 8,
	DingusSavant = 9,
	FastLearner = 10,
	FierceTempered = 11,
	FranticWhiner = 12,
	Gourmand = 13,
	Grappler = 14,
	HeroType = 15,
	Inventive = 16,
	Irritating = 17,
	MysteriousPast = 18,
	Nurturing = 19,
	Oblivious = 20,
	Paranoid = 21,
	Phoenix = 22,
	Practical = 23,
	ResilientType = 24,
	SoundSleeper = 25,
	Specialist = 26,
	TinyEater = 27,
	TravelLight = 28,
	Unpredictable = 29,
	Warrior = 30,
	Fireproof = 31,
})

return GForthTrait
