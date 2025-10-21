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

--- @enum dr2c.MessageFields.ClientConnect
MessageFields.ClientConnect = {
	clientID = 1,
}

--- @enum dr2c.MessageFields.ClientPublicAttribute
MessageFields.ClientPublicAttribute = {
	clientID = 1,
	attribute = 2,
	value = 3,
	requestID = 4,
}

--- @enum dr2c.MessageFields.ClientPrivateAttribute
MessageFields.ClientPrivateAttribute = {
	clientID = 1,
	attribute = 2,
	value = 3,
	requestID = 4,
}

--- @enum dr2c.MessageFields.PlayerInputs
MessageFields.PlayerInputs = {
	worldSessionID = 1,
	worldTick = 2,
	playerID = 3,
	playerInputs = 4,
}

--- @enum dr2c.MessageFields.Snapshot
MessageFields.Snapshot = {
	worldSessionID = 1,
	snapshotID = 2,
	snapshotData = 3,
}

--- @enum dr2c.MessageFields.SnapshotResponse
MessageFields.SnapshotResponse = {
	worldSessionID = 1,
	snapshotID = 2,
	snapshotData = 3,
	reason = 4,
}

--- @enum dr2c.MessageFields.WorldSessionStart
MessageFields.WorldSessionStart = {
	worldSessionID = 1,
	sponsorClientID = 2,
	attributes = 3,
	suppressedReason = 4,
}

--- @enum dr2c.MessageFields.WorldSessionRestart
MessageFields.WorldSessionRestart = {
	worldSessionID = 1,
}

--- @enum dr2c.MessageFields.WorldSessionFinish
MessageFields.WorldSessionFinish = {
	worldSessionID = 1,
	sponsorClientID = 2,
	attributes = 3,
	suppressedReason = 4,
}

--- @enum dr2c.MessageFields.WorldSessionPause
MessageFields.WorldSessionPause = {
	worldSessionID = 1,
	sponsorClientID = 2,
	suppressedReason = 3,
	timePaused = 4,
}

--- @enum dr2c.MessageFields.WorldSessionUnpause
MessageFields.WorldSessionUnpause = {
	worldSessionID = 1,
	sponsorClientID = 2,
	suppressedReason = 3,
	timePaused = 4,
}

--- @enum dr2c.MessageFields.ForthSessionStart
MessageFields.ForthSessionStart = {
	forthSessionID = 1,
	sponsorClientID = 2,
	attributes = 3,
	suppressedReason = 4,
}

--- @enum dr2c.MessageFields.ForthSessionFinish
MessageFields.ForthSessionFinish = {
	forthSessionID = 1,
	gameMode = 2,
	characters = 3,
}

return MessageFields
