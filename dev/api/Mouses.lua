--- @meta
error("this is a lua library module")

--- @class TE.Mouse : userdata
mouses = {}

--- @param index integer
function mouses:getMouseAt(index) end

--- @param id integer
function mouses:getMouseByID(id) end

--- @return TE.Mouse
function mouses:getPrimaryMouse() end
