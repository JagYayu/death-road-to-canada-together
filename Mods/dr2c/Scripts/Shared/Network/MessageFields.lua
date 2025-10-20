--[[
-- @module dr2c.Shared.Network.MessageFields
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("TE.Table")

--- @class dr2c.GNetworkMessageFields
local MessageFields = {}

MessageFields.ClientPrivateAttribute = {
	requestID = 1,
	clientID = 2,
	attribute = 3,
}

MessageFields.PlayerInputs = {
	worldTick = 1,
	playerID = 2,
	playerInputs = 3,
}

MessageFields.Snapshot = {
	snapshotID = 1,
	snapshotData = 2,
}

MessageFields.SnapshotResponse = {
	snapshotID = 1,
	snapshotData = 2,
	reason = 3,
}

MessageFields.WorldSessionStart = {
	sponsorClientID = 1,
	attributes = 2,
	suppressedReason = 3,
}

MessageFields.ForthSessionStart = {
	gameMode = 1,
	characters = 2,
}

for key, value in pairs(MessageFields) do
	MessageFields[key] = Table.readonly(value)
end

return MessageFields
