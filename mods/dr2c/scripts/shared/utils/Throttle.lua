local GCycle = require("dr2c.shared.misc.Cycle")

local getSystemTime = Time.getSystemTime
local getTick = GCycle.getTick

--- @class dr2c.ThrottleData
--- @field [0] "call" | "tick" | "time"
--- @field [1] boolean
--- @field [2] integer
--- @field [3] (integer | boolean)?
--- @field [4] number?

--- @class dr2c.Throttle : dr2c.ThrottleData
--- @overload fun(): boolean

--- @class dr2c.GThrottle
local GThrottle = {}

--- @type table<dr2c.Throttle, dr2c.Throttle>
local instances = setmetatable({}, {
	__mode = "kv",
})

local function checkParamInterval(value, mustBeInteger)
	if type(value) ~= "number" then
		error("Bad argument to #1 'interval', number expected", 3)
	elseif value <= 0 then
		error("Bad argument to #1 'interval': must be greater than 0", 3)
	elseif mustBeInteger and value ~= math.floor(value) then
		error("Bad argument to #1 'interval': must be an integer", 3)
	end
end

local metatableCall = {
	__call = function(t)
		t[3] = (t[3] % t[2]) + 1
		if t[1] then
			return t[3] ~= t[2]
		else
			return t[3] == t[2]
		end
	end,
}

--- @param interval integer
--- @param invert boolean?
--- @return dr2c.Throttle
function GThrottle.newCall(interval, invert)
	checkParamInterval(interval, true)

	local proxy = setmetatable({
		[0] = "tick",
		not not invert,
		interval,
		0,
	}, metatableCall)

	instances[proxy] = proxy

	return proxy
end

local metatableTick = {
	__call = function(t)
		local v = getTick() % t[2]
		if t[1] then
			return v == 0
		else
			return v ~= 0
		end
	end,
}

--- @param interval integer
--- @param invert boolean?
--- @return dr2c.Throttle
function GThrottle.newTick(interval, invert)
	checkParamInterval(interval, true)

	local proxy = setmetatable({
		[0] = "tick",
		not not invert,
		interval,
	}, metatableTick)

	instances[proxy] = proxy

	return proxy
end

local metatableTime = {
	__call = function(t)
		local st = getSystemTime()
		local v = (st - t[4]) > t[2]
		if not t[1] then
			v = not v
		end

		if not v then
			if t[3] then
				t[4] = t[4] + t[2]
			else
				t[4] = st
			end
		end

		return v
	end,
}

--- @param interval number
--- @param invert boolean?
--- @param lazy boolean? @default: true
--- @return dr2c.Throttle
function GThrottle.newTime(interval, invert, lazy)
	checkParamInterval(interval, false)

	local proxy = setmetatable({
		[0] = "time",
		not not invert,
		interval,
		lazy ~= false,
		getSystemTime(),
	}, metatableTime)

	instances[proxy] = proxy

	return proxy
end

function GThrottle.getAll()
	return instances
end

events:add("DebugSnapshot", function(e)
	e.instances = instances
end, nil, nil, scriptName)

return GThrottle
