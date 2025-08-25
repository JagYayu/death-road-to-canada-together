local Number = require("tudov.Number")

--- @class Utility
local Utility = {}

local indexNan = Number.nan

function Utility.isIndexKey(value)
	return value ~= nil and value ~= indexNan
end

return Utility
