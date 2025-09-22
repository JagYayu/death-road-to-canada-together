--- @meta
error("this is a lua library module")

--- @class TE.RenderTarget : userdata
local renderTarget = {}

--- @return boolean resized
function renderTarget:resizeToFit() end

--- @param scaleX number
--- @param scaleY number
function renderTarget:setCameraTargetScale(scaleX, scaleY) end
