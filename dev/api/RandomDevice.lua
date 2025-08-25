--- @meta
error("this is a lua library module")

--- @class RandomDevice
RandomDevice = {}

--- @param min number
--- @param max number
--- @return number
function RandomDevice.generate(min, max) end

--- @return integer
function RandomDevice.entropy() end
