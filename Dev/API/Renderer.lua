--- @meta
error("this is a lua library module")

--- @class TE.Texture

--- @alias DrawArgFont Font | FontID

--- @alias DrawArgTexture TE.Image | TE.ImageID | TE.RenderTarget

--- @class TE.DrawRectArgs : userdata
--- @field texture DrawArgTexture?
--- @field destination TE.RectangleF
--- @field source TE.RectangleF?
--- @field color Color
--- @field angle number
--- @field origin Vector

--- @class TE.DrawTextArgs : userdata
--- @field text string
--- @field font DrawArgFont
--- @field characterSize number
--- @field x number
--- @field y number
--- @field scale number
--- @field alignX number
--- @field alignY number
--- @field maxWidth number
--- @field color Color
--- @field backgroundColor Color
--- @field shadow number
--- @field shadowColor Color

--- @return TE.DrawRectArgs
function DrawRectArgs() end

--- @return TE.DrawTextArgs
function DrawTextArgs() end

--- @class TE.Renderer : userdata
local renderer = {}

--- @param renderTarget TE.RenderTarget
function renderer:beginTarget(renderTarget) end

--- @return TE.RenderTarget
function renderer:endTarget() end

function renderer:clear() end

--- @return TE.RenderBuffer
function renderer:newRenderBuffer() end

--- @param width integer
--- @param height integer
--- @return TE.RenderTarget
function renderer:newRenderTarget(width, height) end

--- @param args TE.DrawRectArgs
function renderer:drawRect(args) end

--- @param x number
--- @param y number
--- @param text string
function renderer:drawDebugText(x, y, text) end

--- @param args TE.DrawTextArgs
--- @return number x
--- @return number y
--- @return number w
--- @return number h
function renderer:drawText(args) end
