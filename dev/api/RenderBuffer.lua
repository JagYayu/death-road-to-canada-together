--- @meta
error("this is a lua library module")

--- @class RenderBuffer.AddRectangleArgs : userdata
--- @field texture TextureArg?
--- @field destination Rectangle
--- @field source Rectangle?
--- @field color Color?

--- @class RenderBuffer : userdata
local renderBuffer = {}

--- @param args RenderBuffer.AddRectangleArgs
function renderBuffer:addRectangle(args) end

function renderBuffer:addVertices() end

function renderBuffer:clear() end

function renderBuffer:draw() end
