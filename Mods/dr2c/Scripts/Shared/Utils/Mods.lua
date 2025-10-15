--- @class dr2c.GUtilsMods
local GUtilsMods = {}

--- @return { [1]: string, [2]: string }
function GUtilsMods.collectMods()
	local mods = {}

	--- @param value TE.Mod
	for index, value in TE.mods:getLoadedMods():pairs() do
		local config = value:getConfig()
		local version = config.version
		mods[#mods + 1] = { config.uid, tostring(version) }
	end

	return mods
end

return GUtilsMods
