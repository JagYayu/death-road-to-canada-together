--- @meta

--- @class Renderer
local Renderer = {}

--- @class Renderer : userdata
--- @field clear fun(self: self)
--- @field draw fun(self: self, x: number, y: number, w: number, h: number, t: integer, tx: number, ty: number, tw: number, th: number, col: integer, a: number, cx: number, cy: number, z: number)
--- @field drawText fun(self: self, text: string, fonts: integer[], fontSize: integer, x: number, y: number, col: integer, scale: number, alignX: number, alignY: number, wrapWidth: number, shadow: number)
--- @field render fun(self: self)
--- @field setTransform fun(self: self, matrix: Matrix3x3)

return Renderer
