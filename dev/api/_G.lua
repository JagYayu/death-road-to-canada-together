--- @meta

--- @alias Serializable boolean | number | string | table<Serializable, Serializable>

--- Insert current script's namespace with prefix at the start of `str`, e.g. N_("HelloWorld") --> "dr2c_HelloWorld"
--- @param str string
--- @return string
function N_(str) end
