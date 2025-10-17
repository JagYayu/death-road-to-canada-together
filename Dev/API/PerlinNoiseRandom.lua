--- @meta
error("this is a lua library module")

--- @class TE.NoiseRandom
local noiseRandom = {}

--- @return number
function noiseRandom:getFrequency() end

--- @return integer
function noiseRandom:getSeed() end

--- @param value number
function noiseRandom:setFrequency(value) end

--- @param value integer
function noiseRandom:setSeed(value) end

--- [-1, 1]
--- @param x number
--- @return number
function noiseRandom:noise1(x) end

--- [-1, 1]
--- @param x number
--- @param y number
--- @return number
function noiseRandom:noise2(x, y) end

--- [-1, 1]
--- @param x number
--- @param y number
--- @param z number
--- @return number
function noiseRandom:noise3(x, y, z) end

--- [min, max]
--- @param x number
--- @param min number
--- @param max number
--- @return number
function noiseRandom:float1(x, min, max) end

--- [min, max]
--- @param x number
--- @param y number
--- @param min number
--- @param max number
--- @return number
function noiseRandom:float2(x, y, min, max) end

--- [min, max]
--- @param x number
--- @param y number
--- @param z number
--- @param min number
--- @param max number
--- @return number
function noiseRandom:float3(x, y, z, min, max) end

--- [min, max]
--- @param x number
--- @param min integer
--- @param max integer
--- @return integer
function noiseRandom:int1(x, min, max) end

--- [min, max]
--- @param x number
--- @param y number
--- @param min integer
--- @param max integer
--- @return integer
function noiseRandom:int2(x, y, min, max) end

--- [min, max]
--- @param x number
--- @param y number
--- @param z number
--- @param min integer
--- @param max integer
--- @return integer
function noiseRandom:int3(x, y, z, min, max) end

--- @class TE.PerlinNoiseRandom : TE.NoiseRandom

--- @return TE.PerlinNoiseRandom
function PerlinNoiseRandom() end

--- @param seed integer @default 1337
--- @return TE.PerlinNoiseRandom
function PerlinNoiseRandom(seed) end

--- @param seed integer @default 1337
--- @param frequency number @default: 0.01
--- @return TE.PerlinNoiseRandom
function PerlinNoiseRandom(seed, frequency) end
