--[[
-- @module dr2c.Client.Render.SpriteRenderer
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

-- local Camera = require "dr2c.render.Camera"
-- local SpriteRenderer = {}

-- local renderer = Engine.getPrimaryWindow().getRenderer()

-- function SpriteRenderer.test()
-- end

local renderTarget
local drawOnce = true

TE.events:add(N_("CRender"), function(e)
	--
end, N_("renderSprites"))

-- return SpriteRenderer
