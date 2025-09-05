--[[
-- @module tudov.Color
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class Color
local Color = {}

--- @param r UInt8
--- @param g UInt8
--- @param b UInt8
function Color.rgb(r, g, b) end

--- @param r UInt8
--- @param g UInt8
--- @param b UInt8
--- @param a UInt8
function Color.rgba(r, g, b, a) end

function Color.rgbF(r, g, b) end

function Color.rgbaF(r, g, b, a) end

function Color.hsv() end

function Color.hsva() end

function Color.getR(c) end

function Color.getG(c) end

function Color.getB(c) end

function Color.getA(c) end

function Color.getH(c) end

function Color.getS(c) end

function Color.getV(c) end

function Color.setR(c) end

function Color.setG(c) end

function Color.setB(c) end

function Color.setA(c) end

function Color.setH(c) end

function Color.setS(c) end

function Color.setV(c) end

Color.White = Color.rgb(255, 255, 255)
Color.Transparent = Color.rgba(0, 0, 0, 0)
Color.Red = Color.rgb(255, 0, 0)
Color.Green = Color.rgb(0, 255, 0)
Color.Blue = Color.rgb(0, 0, 255)
Color.Black = Color.rgb(0, 0, 0)

return Color
