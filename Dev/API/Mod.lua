--- @meta
error("this is a lua library module")

--- @enum TE.EModDistribution
EModDistribution = {
	ClientOnly = 0b01,
	ServerOnly = 0b10,
	Universal = 0b11,
}

--- @class TE.Mod
local mod = {}

--- @return TE.ModConfig
function mod:getConfig() end

--- @return integer
function mod:getHash() end

--- @return boolean
function mod:shouldScriptLoad() end

--- @class TE.ModConfig
local modConfig = {}

--- @type string
modConfig.author = nil

--- @type string
modConfig.description = nil

--- @type TE.EModDistribution
modConfig.distribution = nil

--- @type string
modConfig.name = nil

--- @type string
modConfig.namespace = nil

--- @type string
modConfig.uid = nil

--- @type TE.Version
modConfig.version = nil
