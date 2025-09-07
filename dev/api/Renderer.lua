--- @meta
error("this is a lua library module")

--- @class DrawTextArgs : userdata
--- @field text string?
--- @field font number?
--- @field characterScale number?
--- @field x number?
--- @field y number?
--- @field scale number?
--- @field alignX number?
--- @field alignY number?
--- @field maxWidth number?

--- @return DrawTextArgs
function DrawTextArgs() end

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

--- @param args DrawTextArgs
function renderer:drawText(args) end
