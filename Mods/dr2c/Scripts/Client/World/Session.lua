--[[
-- @module dr2c.Client.World.Session
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")
local json = require("Lib.json")

local CNetworkClient = require("dr2c.Client.Network.Client")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GWorldSession = require("dr2c.Shared.World.Session")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")

--- @class dr2c.CWorldSession : dr2c.MWorldSession
local CWorldSession = GWorldSession.new()

--- 开始世界会话，将发送申请给服务器
--- @param attributes dr2c.WorldSessionAttributes
--- @return boolean
function CWorldSession.start(attributes)
	return CNetworkClient.sendReliable(GNetworkMessage.Type.SWorldSessionStart, attributes)
end

--- 重启世界会话，将发送申请给服务器
--- @return boolean
function CWorldSession.restart()
	return CNetworkClient.sendReliable(GNetworkMessage.Type.SWorldSessionRestart)
end

--- 结束世界会话，将发送申请给服务器
--- @return boolean
function CWorldSession.finish()
	return CNetworkClient.sendReliable(GNetworkMessage.Type.SWorldSessionFinish)
end

--- 暂停世界会话，将发送申请给服务器
--- @return boolean
function CWorldSession.pause()
	return CNetworkClient.sendReliable(GNetworkMessage.Type.SWorldSessionPause)
end

--- 继续世界会话，将发送申请给服务器
--- @return boolean
function CWorldSession.unpause()
	return CNetworkClient.sendReliable(GNetworkMessage.Type.SWorldSessionUnpause)
end

CWorldSession.eventCWorldSessionStart = TE.events:new(N_("CWorldSessionStart"), {
	"Reset",
	"Level",
	"Scene",
	"Map",
})

CWorldSession.eventCWorldSessionFinish = TE.events:new(N_("CWorldSessionFinish"), {
	"Map",
	"Scene",
	"Level",
	"Reset",
})

CWorldSession.eventCWorldSessionRestart = TE.events:new(N_("CWorldSessionRestart"), {
	"", -- TODO
})

CWorldSession.eventCWorldSessionPause = TE.events:new(N_("CWorldSessionPause"), {
	"Time",
})

CWorldSession.eventCWorldSessionUnpause = TE.events:new(N_("CWorldSessionUnpause"), {
	"Time",
})

--- 开始世界会话，将立即应用到本地客户端
--- 不是所有可用的属性都是有效的，一些内部的属性例如`GWorldSession.Attribute.TimeStart`会被函数或其他时间句柄重写。
--- @param attributes dr2c.WorldSessionAttributes
--- @param sponsorClientID TE.Network.ClientID?
function CWorldSession.startLocally(attributes, sponsorClientID)
	sponsorClientID = sponsorClientID or CNetworkClient.getClientID() or 0
	CWorldSession.setAttributes(attributes)

	CWorldSession.setAttribute(GWorldSession.Attribute.TimeStart, Time.getSystemTime())

	--- @class dr2c.E.CWorldSessionStart
	local e = {
		attributes = CWorldSession.getAttributes(),
		sponsorClientID = sponsorClientID,
	}

	TE.events:invoke(
		CWorldSession.eventCWorldSessionStart,
		e,
		tonumber(attributes[GWorldSession.Attribute.Mode]) or GWorldSession.Mode.None
	)
end

--- 重启世界会话，将立即应用到本地客户端
function CWorldSession.restartLocally()
	-- TODO
end

--- 结束世界会话，将立即应用到本地客户端
function CWorldSession.finishLocally()
	CWorldSession.resetAttributes()

	TE.events:invoke(CWorldSession.eventCWorldSessionFinish, {})
end

--- 暂停世界会话，将立即应用到本地客户端
--- @param timePaused number
function CWorldSession.pauseLocally(timePaused)
	CWorldSession.setAttribute(GWorldSession.Attribute.State, GWorldSession.State.Paused)
	CWorldSession.setAttribute(GWorldSession.Attribute.TimePaused, timePaused)

	TE.events:invoke(CWorldSession.eventCWorldSessionPause, {})
end

--- 继续世界会话，将立即应用到本地客户端
--- @param timePaused number
function CWorldSession.unpauseLocally(timePaused)
	CWorldSession.setAttribute(GWorldSession.Attribute.State, GWorldSession.State.Playing)
	CWorldSession.setAttribute(GWorldSession.Attribute.State, GWorldSession.State.Playing)

	local elapsedPaused = CWorldSession.getAttribute(GWorldSession.Attribute.ElapsedPaused)
	elapsedPaused = elapsedPaused + timePaused - CWorldSession.getAttribute(GWorldSession.Attribute.TimePaused)
	CWorldSession.setAttribute(GWorldSession.Attribute.ElapsedPaused, elapsedPaused)

	TE.events:invoke(CWorldSession.eventCWorldSessionUnpause, {})
end

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	if type(e.content) ~= "table" then
		return
	end

	CWorldSession.setAttributes(e.content)
end, "ReceiveWorldSessionAttributes", "Receive", GNetworkMessage.Type.CWorldSessionAttributes)

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	if type(e.content) ~= "table" then
		return
	end

	local fields = GNetworkMessageFields.CWorldSessionStart
	local suppressedReason = e.content[fields.suppressedReason]

	if not suppressedReason then
		log.info(("Client %s started world session"):format(e.content[fields.sponsorClientID]))

		CWorldSession.startLocally(e.content[fields.attributes])
	elseif e.content[fields.sponsorClientID] == CNetworkClient.getClientID() and log.canWarn() then
		log.warn(("Cannot start world session: %s"):format(suppressedReason))
	end
end, "StartWorldSessionLocally", "Receive", GNetworkMessage.Type.CWorldSessionStart)

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	if type(e.content) ~= "table" then
		return
	end

	local fields = GNetworkMessageFields.CWorldSessionStart
	local suppressedReason = e.content[fields.suppressedReason]

	if not suppressedReason then
		log.info(("Client %s restarted world session"):format(e.content[fields.sponsorClientID]))

		CWorldSession.restartLocally()
	elseif e.content[fields.sponsorClientID] == CNetworkClient.getClientID() and log.canWarn() then
		log.warn(("Cannot restart world session: %s"):format(suppressedReason))
	end
