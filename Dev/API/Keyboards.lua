--- @meta
error("this is a lua library module")

--- @class TE.Keyboards : userdata
local keyboards = {}

--- @param index integer
--- @return TE.Keyboard?
function keyboards:getKeyboardAt(index) end

--- @param id TE.KeyboardID
--- @return TE.Keyboard?
function keyboards:getKeyboardByID(id) end

--- @return TE.Keyboard[]
--- @return TE.Keyboard[]
function keyboards:listKeyboards() end

TE.keyboards = keyboards
