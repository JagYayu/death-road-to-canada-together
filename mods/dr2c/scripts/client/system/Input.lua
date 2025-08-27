local CECS = require("dr2c.client.ecs.ECS")
local Table = require("tudov.Table")

--- @class dr2c.CInput
local CInput = {}

local defaultWindowKey = "MainWindow"

--- @type table<string, { list: EKeyCode[], set: table<EKeyCode, integer> }>
local windowKey2HoldingKeyCodes = {}

--- @type table<string, { list: EScanCode[], set: table<EScanCode, integer> }>
local windowKey2HoldingScanCodes = {}

--- @param keyCode EScanCode
--- @param windowKey string?
--- @return boolean
function CInput.isKeyCodeHeld(keyCode, windowKey)
	local holdings = windowKey2HoldingKeyCodes[windowKey or defaultWindowKey]
	return not not (holdings and holdings.set[keyCode])
end

--- @param scanCode EScanCode
--- @param windowKey string?
--- @return boolean
function CInput.isScanCodeHeld(scanCode, windowKey)
	local holdings = windowKey2HoldingScanCodes[windowKey or defaultWindowKey]
	return not not (holdings and holdings.set[scanCode])
end

local function getHeldCodes(windowKey2HoldingCodes, windowKey)
	local holdings = windowKey2HoldingKeyCodes[windowKey or defaultWindowKey]
	return holdings and holdings.list or Table.empty
end

--- @param windowKey string?
--- @return EKeyCode[]
function CInput.getHeldKeyCodes(windowKey)
	return getHeldCodes(windowKey2HoldingKeyCodes, windowKey)
end

--- @param windowKey string?
--- @return EScanCode[]
function CInput.getHeldScanCodes(windowKey)
	return getHeldCodes(windowKey2HoldingScanCodes, windowKey)
end

--- @generic T
--- @param window Window
--- @param windowKey2HoldingCodes table<string, { list: T[], set: table<T, integer> }>
--- @param code T
--- @param whatCode string
local function addHoldingCode(window, windowKey2HoldingCodes, code, whatCode)
	local holdingCodes = windowKey2HoldingCodes[window:getKey()]
	if not holdingCodes then
		holdingCodes = {
			list = {},
			set = {},
		}
		windowKey2HoldingCodes[window:getKey()] = holdingCodes
	end

	if not holdingCodes[code] then
		holdingCodes.list[#holdingCodes + 1] = code
		holdingCodes.set[code] = #holdingCodes
	elseif log.canWarn() then
		log.warn(("%s code %s is already down"):format(whatCode, code))
	end
end

--- @param e Events.E.KeyDown
events:add(N_("CKeyDown"), function(e)
	addHoldingCode(e.window, windowKey2HoldingKeyCodes, e.keyCode, "Key")
	addHoldingCode(e.window, windowKey2HoldingScanCodes, e.scanCode, "Scan")
end, "AddHoldingCodes", "Hold")

--- @generic T
--- @param window Window
--- @param windowKey2HoldingCodes table<string, { list: T[], set: table<T, integer> }>
--- @param code T
--- @param whatCode string
local function removeHoldingCode(window, windowKey2HoldingCodes, code, whatCode)
	local holdingCodes = windowKey2HoldingCodes[window:getKey()]
	if (not holdingCodes or not holdingCodes.set[code]) and log.canWarn() then
		log.warn(("%s code %s is not held"):format(whatCode, code))

		return
	end

	Table.listRemoveFirst(holdingCodes.list, code)
	holdingCodes.set[code] = nil
	if next(holdingCodes) == nil then
		windowKey2HoldingCodes[window:getKey()] = nil
	end
end

--- @param e Events.E.KeyUp
events:add(N_("CKeyUp"), function(e)
	removeHoldingCode(e.window, windowKey2HoldingKeyCodes, e.keyCode, "Key")
	removeHoldingCode(e.window, windowKey2HoldingScanCodes, e.scanCode, "Scan")
end)

-- local dx = 0
-- local dy = 0

-- if e.scanCode == EScanCode.A then
-- 	dx = dx - 1
-- end
-- if e.scanCode == EScanCode.D then
-- 	dx = dx + 1
-- end
-- if e.scanCode == EScanCode.W then
-- 	dy = dy - 1
-- end
-- if e.scanCode == EScanCode.S then
-- 	dy = dy + 1
-- end

return CInput
