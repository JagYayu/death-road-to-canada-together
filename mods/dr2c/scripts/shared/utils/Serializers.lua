local String = require("tudov.String")
local Table = require("tudov.Table")
local json = require("json")

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

return GSerializers
