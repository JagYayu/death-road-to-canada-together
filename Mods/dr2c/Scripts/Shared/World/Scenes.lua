--[[
-- @module dr2c.Shared.World.Scene
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("TE.Table")

--- @class dr2c.GWorldScene
local GWorldScenes = {}

--- @class dr2c.WorldSceneName : string

--- @class dr2c.WorldSceneIndex : integer

local worldSceneModuleData = {}

worldSceneModuleData = persist("worldSceneModules", function()
	return worldSceneModuleData
end)

--- @return dr2c.WorldScenes
function GWorldScenes.new()
	--- @class dr2c.WorldScenes
	local WorldScenes = {}

	--- @type dr2c.WorldSceneName[]
	local worldSceneList
	--- @type table<dr2c.WorldSceneName, dr2c.WorldSceneIndex> | table<dr2c.WorldSceneIndex, dr2c.WorldSceneName>
	local worldSceneMap
	local scriptName = TE.scriptLoader:getLoadingScriptName()
	if scriptName then
		if worldSceneModuleData[scriptName] then
			worldSceneList = worldSceneModuleData[scriptName][1]
			worldSceneMap = worldSceneModuleData[scriptName][2]
		else
			worldSceneList = {}
			worldSceneMap = {}
		end

		worldSceneModuleData[scriptName] = { worldSceneList, worldSceneMap }
	else
		worldSceneList = {}
		worldSceneMap = {}
	end

	--- @return dr2c.WorldSceneName[]
	function WorldScenes.getSceneList()
		return worldSceneList
	end

	--- @return table<dr2c.WorldSceneIndex, dr2c.WorldSceneName> | table<dr2c.WorldSceneName, dr2c.WorldSceneIndex>
	function WorldScenes.getSceneMap()
		return worldSceneMap
	end

	--- @param sceneNameOrIndex dr2c.WorldSceneName | dr2c.WorldSceneIndex
	--- @return (dr2c.WorldSceneIndex | dr2c.WorldSceneName)?
	function WorldScenes.invert(sceneNameOrIndex)
		return worldSceneMap[sceneNameOrIndex]
	end

	--- @param sceneNames dr2c.WorldSceneName[]
	function WorldScenes.store(sceneNames)
		worldSceneList = Table.new(#sceneNames, 0)
		worldSceneMap = Table.new(0, #worldSceneList)

		for index, sceneName in ipairs(sceneNames) do
			sceneName = tostring(sceneName)

			worldSceneList[index] = sceneName
			worldSceneMap[index] = sceneName
		end

		for index, sceneID in ipairs(worldSceneList) do
			worldSceneMap[sceneID] = index
		end
	end

	function WorldScenes.clear()
		worldSceneList = {}
		worldSceneMap = {}
	end

	return WorldScenes
end

return GWorldScenes
