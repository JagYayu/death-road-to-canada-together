--- @meta
error("this is a lua library module")

--- @class TE.Mouse : userdata
TE.mice = {}

--- @param index integer
function TE.mice:getMouseAt(index) end

--- @param id integer
function TE.mice:getMouseByID(id) end

--- @return TE.Mouse?
function TE.mice:getPrimaryMouse() end
