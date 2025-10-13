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
local SServer = require("dr2c.Server.Network.Server")
local GSession = require("dr2c.Shared.Forth.Session")

--- @class dr2c.SForthSession : dr2c.ForthSession
local SForthSession = GSession.new()

SForthSession.eventSForthSessionStart = TE.events:new(N_("SForthSessionStart"), {
	"",
})

SForthSession.eventSForthSessionFinish = TE.events:new(N_("SForthSessionFinish"), {
	"",
})

function SForthSession.startNew()
	--
end

function SForthSession.finish()
	SForthSession.setAttributes({})

	local e = {}
	TE.events:invoke(SForthSession.eventSForthSessionFinish, e)

	if not e.suppressed then
		SServer.broadcastReliable(GMessage.Type.WorldSessionFinish)
	end
end

--- @param e {}
TE.events:add(N_("CMessage"), function(e)
	SForthSession.finish()
end, "ReceiveForthSessionStart", "Receive", GMessage.Type.ForthSessionStart)

--- @param e {}
TE.events:add(N_("CMessage"), function(e)
	SForthSession.finish()
end, "ReceiveForthSessionFinish", "Receive", GMessage.Type.ForthSessionFinish)

return SForthSession
