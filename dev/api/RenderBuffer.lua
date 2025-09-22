--- @meta
error("this is a lua library module")

--- @class TE.RenderBuffer.AddRectangleArgs : userdata
--- @field texture DrawArgTexture?
--- @field destination Rectangle
--- @field source Rectangle?
--- @field color Color?

--- @class TE.RenderBuffer : userdata
local renderBuffer = {}

--- @param args TE.RenderBuffer.AddRectangleArgs
function renderBuffer:addRectangle(args) end

function renderBuffer:addVertices() end

function renderBuffer:clear() end

function renderBuffer:draw() end
