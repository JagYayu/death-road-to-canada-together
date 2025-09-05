--[[
-- @module dr2c.client.render.SpriteRenderer
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

-- local renderer = Engine.getMainWindow().getRenderer()

-- function SpriteRenderer.test()
-- end

local renderTarget
local drawOnce = true

events:add(N_("CRender"), function(e)
	--
end, N_("renderSprites"))

-- return SpriteRenderer
