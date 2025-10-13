--- @meta
error("this is a lua library module")

--- @class TE.Array : userdata

--- @generic T
--- @class TE.Array : { [integer]: T }
local Array = {}

--- @generic T
--- @param index integer
--- @return T
function Array.get(index) end

--- @generic T
--- @param index integer
--- @param value T
function Array.set(index, value) end

function Array.encode() end

function Array.decode(data) end

--- @return TE.Array<boolean>
function ArrayBit() end

--- @return TE.Array<Int8>
function ArrayI8() end

--- @return TE.Array<Int16>
function ArrayI16() end

--- @return TE.Array<Int32>
function ArrayI32() end

--- @return TE.Array<Int64>
function ArrayI64() end

--- @return TE.Array<UInt8>
function ArrayU8() end

--- @return TE.Array<UInt16>
function ArrayU16() end

--- @return TE.Array<UInt16>
function ArrayU32() end

--- @return TE.Array<UInt32>
function ArrayU64() end

--- @return TE.Array<Float>
function ArrayF32() end

--- @return TE.Array<Double>
function ArrayF64() end
