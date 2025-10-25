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

local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local SNetworkClients = require("dr2c.Server.Network.Clients")
local SNetworkRPC = require("dr2c.Server.Network.RPC")
local SNetworkRoom = require("dr2c.Server.Network.Room")
local SNetworkServer = require("dr2c.Server.Network.Server")
local SWorldSession = require("dr2c.Server.World.Session")

--- @class dr2c.SWorldSnapshot
local SWorldSnapshot = {}

--- @type table<dr2c.NetworkRoomID, { id: dr2c.SnapshotID, data: dr2c.SnapshotData }>
local roomsLatestSnapshots = {}

roomsLatestSnapshots = persist("serverSnapshots", function()
	return roomsLatestSnapshots
end)

--- @param roomID dr2c.NetworkRoomID
--- @return dr2c.SnapshotID? snapshotID
--- @return dr2c.SnapshotData snapshotData
--- @nodiscard
function SWorldSnapshot.getSnapshot(roomID)
	local snapshot = roomsLatestSnapshots[roomID]
	if snapshot then
		return snapshot.id, snapshot.data
	else
		return nil, ""
	end
end

--- @param roomID? dr2c.NetworkRoomID
function SWorldSnapshot.clear(roomID)
	if roomID then
		roomsLatestSnapshots[roomID] = nil
	else
		roomsLatestSnapshots = {}
	end
end

local function resetSnapshots()
	SWorldSnapshot.clear()
end

TE.events:add(N_("SHost"), resetSnapshots, "ResetSnapshots", "Reset")

TE.events:add(N_("SShutdown"), resetSnapshots, "ResetSnapshots", "Reset")

TE.events:add(N_("SWorldSessionStart"), resetSnapshots, "ResetSnapshots", "Reset")

TE.events:add(N_("SWorldSessionFinish"), resetSnapshots, "ResetSnapshots", "Reset")

--- @param content table
--- @param clientID TE.Network.ClientID
--- @param requestHandle dr2c.NetworkRemoteRequestHandle
local function serverSnapshotRequestCallback(content, clientID, requestHandle)
	if content then
		local sFields = GNetworkMessageFields.SServerSnapshotRequest
		local cFields = GNetworkMessageFields.CClientSnapshotRequest

		local snapshotID = content[sFields.snapshotID]
		local snapshotData = content[sFields.snapshotData]

		SNetworkServer.sendReliable(clientID, GNetworkMessage.Type.ClientSnapshotRequest, {
			[cFields.requestHandle] = requestHandle,
			[cFields.snapshotID] = snapshotID,
			[cFields.snapshotData] = snapshotData,
		})

		local roomID = SNetworkClients.getRoomID(clientID)
		if roomID then
			local latestSnapshot = roomsLatestSnapshots[roomID]
			if not latestSnapshot then
				latestSnapshot = {
					id = snapshotID,
					data = snapshotData,
				}
			elseif latestSnapshot.id <= snapshotID then
				latestSnapshot.id = snapshotID
				latestSnapshot.data = snapshotData
			end
		end
	else -- TODO
	end
end

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	if type(e.content) ~= "table" then
		return
	end

	local sFields = GNetworkMessageFields.SClientSnapshotRequest

	local snapshotID = e.content[sFields.snapshotID]

	local clientID = e.clientID
	local roomID = SNetworkClients.getRoomID(clientID)
	if not (roomID and SNetworkRoom.hasRoom(roomID)) then
		return
	end

	local requestHandle = e.content[sFields.requestHandle]

	local serverSnapshot = roomsLatestSnapshots[roomID]
	if serverSnapshot and serverSnapshot.id == snapshotID then
		local cFields = GNetworkMessageFields.CClientSnapshotRequest

		SNetworkServer.sendReliable(clientID, e.type, {
			[cFields.requestHandle] = requestHandle,
			[cFields.snapshotID] = snapshotID,
			[cFields.snapshotData] = serverSnapshot.data,
		})

		return
	end

	local authoritativeClientID = SNetworkClients.getAuthoritativeClient(roomID)
	if not authoritativeClientID then
		return
	end

	local cFields = GNetworkMessageFields.CServerSnapshotRequest

	SNetworkRPC.sendReliable(authoritativeClientID, GNetworkMessage.Type.ServerSnapshotRequest, {
		[cFields.worldSessionID] = SWorldSession.getSessionID(roomID),
		[cFields.snapshotID] = snapshotID,
	}, nil, serverSnapshotRequestCallback, nil, clientID, requestHandle)
end, "ResponseSnapshotRequest", "Response", GNetworkMessage.Type.ClientSnapshotRequest)

return SWorldSnapshot
