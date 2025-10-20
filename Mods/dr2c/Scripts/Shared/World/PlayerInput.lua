--[[
-- @module dr2c.Shared.World.PlayerInput
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")

--- @alias dr2c.PlayerInputID dr2c.GPlayerInput.ID

--- @class dr2c.PlayerInput : integer
local GPlayerInput = {}

GPlayerInput.InputListMaxSize = 8

GPlayerInput.ID = Enum.sequence({
	Move = 1,
})

GPlayerInput.Type = Enum.immutable({
	Discrete = 0,
	--- Serve as a predictive value for the client.
	Continuous = 1,
	Intent = 2,
})

local playerInputID2Type = {
	[GPlayerInput.ID.Move] = GPlayerInput.Type.Continuous,
}

local continuousIDSet = {
	[GPlayerInput.ID.Move] = true,
}

--- @param playerInputID dr2c.PlayerInputID
--- @return boolean
--- @nodiscard
function GPlayerInput.isContinuous(playerInputID)
	return not not continuousIDSet[playerInputID]
end

--- @param playerInputID dr2c.PlayerInputID
--- @return boolean
--- @nodiscard
function GPlayerInput.isDiscrete(playerInputID)
	return not continuousIDSet[playerInputID]
end

-- --- @param playerInputID dr2c.PlayerInputID
-- --- @return dr2c.PlayerInputType
-- function GPlayerInput.getType(playerInputID)
-- 	return playerInputID2Type[playerInputID] or GPlayerInput.Type.Discrete
-- end

return GPlayerInput
