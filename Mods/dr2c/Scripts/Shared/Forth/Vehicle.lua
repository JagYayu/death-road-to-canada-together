--[[
-- @module dr2c.Shared.Forth.Vehicle
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")

--- @class dr2c.GForthVehicle
local GForthVehicle = {}

GForthVehicle.Type = Enum.protocol({
	OldCar = 1,
})

GForthVehicle.Info = Enum.protocol({
	Speed = 1,
	Durability = 2,
	Chassis = 3,
	Armour = 4,
	Engine = 5,
	GasCost = 6,
	RepairLevel = 7,
})

local vehicleFallback = {}

function GForthVehicle.registerFallback() end

return GForthVehicle
