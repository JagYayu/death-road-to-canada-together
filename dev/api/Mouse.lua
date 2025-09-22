--- @meta
error("this is a lua library module")

--- @enum TE.EMouseButton
EMouseButton = {
	Left = 1,
	Middle = 2,
	Right = 3,
	Extra1 = 4,
	Extra2 = 5,
}

--- @enum TE.EMouseButtonFlag
EMouseButtonFlag = {
	Left = 1,
	Middle = 2,
	Right = 4,
	Extra1 = 8,
	Extra2 = 16,
}

--- @class TE.Mouse : userdata
local mouse = {}

--- @return TE.Window?
function mouse:getFocusedWindow() end

--- @return integer
function mouse:getID() end

--- @return string
function mouse:getName() end

--- @return number mouseX
--- @return number mouseY
function mouse:getPosition() end

--- @return number mouseX
--- @return number mouseY
--- @return TE.EMouseButtonFlag mouseFlags
function mouse:getState() end

--- @param button TE.EMouseButton
function mouse:isButtonDown(button) end

--- @param buttonFlags TE.EMouseButtonFlag
function mouse:isButtonsDown(buttonFlags) end
