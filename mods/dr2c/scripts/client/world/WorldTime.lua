local Time = Time

--- @class dr2c.CWorldTime
local CWorldTime = {}

local worldBeginTime = -1

--- @return boolean
function CWorldTime.isAvailable()
	return worldBeginTime >= 0
end

--- @return number
function CWorldTime.getTime()
	if worldBeginTime >= 0 then
		return Time.getSystemTime() - worldBeginTime
	else
		return 0
	end
end

events:add(N_("CConnect"), function(e)
	worldBeginTime = Time.getSystemTime()
end, N_("Initialize"), "Initialize")

return CWorldTime
