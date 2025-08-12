--- @meta
error("this is a lua library module")

--- @class Tudov.ImageManager
images = {}

--- @param imagePath string
--- @return ImageID
function images:getID(imagePath) end

--- @param imageID ImageID
--- @return Image
function images:get(imageID) end
