--- @meta
error("this is a lua library module")

--- @class TE.ImageID : integer

--- @class TE.ImageManager
images = {}

--- @param imagePath string
--- @return TE.ImageID
function images:getID(imagePath) end

--- @param imageID TE.ImageID
--- @return TE.Image
function images:get(imageID) end

--- @class TE.Image
local image = {}

--- @return integer width
function image.getWidth() end

--- @return integer height
function image.getHeight() end

--- @return integer width
--- @return integer height
function image.getSize() end
