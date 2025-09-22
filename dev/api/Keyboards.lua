--- @meta
error("this is a lua library module")

--- @class TE.Keyboards : userdata
keyboards = {}

--- @param index integer
--- @return TE.Keyboard?
function keyboards:getKeyboardAt(index) end

--- @param id KeyboardID
--- @return TE.Keyboard?
function keyboards:getKeyboardByID(id) end

--- @return Keyboard[]
--- @return TE.Keyboard[]
function keyboards:listKeyboards() end
