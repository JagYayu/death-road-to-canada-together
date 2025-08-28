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
		return Time:getSystemTime() - worldBeginTime
	else
		return 0
	end
end

return CWorldTime
