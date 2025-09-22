--[[
-- @module dr2c.shared.utils.Serializers
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local String = require("tudov.String")
local Table = require("tudov.Table")
local json = require("lib.json")

--- @class dr2c.Serializer
--- @field serialize fun(obj: any): string
--- @field deserialize fun(obj: string): any

--- @class dr2c.GSerializers
local GSerializers = {}

--- @type dr2c.Serializer
GSerializers.JSON = Table.readonly({
	serialize = json.encode,
	deserialize = json.decode,
})

--- @type dr2c.Serializer
GSerializers.LJBuffer = Table.readonly({
	serialize = String.bufferEncode,
	deserialize = String.bufferDecode,
})

local function serializeLuaTable(tbl)
	--
end

local function deserializeLuaTable(data)
	--
end

GSerializers.LuaTable = Table.readonly({
	serialize = serializeLuaTable,
	deserialize = deserializeLuaTable,
})

return GSerializers
