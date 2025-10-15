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
local GForthSession = require("dr2c.Shared.Forth.Session")
local GForthCharacter = require("dr2c.Shared.Forth.Character")
local GForthGameMode = require("dr2c.Shared.Forth.GameMode")
local GNetworkMessage = require("dr2c.Shared.Network.Message")

--- @class dr2c.CForthSession : dr2c.ForthSession
local CForthSession = GForthSession.new()

--- @class dr2c.ForthSessionStartNewArgs
--- @field characters? dr2c.ForthCharacter[]
--- @field gameMode? dr2c.ForthGameModeType

--- @param args dr2c.ForthSessionStartNewArgs
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

	CClient.sendReliable(GNetworkMessage.Type.ForthSessionStart, content)
end

function CForthSession.startSave(saveName)
	error("not implement yet")

	CClient.sendReliable(GNetworkMessage.Type.ForthSessionStart, {})
end

function CForthSession.finish()
	CClient.sendReliable(GNetworkMessage.Type.ForthSessionFinish, {})
end

--- @param attributes table
function CForthSession.startLocally(attributes)
	CForthSession.setAttributes(attributes)
end

function CForthSession.finishLocally()
	--
end

TE.events:add(N_("CMessage"), function(e)
	if type(e.content) == "table" then
		if not e.suppressed then
			log.info(("Client %s started forth session"):format(e.content.sponsorClientID))

			CForthSession.startLocally(e.content.attributes)
		elseif e.content.sponsorClientID == CClient.getClientID() and log.canWarn() then
			log.warn(("Cannot start forth session: %s"):format(e.suppressed))
		end
	end
end, "StartForthSessionLocally", "Receive", GNetworkMessage.Type.ForthSessionStart)

TE.events:add(N_("CMessage"), function(e)
	if type(e.content) == "table" then
		if not e.suppressed then
			log.info(("Client %s finished forth session"):format(e.content.sponsorClientID))

			CForthSession.finishLocally()
		elseif e.content.sponsorClientID == CClient.getClientID() and log.canWarn() then
			log.warn(("Cannot start forth session: %s"):format(e.suppressed))
		end
	end
end, "FinishForthSessionLocally", "Receive", GNetworkMessage.Type.ForthSessionFinish)

return CForthSession
