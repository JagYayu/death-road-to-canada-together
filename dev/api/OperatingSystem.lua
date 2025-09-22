--- @meta
error("this is a lua library module")

--- @enum TE.EOperatingSystem
EOperatingSystem = {
	Unknown = 0,
	Windows = 1,
	Linux = 2,
	MaxOS = 3,
	Android = 4,
	IOS = 5,
}

--- @class TE.OperatingSystem
OperatingSystem = {}

--- @return TE.EOperatingSystem
function OperatingSystem.getType() end

--- @return boolean
function OperatingSystem.isMobile() end

--- @return boolean
function OperatingSystem.isPC() end
