--- @meta

--- @class Render
Render = {}

--- @class Render.Buffer : userdata
--- @field clear fun(self: self, )
--- @field draw fun(self: self, x: number, y: number, w: number, h: number, t: integer, tx: number, ty: number, tw: number, th: number, a: number, cx: number, cy: number, z: number)
--- @field render fun(self: self, )
--- @field setTransform fun(self: self, matrix: Matrix3x3)

--- @return Render.Buffer
function Render.newBuffer() end

return Render
