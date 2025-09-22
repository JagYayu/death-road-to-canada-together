--- @meta
error("this is a lua library module")

--- @class TE.WindowID : integer

--- @class TE.Window : userdata
local window = {}

--- @type TE.Renderer
window.renderer = {}

function window:close() end

--- @return integer height
function window:getHeight() end

--- @return string
function window:getKey() end

--- @return integer width
--- @return integer height
function window:getSize() end

--- @return integer width
function window:getWidth() end

--- @return TE.WindowID
function window:getWindowID() end

--- @return boolean
function window:shouldClose() end
