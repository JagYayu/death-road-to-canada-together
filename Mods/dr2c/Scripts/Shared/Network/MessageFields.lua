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
	worldTick = 1,
	playerID = 2,
	playerInputs = 3,
}

--- @enum dr2c.MessageFields.Snapshot
MessageFields.Snapshot = {
	snapshotID = 1,
	snapshotData = 2,
}

--- @enum dr2c.MessageFields.SnapshotResponse
MessageFields.SnapshotResponse = {
	snapshotID = 1,
	snapshotData = 2,
	reason = 3,
}

--- @enum dr2c.MessageFields.WorldSessionStart
MessageFields.WorldSessionStart = {
	sponsorClientID = 1,
	attributes = 2,
	suppressedReason = 3,
}

--- @enum dr2c.MessageFields.WorldSessionRestart
MessageFields.WorldSessionRestart = {}

--- @enum dr2c.MessageFields.WorldSessionFinish
MessageFields.WorldSessionFinish = {
	sponsorClientID = 1,
	attributes = 2,
	suppressedReason = 3,
}

--- @enum dr2c.MessageFields.WorldSessionPause
MessageFields.WorldSessionPause = {
	sponsorClientID = 1,
	suppressedReason = 2,
	timePaused = 3,
}

--- @enum dr2c.MessageFields.WorldSessionUnpause
MessageFields.WorldSessionUnpause = {
	sponsorClientID = 1,
	suppressedReason = 2,
	timePaused = 3,
}

--- @enum dr2c.MessageFields.ForthSessionStart
MessageFields.ForthSessionStart = {
	sponsorClientID = 1,
	attributes = 2,
	suppressedReason = 3,
}

--- @enum dr2c.MessageFields.ForthSessionFinish
MessageFields.ForthSessionFinish = {
	gameMode = 1,
	characters = 2,
}

return MessageFields
