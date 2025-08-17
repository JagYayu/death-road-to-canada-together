--- @meta
error("this is a lua library module")

--- @alias N_Hint string

--- @alias NotNil number | string | boolean | table | function | thread | userdata

--- @alias Serializable boolean | number | string | table<Serializable, Serializable>

--- @class FRect
--- @field [1] number
--- @field [2] number
--- @field [3] number
--- @field [4] number

--- @class ImageID : integer

--- *Mod Scope*
--- Insert prefix of current script's namespace at the start of `str`.
--- e.g. `N_("HelloWorld")` --> `"dr2c_HelloWorld"`
--- @param str N_Hint
--- @return string
function N_(str) end

--- *Mod Scope*
--- @generic T : NotNil
--- @param key string
--- @param defaultValue T
--- @param getter fun(): T
--- @return T
function persist(key, defaultValue, getter) end

--- *Mod Scope*
--- You can also assess this value via `modConfig`.
--- @type string
modNamespace = nil

--- *Mod Scope*
--- You can also assess this value via `modConfig`.
--- @type boolean
modSandboxed = nil

--- *Mod Scope*
--- You can also assess this value via `modConfig`.
--- @type string
modUID = nil

--- *Script Scope*
--- Current script's internal id.
--- e.g. 32
--- @type integer
scriptID = nil

--- *Script Scope*
--- Current script's name.
--- e.g. "dr2c.client.test.Test"
--- @type string
scriptName = nil
