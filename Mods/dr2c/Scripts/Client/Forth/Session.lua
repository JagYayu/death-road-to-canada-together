--[[
-- @module dr2c.Client.Forth.Session
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")

local CClient = require("dr2c.Client.Network.Client")
local GForthCharacter = require("dr2c.Shared.Forth.Character")
local GForthGameMode = require("dr2c.Shared.Forth.GameMode")
local GMessage = require("dr2c.Shared.Network.Message")

--- @class dr2c.CForthSession
local CForthSession = {}

--- @class dr2c.ForthSessionStartNewArgs
--- @field characters? dr2c.ForthCharacter[]
--- @field gameMode? dr2c.ForthGameModeType

--- @param args dr2c.MessageContent.ForthSession
function CForthSession.startNew(args)
	local gameMode = tonumber(args.gameMode) or GForthGameMode.Type.Unknown

	local characters = args.characters
	if type(characters) ~= "table" then
		characters = {}
	end

	if not characters[1] then
		characters[1] = GForthCharacter.randomCharacter()
	end

	--- @class dr2c.MessageContent.ForthSession : dr2c.ForthSessionStartNewArgs
	--- @field characters dr2c.ForthCharacter[]
	--- @field gameMode dr2c.ForthGameModeType
	local content = {
		gameMode = gameMode,
		characters = characters,
	}

	CClient.sendReliable(GMessage.Type.ForthSessionStart, content)
end

function CForthSession.startSave(saveName)
	error("not implement yet")

	CClient.sendReliable(GMessage.Type.ForthSessionStart, {})
end

function CForthSession.finish()
	CClient.sendReliable(GMessage.Type.ForthSessionFinish)
end

return CForthSession
