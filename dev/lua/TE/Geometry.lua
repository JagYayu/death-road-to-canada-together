local tonumber = tonumber

--- @class TE.Geometry
local Geometry = {}

function Geometry.copyCppRect()
	-- TODO
end

--- @param rect Rectangle | table
--- @param x number?
--- @param y number?
--- @param w number?
--- @param h number?
--- @return Rectangle
function Geometry.copyLuaRect(rect, x, y, w, h)
	return {
		tonumber(rect[1]) or x or 0,
		tonumber(rect[2]) or y or 0,
		tonumber(rect[3]) or w or 0,
		tonumber(rect[4]) or h or 0,
	}
end

--- @param rect Rectangle | table
--- @param x number
--- @param y number
--- @return boolean
function Geometry.luaRectContains(rect, x, y)
	return x > rect[1] and y > rect[2] and x < rect[1] + rect[3] and y < rect[2] + rect[4]
end

return Geometry
