--- @meta
error("this is a lua library module")

--- @class TE.Keyboards : userdata
TE.keyboards = {}

--- @param index integer
--- @return TE.Keyboard?
function TE.keyboards:getKeyboardAt(index) end

--- @param id TE.KeyboardID
--- @return TE.Keyboard?
function TE.keyboards:getKeyboardByID(id) end

--- @return TE.Keyboard[]
--- @return TE.Keyboard[]
function TE.keyboards:listKeyboards() end
