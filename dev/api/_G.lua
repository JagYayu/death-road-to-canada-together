--- @meta
error("this is a lua library module")

--- @class metatable
--- @field _TE_locked true? @A marker field, means this metatable is locked by engine and inaccessible to sandboxed scripts.

--- @diagnostic disable-next-line: duplicate-doc-alias
--- @alias N_Hint string

--- @alias NotNil number | string | boolean | table | function | thread | userdata

--- @alias Serializable boolean | number | string | table<Serializable, Serializable>

--- @alias UInt8 integer

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
--- @nodiscard
function N_(str) end

--- *Mod Scope*
--- Import from engine
--- Register a persistent value. Lifespan is the whole application, which even longer than global variables.
--- @generic T : NotNil
--- @param key string
--- @param defaultValue T
--- @param getter fun(): T
--- @return T
--- @nodiscard
function persist(key, defaultValue, getter) end

--- @see persist
--- @generic T : NotNil
--- @param key string
--- @param getter fun(): T
--- @return T
--- @nodiscard
function persist(key, getter) end

--- *Mod Scope*
--- Import from lua
--- You can also assess this value via `modConfig`.
--- e.g. "dr2c"
--- @type string
modNamespace = nil

--- *Mod Scope*
--- Import from lua
--- You can also assess this value via `modConfig`.
--- @type boolean
modSandboxed = nil

--- *Mod Scope*
--- Import from lua
--- You can also assess this value via `modConfig`.
--- @type string
modUID = nil

--- *Script Scope*
--- Import from lua
--- Current script's internal id.
--- e.g. 32
--- @type integer
scriptID = nil

--- *Script Scope*
--- Import from lua
--- Current script's name.
--- e.g. "dr2c.client.test.Test"
--- @type string
scriptName = nil