end, "RestartWorldSessionLocally", "Receive", GNetworkMessage.Type.CWorldSessionRestart)

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	if type(e.content) ~= "table" then
		return
	end

	local fields = GNetworkMessageFields.CWorldSessionFinish
	local suppressedReason = e.content[fields.suppressedReason]

	if not suppressedReason then
		log.info(("Client %s finished world session"):format(e.content[fields.sponsorClientID]))

		CWorldSession.finishLocally()
	elseif e.content[fields.sponsorClientID] == CNetworkClient.getClientID() and log.canWarn() then
		log.warn(("Cannot finish world session: %s"):format(suppressedReason))
	end
end, "FinishWorldSessionLocally", "Receive", GNetworkMessage.Type.CWorldSessionFinish)

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	if type(e.content) ~= "table" then
		return
	end

	local fields = GNetworkMessageFields.CWorldSessionPause
	local suppressedReason = e.content[fields.suppressedReason]

	if not suppressedReason then
		log.info(("Client %s paused world session"):format(e.content[fields.sponsorClientID]))

		CWorldSession.pauseLocally(e.content[fields.timePaused])
	elseif e.content[fields.sponsorClientID] == CNetworkClient.getClientID() and log.canWarn() then
		log.warn(("Cannot pause world session: %s"):format(suppressedReason))
	end
end, "PauseWorldSessionLocally", "Receive", GNetworkMessage.Type.CWorldSessionPause)

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	if type(e.content) ~= "table" then
		return
	end

	local fields = GNetworkMessageFields.CWorldSessionUnpause
	local suppressedReason = e.content[fields.suppressedReason]

	if not suppressedReason then
		log.info(("Client %s unpaused world session"):format(e.content[fields.sponsorClientID]))

		CWorldSession.unpauseLocally(e.content[fields.timePaused])
	elseif e.content[fields.sponsorClientID] == CNetworkClient.getClientID() and log.canWarn() then
		log.warn(("Cannot unpause world session: %s"):format(suppressedReason))
	end
end, "UnpauseWorldSessionLocally", "Receive", GNetworkMessage.Type.CWorldSessionUnpause)

--- @param e TE.E.DebugCommand
TE.events:add("DebugProvide", function(e)
	e.data:setDebugCommand({
		name = "startWorldSession",
		help = "startWorldSession [args]",
		func = function(arg)
			local success, result
			if arg ~= "" then
				success, result = pcall(json.decode, arg)
			else
				success, result = true, {}
			end

			if success then
				CWorldSession.start(result)
				return { "Sent start world session request to server", EDebugConsoleCode.Success }
			else
				return { result, EDebugConsoleCode.Failure }
			end
		end,
	})
end, "ProvideClientWorldSessionCommands")

return CWorldSession
