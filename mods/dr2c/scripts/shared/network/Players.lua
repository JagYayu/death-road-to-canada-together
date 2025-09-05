--[[
-- @module dr2c.shared.network.Players
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("tudov.Enum")

--- @alias dr2c.PlayerAttribute dr2c.GPlayers.Attribute

local GPlayers = {}

GPlayers.MaxPerClient = 4

GPlayers.Attribute = Enum.sequence({
	ID = 0,
	ClientID = 1,
})

local eventClientPlayersUpdateMaxPerClient = events:new("CPlayersUpdateMaxPerClient", {
	"Overrides",
	"Cache",
})

function GPlayers.updateMaxPerClient()
	local e = {
		maximum = 4,
	}
	events:invoke(eventClientPlayersUpdateMaxPerClient, e)

	GPlayers.MaxPerClient = tonumber(e.maximum)
end

return GPlayers
