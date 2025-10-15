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

local GForthSession = require("dr2c.Shared.Forth.Session")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkReason = require("dr2c.Shared.Network.Reason")
local SNetworkServer = require("dr2c.Server.Network.Server")
local GUtilsMods = require("dr2c.Shared.Utils.Mods")

--- @class dr2c.SForthSession : dr2c.ForthSession
local SForthSession = GForthSession.new()

SForthSession.eventSForthSessionStart = TE.events:new(N_("SForthSessionStart"), {
	"GameMode",
	"Overrides",
	"Attributes",
	"Network",
})

SForthSession.eventSForthSessionFinish = TE.events:new(N_("SForthSessionFinish"), {
	"",
})

--- @class dr2c.ServerForthSessionStartArgs : dr2c.MessageContent.ForthSession
--- @field clientID? TE.Network.ClientID

--- @param args dr2c.ServerForthSessionStartArgs
function SForthSession.start(args)
	local gameMode = args.gameMode
	local time = Time:getSystemTime()

	local e = {
		sponsorClientID = args.clientID,
		version = tostring(TE.engine:getVersion()),
		mods = GUtilsMods.collectMods(),
		modsHash = ("%X"):format(TE.mods:getLoadedModsHash()),
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
		SNetworkServer.broadcastReliable(GNetworkMessage.Type.WorldSessionFinish, {}) -- TODO
	end
end

TE.events:add(SForthSession.eventSForthSessionStart, function(e)
	local Attr = GForthSession.Attribute
	local set = SForthSession.setAttribute

	if e.suppressed then
		set(Attr.Active, false)

		log.info("Suppressed forth session")
	else
		set(Attr.Active, true)
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

		log.info("Started forth session")
	end
end, N_("SetAttributes"), "Attributes")

TE.events:add(SForthSession.eventSForthSessionStart, function(e)
	if e.suppressed then
		SNetworkServer.broadcastReliable(GNetworkMessage.Type.ForthSessionStart, {
			attributes = {},
			sponsorClientID = e.sponsorClientID,
			suppressed = type(e.suppressed) == "string" and e.suppressed or GNetworkReason,
		})
	else
		SNetworkServer.broadcastReliable(GNetworkMessage.Type.ForthSessionStart, {
			attributes = SForthSession.getAttributes(),
			sponsorClientID = e.sponsorClientID,
			suppressed = false,
		})
	end
end, N_("Broadcast"), "Network")

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	if type(e.content) == "table" then
		e.content.clientID = e.clientID
		SForthSession.start(e.content)
	end
end, "ReceiveForthSessionStart", "Receive", GNetworkMessage.Type.ForthSessionStart)

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	SForthSession.finish()
end, "ReceiveForthSessionFinish", "Receive", GNetworkMessage.Type.ForthSessionFinish)

return SForthSession
