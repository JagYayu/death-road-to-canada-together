--- @meta
error("this is a lua library module")

--- @class Fonts
fonts = {}

--- @param imagePath string
--- @return FontID
function fonts:getID(imagePath) end

--- @param imageID FontID
--- @return Font
function fonts:get(imageID) end

--- @class Font
local font = {}

--- @class FontID : integer
