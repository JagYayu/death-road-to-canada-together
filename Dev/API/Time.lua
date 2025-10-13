--- @meta
error("this is a lua library module")

--- @class TE.Time
Time = {}

--- Get startup system time in seconds since epoch.
--- @return number seconds
function Time.getStartupTime() end

--- Get system time in seconds since epoch.
--- @return number seconds
function Time.getSystemTime() end

--- @class Timer : function
local Timer = {}

--- @param paused boolean?
--- @return Timer
function Timer(paused) end

--- Get time in seconds.
--- @return number seconds
function Timer:getTime() end

function Timer:pause() end

function Timer:reset() end

function Timer:unpause() end
