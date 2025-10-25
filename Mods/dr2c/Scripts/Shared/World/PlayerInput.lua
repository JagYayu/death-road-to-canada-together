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

--- @alias dr2c.PlayerInputID dr2c.GWorldPlayerInput.ID

--- @class dr2c.PlayerInput : integer
local GWorldPlayerInput = {}

GWorldPlayerInput.InputListMaxSize = 8

GWorldPlayerInput.ID = Enum.sequence({
	Move = 1,
})

local continuousIDSet = {
	[GWorldPlayerInput.ID.Move] = true,
}

--- @param playerInputID dr2c.PlayerInputID
--- @return boolean
--- @nodiscard
function GWorldPlayerInput.isContinuous(playerInputID)
	return not not continuousIDSet[playerInputID]
end

--- @param playerInputID dr2c.PlayerInputID
--- @return boolean
--- @nodiscard
function GWorldPlayerInput.isDiscrete(playerInputID)
	return not continuousIDSet[playerInputID]
end

return GWorldPlayerInput
