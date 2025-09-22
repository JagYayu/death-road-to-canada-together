--- @meta
error("this is a lua library module")

--- @enum TE.VFS.PathListOption
EPathListOption = {
	All = 255,
	Directory = 2,
	File = 1,
	FullPath = 16,
	None = 0,
	Recursed = 4,
	Sorted = 8,
}

--- @class TE.VFS.ListEntry
--- @field isDirectory boolean
--- @field path string

--- @class TE.VFS
vfs = {}

--- @param path string
--- @return boolean exists
function vfs:exists(path) end

--- @param directory string?
--- @param options TE.VFS.PathListOption
--- @return TE.VFS.ListEntry[] paths
function vfs:list(directory, options) end

--- @param file string?
--- @return string data
function vfs:readFile(file) end
