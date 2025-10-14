--- @meta
error("this is a lua library module")

--- @class TE.ModManager
local mods = {}

--- @return TE.stl.vector<TE.Mod>
function mods:getLoadedMods() end

function mods:getLoadedModsHash() end

function mods:loadMods() end

function mods:unloadMods() end

TE.mods = mods
