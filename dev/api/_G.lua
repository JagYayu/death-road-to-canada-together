--- @meta
error("this is a lua library module")

--- @alias Serializable boolean | number | string | table<Serializable, Serializable>

--- @class FRect
--- @field [1] number
--- @field [2] number
--- @field [3] number
--- @field [4] number

--- @class ImageID : integer

--- Insert prefix of current script's namespace at the start of `str`.
--- e.g. `N_("HelloWorld")` --> `"dr2c_HelloWorld"`
--- @param str string
--- @return string
function N_(str) end
