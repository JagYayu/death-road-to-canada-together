--[[
-- @module dr2c.Server.World.Snapshot
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("TE.Table")
local List = require("TE.List")

local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkReason = require("dr2c.Shared.Network.Reason")
local GWorldSnapshot = require("dr2c.Shared.World.Snapshot")

local SNetworkClients = require("dr2c.Server.Network.Clients")
local SNetworkServer = require("dr2c.Server.Network.Server")

--- @class dr2c.SWorldSnapshot : dr2c.WorldSnapshot
local SWorldSnapshot = GWorldSnapshot.new()

--- @class dr2c.ServerSnapshots
local serverSnapshots = {}

local serverSnapshotRequests = {}

serverSnapshots = persist("serverSnapshots", function()
	return serverSnapshots
end)
serverSnapshotRequests = persist("serverSnapshotRequests", function()
	return serverSnapshotRequests
end)

local function resetSnapshots()
	SWorldSnapshot.clear()
end

TE.events:add(N_("SConnect"), resetSnapshots, "ResetSnapshots", "Reset")

TE.events:add(N_("SDisconnect"), resetSnapshots, "ResetSnapshots", "Reset")

TE.events:add(N_("SWorldSessionStart"), resetSnapshots, "ResetSnapshots", "Reset")

TE.events:add(N_("SWorldSessionFinish"), resetSnapshots, "ResetSnapshots", "Reset")

TE.events:add(N_("SUpdate"), function(e)
	-- 没有任何快照请求，直接返回
	if not serverSnapshotRequests[1] then
		return
	end

	List.removeIfV(serverSnapshotRequests, function(entry, index)
		-- 查找服务器本地缓存的快照
		local snapshot = serverSnapshots[entry.snapshotID]
		if snapshot then
			SNetworkServer.sendReliable(entry.clientID, GNetworkMessage.Type.SnapshotResponse, {
				snapshotID = entry.snapshotID,
				snapshot = snapshot,
			})

			return true
		end

		-- 检查服务器向权威客户端的快照请求次数
		if entry.requestTimes <= 0 then
			SNetworkServer.sendReliable(entry.clientID, GNetworkMessage.Type.SnapshotResponse, {
				snapshotID = entry.snapshotID,
				snapshot = nil,
				reason = GNetworkReason.ID.NoAuthoritativeSnapshot,
			})

			return true
		end

		-- 尝试获取当前服务器内的权威客户端
		local hostClientID = SNetworkClients.getAuthoritativeClient()
		if not hostClientID then
			SNetworkServer.sendReliable(entry.clientID, GNetworkMessage.Type.SnapshotResponse, {
				snapshotID = entry.snapshotID,
				snapshot = nil,
				reason = GNetworkReason.ID.NoAuthoritativeClient,
			})

			return true
		end

		-- 向权威客户端发送快照请求
		if Time.getSystemTime() >= entry.requestSendTime then
			SNetworkServer.sendReliable(hostClientID, GNetworkMessage.Type.SnapshotRequest)

			entry.requestSendTime = Time.getSystemTime() + 2.5
			entry.requestTimes = entry.requestTimes - 1
		end

		return false
	end)
end, "responseSnapshotRequests", "Network", nil, 1)

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	serverSnapshotRequests[#serverSnapshotRequests + 1] = {
		clientID = e.clientID,
		snapshotID = e.content.snapshotID,
		requestSendTime = Time.getSystemTime(),
		requestTimes = 4,
	}
end, "ReceiveSnapshotRequest", "Receive", GNetworkMessage.Type.SnapshotRequest)

return SWorldSnapshot
