-- local Camera = require "dr2c.render.Camera"
-- local SpriteRenderer = {}

-- local renderer = Engine.getMainWindow().getRenderer()

-- function SpriteRenderer.test()
-- end

local renderTarget

events:add(N_("RenderGame"), function(e)
	renderTarget = renderTarget or e.window.renderer:newRenderTarget()
	renderTarget:beginTarget()

	renderTarget:endTarget()
	-- local p = print(1);
end, N_("renderSprites"), "Main")

-- return SpriteRenderer
