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
function perlinNoiseRandom.setFrequency(value) end

--- @param value integer
function perlinNoiseRandom.setSeed(value) end

--- @param x number
--- @param y number
function perlinNoiseRandom.noise2(x, y) end

--- @param x number
--- @param y number
--- @param z number
function perlinNoiseRandom.noise3(x, y, z) end
