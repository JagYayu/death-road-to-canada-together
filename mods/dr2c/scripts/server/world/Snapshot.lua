--[[
-- @module dr2c.server.world.Snapshot
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local GNetworkMessage = require("dr2c.shared.network.Message")
local GWorldSnapshot = require("dr2c.shared.world.Snapshot")

local SNetworkServer = require("dr2c.server.network.Server")

--- @class dr2c.SWorldSnapshot : dr2c.WorldSnapshot
local SWorldSnapshot = GWorldSnapshot.new()

--- @class dr2c.ServerSnapshots
local serverSnapshots = {}
local serverSnapshotRequests = {}

local function resetSnapshots()
	SWorldSnapshot.clear()
end

events:add(N_("CConnect"), resetSnapshots, N_("ResetSnapshot"), "Reset")

events:add(N_("CDisconnect"), resetSnapshots, N_("ResetSnapshot"), "Reset")

events:add(N_("SUpdate"), function(e)
	if not serverSnapshotRequests[1] then
		return
	end

	for _, entry in ipairs(serverSnapshotRequests) do
		local snapshot = serverSnapshots[entry.snapshotID]
		if snapshot then
			SNetworkServer.sendReliable(entry.clientID, GNetworkMessage.Type.SnapshotResponse, {
				snapshotID = entry.snapshotID,
				snapshot = snapshot,
			})
		end
	end

	serverSnapshotRequests = {}
end, "responseSnapshotRequests", "Network", nil, 1)

--- @param e dr2c.E.ServerMessage
events:add(N_("SMessage"), function(e)
	serverSnapshotRequests[#serverSnapshotRequests + 1] = {
		clientID = e.clientID,
		snapshotID = e.content.snapshotID,
	}
end, "ReceiveSnapshotRequest", "Receive", GNetworkMessage.Type.SnapshotRequest)

return SWorldSnapshot
