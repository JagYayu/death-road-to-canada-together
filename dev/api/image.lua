--- @meta
error("this is a lua library module")

--- @class ImageManager
images = {}

--- @param imagePath string
--- @return ImageID
function images:getID(imagePath) end

--- @param imageID ImageID
--- @return Image
function images:get(imageID) end

--- @class Image
local image = {}

--- @return integer
function image.getWidth() end

--- @return integer
function image.getHeight() end

--- @return integer, integer
function image.getSize() end

--- @class ImageID : integer
