--- @meta
error("this is a lua library module")

--- @class RenderTarget : userdata
local renderTarget = {}

-- function renderTarget.() end

--- @class Renderer : userdata
local renderer = {}

--- @param renderTarget RenderTarget
function renderer:beginTarget(renderTarget) end

--- @return RenderTarget
function renderer:endTarget() end

function renderer:clear() end

--- @param width integer
--- @param height integer
--- @return RenderTarget
function renderer:newRenderTarget(width, height) end

--- @param args table
function renderer:draw(args) end

--- @param x number
--- @param y number
--- @param text string
function renderer:drawDebugText(x, y, text) end
