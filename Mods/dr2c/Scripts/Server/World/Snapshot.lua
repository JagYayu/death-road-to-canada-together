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
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
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

--- @param entry table
--- @param index integer
--- @return boolean
local function updateAndTryRemoveRequest(entry, index)
	if entry.reason then
		-- 发送请求失败理由

		local fields = GNetworkMessageFields.SnapshotResponse
		SNetworkServer.sendReliable(entry.clientID, GNetworkMessage.Type.SnapshotResponse, {
			[fields.snapshotID] = entry.snapshotID,
			[fields.snapshotData] = nil,
			[fields.reason] = entry.reason,
		})

		return true
	end

	-- 查找服务器本地的快照或接收到的快照
	local snapshotData = entry.snapshotData or SWorldSnapshot.getSnapshotData(entry.snapshotID)
	if snapshotData then
		-- 若找到则直接发送给请求者，并移除当前请求

		local fields = GNetworkMessageFields.SnapshotResponse
		SNetworkServer.sendReliable(entry.clientID, GNetworkMessage.Type.SnapshotResponse, {
			[fields.snapshotID] = entry.snapshotID,
			[fields.snapshotData] = snapshotData,
		})

		return true
	end

	-- 检查服务器向权威客户端的快照请求次数
	if entry.requestAllowedTimes <= 0 then
		-- 若超出发送次数了，则发送给请求者未找到快照

		local fields = GNetworkMessageFields.SnapshotResponse
		SNetworkServer.sendReliable(entry.clientID, GNetworkMessage.Type.SnapshotResponse, {
			[fields.snapshotID] = entry.snapshotID,
			[fields.snapshot] = nil,
			[fields.reason] = GNetworkReason.ID.NoAuthoritativeSnapshot,
		})

		return true
	end

	-- 尝试获取当前服务器内的权威客户端
	local hostClientID = SNetworkClients.getAuthoritativeClient()
	if not hostClientID then
		-- 发送给请求者当前服务器无权威客户端

		local fields = GNetworkMessageFields.SnapshotResponse
		SNetworkServer.sendReliable(entry.clientID, GNetworkMessage.Type.SnapshotResponse, {
			[fields.snapshotID] = entry.snapshotID,
			[fields.snapshot] = nil,
			[fields.reason] = GNetworkReason.ID.NoAuthoritativeClient,
		})

		return true
	end

	-- 向权威客户端发送快照请求
	if Time.getSystemTime() >= entry.requestSendTime then
		SNetworkServer.sendReliable(hostClientID, GNetworkMessage.Type.SnapshotRequest, entry.snapshotID)

		entry.requestSendTime = Time.getSystemTime() + 2.5
		entry.requestAllowedTimes = entry.requestAllowedTimes - 1
	end

	return false
end

function SWorldSnapshot.update()
	if serverSnapshotRequests[1] then
		List.removeIf(serverSnapshotRequests, updateAndTryRemoveRequest)
	end
end

local function resetSnapshots()
	SWorldSnapshot.clear()
end

TE.events:add(N_("SConnect"), resetSnapshots, "ResetSnapshots", "Reset")

TE.events:add(N_("SDisconnect"), resetSnapshots, "ResetSnapshots", "Reset")

TE.events:add(N_("SWorldSessionStart"), resetSnapshots, "ResetSnapshots", "Reset")

TE.events:add(N_("SWorldSessionFinish"), resetSnapshots, "ResetSnapshots", "Reset")

TE.events:add(N_("SUpdate"), function(e)
	SWorldSnapshot.update()
end, "responseSnapshotRequests", "Network", nil, 1)

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e) end, "ReceiveSnapshot", "Receive", GNetworkMessage.Type.Snapshot)

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	local snapshotID = tonumber(e.content)

	if snapshotID then
		serverSnapshotRequests[#serverSnapshotRequests + 1] = {
			clientID = e.clientID,
			snapshotID = snapshotID,
			requestSendTime = Time.getSystemTime(),
			requestAllowedTimes = 4,
			snapshotData = nil,
			reason = nil,
		}
	end
end, "ReceiveSnapshotRequest", "Receive", GNetworkMessage.Type.SnapshotRequest)

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	local fields = GNetworkMessageFields.SnapshotResponse

	local snapshotID = tonumber(e.content[fields.snapshotID])
	local snapshotData = e.content[fields.snapshotData]
	local reason = e.content[fields.reason]

	if not snapshotID then
		return
	end

	for _, request in ipairs(serverSnapshotRequests) do
		if request.snapshotID == snapshotID then
			if snapshotData then
				request.snapshotData = snapshotData
			else
				request.reason = reason
			end
		end
	end
end, "ReceiveSnapshotResponse", "Receive", GNetworkMessage.Type.SnapshotResponse)

return SWorldSnapshot
