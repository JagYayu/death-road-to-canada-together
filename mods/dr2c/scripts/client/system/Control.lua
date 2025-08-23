--- @class dr2c.CControl
local CControl = {}

--- @param e Events.E.KeyDown
events:add("KeyDown", function(e)
	local dx = 0
	local dy = 0

	if e.scanCode == EScanCode.A then
		dx = dx - 1
	end
	if e.scanCode == EScanCode.D then
		dx = dx + 1
	end
	if e.scanCode == EScanCode.W then
		dy = dy - 1
	end
	if e.scanCode == EScanCode.S then
		dy = dy + 1
	end
end)

return CControl
