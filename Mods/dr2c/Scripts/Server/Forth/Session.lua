--[[
-- @module dr2c.Server.Forth.Session
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local GMessage = require("dr2c.Shared.Network.Message")
local GForthSession = require("dr2c.Shared.Forth.Session")
local SServer = require("dr2c.Server.Network.Server")
local GSession = require("dr2c.Shared.Forth.Session")
local GUtilsMods = require("dr2c.Shared.Utils.Mods")

--- @class dr2c.SForthSession : dr2c.ForthSession
local SForthSession = GSession.new()

SForthSession.eventSForthSessionStart = TE.events:new(N_("SForthSessionStart"), {
	"GameMode",
	"Overrides",
	"Attributes",
	"Network",
})

SForthSession.eventSForthSessionFinish = TE.events:new(N_("SForthSessionFinish"), {
	"",
})

--- @param args dr2c.MessageContent.ForthSession
function SForthSession.start(args)
	local gameMode = args.gameMode
	local time = Time:getSystemTime()

	local e = {
		version = TE.engine:getVersion(),
		mods = GUtilsMods.collectMods(),
		modsHash = TE.mods:getLoadedModsHash(),
		gameMode = args.gameMode,
		createTime = time,
		lastSaveTime = time,
		playtime = 0,
		characters = args.characters,
		vehicle = args.vehicle,
		inventory = args.inventory,
	}
	TE.events:invoke(SForthSession.eventSForthSessionStart, e, gameMode)
end

function SForthSession.finish()
	SForthSession.setAttributes({})

	local e = {}
	TE.events:invoke(SForthSession.eventSForthSessionFinish, e)

	if not e.suppressed then
		SServer.broadcastReliable(GMessage.Type.WorldSessionFinish)
	end
end

TE.events:add(SForthSession.eventSForthSessionStart, function(e)
	if e.suppressed then
		return
	end

	local Attr = GForthSession.Attribute
	local set = SForthSession.setAttribute

	set(Attr.Version, e.version)
	set(Attr.Mods, e.mods)
	set(Attr.ModsHash, e.modsHash)
	set(Attr.GameMode, e.gameMode)
	set(Attr.CreateTime, e.createTime)
	set(Attr.LastSaveTime, e.lastSaveTime)
	set(Attr.Playtime, e.playtime)
	set(Attr.Characters, e.characters)
	set(Attr.Vehicle, e.vehicle)
	set(Attr.Inventory, e.inventory)
end, N_("SetAttributes"), "Attributes")

TE.events:add(SForthSession.eventSForthSessionStart, function(e)
	if not e.suppressed then
		SServer.broadcastReliable(GMessage.Type.ForthSessionStart, SForthSession.getAttributes())
	end
end, N_("Broadcast"), "Network")

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	SForthSession.start(e.content)
end, "ReceiveForthSessionStart", "Receive", GMessage.Type.ForthSessionStart)

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	SForthSession.finish()
end, "ReceiveForthSessionFinish", "Receive", GMessage.Type.ForthSessionFinish)

return SForthSession
