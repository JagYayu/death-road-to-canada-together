--- @meta
error("this is a lua library module")

--- @class Texture

--- @alias DrawArgTexture Image | ImageID | RenderTarget

--- @class DrawRectArgs : userdata
--- @field texture DrawArgTexture?
--- @field destination RectangleF
--- @field source RectangleF?
--- @field color Color?
--- @field angle number?
--- @field origin Vector?

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

--- @return DrawRectArgs
function DrawRectArgs() end

--- @return DrawTextArgs
function DrawTextArgs() end

--- @class Renderer : userdata
local renderer = {}

--- @param renderTarget RenderTarget
function renderer:beginTarget(renderTarget) end

--- @return RenderTarget
function renderer:endTarget() end

function renderer:clear() end

--- @return RenderBuffer
function renderer:newRenderBuffer() end

--- @param width integer
--- @param height integer
--- @return RenderTarget
function renderer:newRenderTarget(width, height) end

--- @param args DrawRectArgs
function renderer:drawRect(args) end

--- @param x number
--- @param y number
--- @param text string
function renderer:drawDebugText(x, y, text) end

--- @param args DrawTextArgs
--- @return number x
--- @return number y
--- @return number w
--- @return number h
function renderer:drawText(args) end
