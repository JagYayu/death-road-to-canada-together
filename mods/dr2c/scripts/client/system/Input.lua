--[[
-- @module dr2c.client.system.Input
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Function = require("TE.Function")
local Table = require("TE.Table")

local CECS = require("dr2c.Client.ECS.ECS")

--- @class dr2c.CInput
local CInput = {}

local primaryWindowID = TE.engine.primaryWindow:getWindowID()
local primaryKeyboard = keyboards:getKeyboardAt(1)
local primaryMouse = mouses:getPrimaryMouse()

--- @type TE.EKeyCode[]
local heldKeyCodesCache = {}
--- @type TE.EScanCode[]
local heldScanCodesCache = {}

--- @type fun()
local eventHandlerCacheHeldInputCodes

if primaryKeyboard then
	--- @param keyCode TE.EKeyCode
	--- @return boolean
	function CInput.isKeyCodeHeld(keyCode)
		return primaryKeyboard:isKeyCodeHeld(keyCode, primaryWindowID)
	end

	--- @param scanCode TE.EScanCode
	--- @return boolean
	function CInput.isScanCodeHeld(scanCode)
		return primaryKeyboard:isScanCodeHeld(scanCode, primaryWindowID)
	end

	--- @return TE.EKeyCode[]
	function CInput.getHeldKeyCodes()
		return heldKeyCodesCache
	end

	--- @return TE.EScanCode[]
	function CInput.getHeldScanCodes()
		return heldScanCodesCache
	end

	function eventHandlerCacheHeldInputCodes()
		heldKeyCodesCache = primaryKeyboard:listHeldKeyCodes()
		heldScanCodesCache = primaryKeyboard:listHeldScanCodes()
	end
else
	--- @param keyCode TE.EKeyCode
	--- @return boolean
	function CInput.isKeyCodeHeld(keyCode)
		return false
	end

	--- @param scanCode TE.EScanCode
	--- @param windowKey string?
	--- @return boolean
	function CInput.isScanCodeHeld(scanCode, windowKey)
		return false
	end

	--- @return TE.EKeyCode[]
	function CInput.getHeldKeyCodes()
		return Table.empty
	end

	--- @return TE.EScanCode[]
	function CInput.getHeldScanCodes()
		return Table.empty
	end

	eventHandlerCacheHeldInputCodes = Function.empty
end

TE.events:add(N_("CUpdate"), eventHandlerCacheHeldInputCodes, "CacheHeldInputCodes", "Inputs")

--- @return number mouseX
--- @return number mouseY
function CInput.getMousePosition()
	return primaryMouse:getPosition()
end

return CInput
