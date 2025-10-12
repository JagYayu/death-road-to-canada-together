--- @meta
error("this is a lua library module")

--- @class TE.Mouse : userdata
local mice = {}

--- @param index integer
function mice:getMouseAt(index) end

--- @param id integer
function mice:getMouseByID(id) end

--- @return TE.Mouse?
function mice:getPrimaryMouse() end

TE.mice = mice
