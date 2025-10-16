--- @meta
error("this is a lua library module")

--- @return TE.PerlinNoiseRandom
function PerlinNoiseRandom() end

--- @param seed integer @default 1337
--- @return TE.PerlinNoiseRandom
function PerlinNoiseRandom(seed) end

--- @param seed integer @default 1337
--- @param frequency number @default: 0.01
--- @return TE.PerlinNoiseRandom
function PerlinNoiseRandom(seed, frequency) end

--- @class TE.PerlinNoiseRandom
local perlinNoiseRandom = {}

--- @return number
function perlinNoiseRandom:getFrequency() end

--- @return integer
function perlinNoiseRandom:getSeed() end

--- @param value number
function perlinNoiseRandom:setFrequency(value) end

--- @param value integer
function perlinNoiseRandom:setSeed(value) end

--- [-1, 1]
--- @param x number
--- @return number
function perlinNoiseRandom:noise1(x) end

--- [-1, 1]
--- @param x number
--- @param y number
--- @return number
function perlinNoiseRandom:noise2(x, y) end

--- [-1, 1]
--- @param x number
--- @param y number
--- @param z number
--- @return number
function perlinNoiseRandom:noise3(x, y, z) end

--- [min, max]
--- @param x number
--- @param min number
--- @param max number
--- @return number
function perlinNoiseRandom:float1(x, min, max) end

--- [min, max]
--- @param x number
--- @param y number
--- @param min number
--- @param max number
--- @return number
function perlinNoiseRandom:float2(x, y, min, max) end

--- [min, max]
--- @param x number
--- @param y number
--- @param z number
--- @param min number
--- @param max number
--- @return number
function perlinNoiseRandom:float3(x, y, z, min, max) end

--- [min, max]
--- @param x number
--- @param min integer
--- @param max integer
--- @return integer
function perlinNoiseRandom:int1(x, min, max) end

--- [min, max]
--- @param x number
--- @param y number
--- @param min integer
--- @param max integer
--- @return integer
function perlinNoiseRandom:int2(x, y, min, max) end

--- [min, max]
--- @param x number
--- @param y number
--- @param z number
--- @param min integer
--- @param max integer
--- @return integer
function perlinNoiseRandom:int3(x, y, z, min, max) end
