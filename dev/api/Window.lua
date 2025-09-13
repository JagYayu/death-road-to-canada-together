--- @meta
error("this is a lua library module")

--- @class WindowID : integer

--- @class Window : userdata
local window = {}

--- @type Renderer
window.renderer = {}

--- @return string
function window:getKey() end

--- @return integer width
--- @return integer height
function window:getSize() end
