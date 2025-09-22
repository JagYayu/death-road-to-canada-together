--- @meta
error("this is a lua library module")

--- @class TE.KeyboardID : integer

--- @class TE.Keyboard : userdata
local keyboard = {}

--- @return TE.KeyboardID
function keyboard:getKeyboardID() end

--- @param keyCode TE.EKeyCode
--- @param windowID TE.WindowID?
--- @return boolean
function keyboard:isKeyCodeHeld(keyCode, windowID) end

--- @param scanCode TE.EScanCode
--- @param windowID TE.WindowID?
--- @return boolean
function keyboard:isScanCodeHeld(scanCode, windowID) end

--- @return TE.EKeyCode[]
function keyboard:listHeldKeyCodes() end

--- @return TE.EScanCode[]
function keyboard:listHeldScanCodes() end
