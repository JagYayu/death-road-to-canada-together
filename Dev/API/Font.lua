--- @meta
error("this is a lua library module")

--- @class Fonts
TE.fonts = {}

--- @param imagePath string
--- @return FontID
function TE.fonts:getID(imagePath) end

--- @param imageID FontID
--- @return Font
function TE.fonts:get(imageID) end

--- @class Font
local font = {}

--- @class FontID : integer
