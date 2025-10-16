--[[
-- @module dr2c.Server.World.Scene
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local GWorldScenes = require("dr2c.Shared.World.Scenes")
local GWorldSession = require("dr2c.Shared.World.Session")

--- @class dr2c.SWorldScenes : dr2c.WorldScenes
local SWorldScenes = GWorldScenes.new()

TE.events:add(N_("SWorldSessionStart"), function(e)
	SWorldScenes.store(e.attributes[GWorldSession.Attribute.Scenes])
end, "StoreScenes", "Scene")

TE.events:add(N_("SWorldSessionFinish"), function(e)
	SWorldScenes.clear()
end, "ClearScenes", "Scene")

return SWorldScenes
