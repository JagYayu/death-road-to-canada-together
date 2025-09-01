local GMessage = require("dr2c.shared.network.Message")

local SServer = require("dr2c.server.network.Server")

local SSnapshot = {}

--- @class dr2c.ServerSnapshots
local serverSnapshots = {}
local serverSnapshotRequests = {}

events:add(N_("SUpdate"), function(e)
	if not serverSnapshotRequests[1] then
		return
	end

	for _, entry in ipairs(serverSnapshotRequests) do
		local snapshot = serverSnapshots[entry.snapshotID]
		if snapshot then
			SServer.sendReliable(entry.clientID, GMessage.Type.SnapshotResponse, {
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
end, "ReceiveSnapshotRequest", "Receive", GMessage.Type.SnapshotRequest)

return SSnapshot
