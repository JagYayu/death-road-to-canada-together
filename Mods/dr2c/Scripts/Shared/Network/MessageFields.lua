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

MessageFields.requestHandle = 0

local metatable = {
	__index = {
		--- 使用RPC模块通信时的请求句柄，接收者必须回传该句柄
		requestHandle = MessageFields.requestHandle,
	},
}

--- @enum dr2c.MessageFields.CClientConnect
MessageFields.CClientConnect = setmetatable({
	clientID = 1,
}, metatable)

--- @enum dr2c.MessageFields.CClientSetPublicAttribute
MessageFields.CClientSetPublicAttribute = setmetatable({
	clientID = 1,
	attribute = 2,
	value = 3,
}, metatable)

--- @enum dr2c.MessageFields.SClientSetPublicAttribute
MessageFields.SClientSetPublicAttribute = setmetatable({
	attribute = 1,
	value = 2,
}, metatable)

--- @enum dr2c.MessageFields.CClientSetPrivateAttribute
MessageFields.CClientSetPrivateAttribute = setmetatable({
	clientID = 1,
	attribute = 2,
	value = 3,
}, metatable)

--- @enum dr2c.MessageFields.SClientSetPrivateAttribute
MessageFields.SClientSetPrivateAttribute = setmetatable({
	attribute = 1,
	value = 2,
}, metatable)

--- @enum dr2c.MessageFields.CClientGetPrivateAttribute
MessageFields.CClientGetPrivateAttribute = setmetatable({
	value = 1,
}, metatable)

--- @enum dr2c.MessageFields.SClientGetPrivateAttribute
MessageFields.SClientGetPrivateAttribute = setmetatable({
	clientID = 1,
	attribute = 2,
}, metatable)

--- @enum dr2c.MessageFields.CServerAttribute
MessageFields.CServerAttribute = setmetatable({
	attribute = 1,
	value = 2,
}, metatable)

--- @enum dr2c.MessageFields.SServerAttribute
MessageFields.SServerAttribute = setmetatable({
	attribute = 1,
	value = 2,
}, metatable)

--- @enum dr2c.MessageFields.CRooms
MessageFields.CRooms = setmetatable({
	--
}, metatable)

--- @enum dr2c.MessageFields.CRoomAttribute
MessageFields.CRoomAttribute = setmetatable({
	--
}, metatable)

--- @enum dr2c.MessageFields.SRoomAttribute
MessageFields.SRoomAttribute = setmetatable({
	--
}, metatable)

--- @enum dr2c.MessageFields.CCreateRoom
MessageFields.CCreateRoom = setmetatable({
	roomID = 1,
	sponsorClientID = 2,
}, metatable)

--- @enum dr2c.MessageFields.SCreateRoom
MessageFields.SCreateRoom = setmetatable({
	roomID = 2,
}, metatable)

--- @enum dr2c.MessageFields.CDestroyRoom
MessageFields.CDestroyRoom = setmetatable({
	roomID = 1,
}, metatable)

--- @enum dr2c.MessageFields.SDestroyRoom
MessageFields.SDestroyRoom = setmetatable({
	roomID = 2,
}, metatable)

--- @enum dr2c.MessageFields.ForthSessionStart
MessageFields.ForthSessionStart = setmetatable({
	roomID = 1,
	forthSessionID = 2,
	sponsorClientID = 3,
	attributes = 4,
	suppressedReason = 5,
}, metatable)

--- @enum dr2c.MessageFields.ForthSessionFinish
MessageFields.ForthSessionFinish = setmetatable({
	roomID = 1,
	forthSessionID = 2,
}, metatable)

--- @enum dr2c.MessageFields.CWorldSessionStart
MessageFields.CWorldSessionStart = setmetatable({
	worldSessionID = 1,
	sponsorClientID = 2,
	attributes = 3,
	suppressedReason = 4,
}, metatable)

--- @enum dr2c.MessageFields.SWorldSessionStart
MessageFields.SWorldSessionStart = setmetatable({
	roomID = 1,
	worldSessionID = 2,
	attributes = 3,
}, metatable)

--- @enum dr2c.MessageFields.CWorldSessionRestart
MessageFields.CWorldSessionRestart = setmetatable({
	roomID = 1,
	worldSessionID = 2,
}, metatable)

--- @enum dr2c.MessageFields.SWorldSessionRestart
MessageFields.SWorldSessionRestart = setmetatable({
	roomID = 1,
	worldSessionID = 2,
}, metatable)

--- @enum dr2c.MessageFields.CWorldSessionFinish
MessageFields.CWorldSessionFinish = setmetatable({
	roomID = 1,
	worldSessionID = 2,
	sponsorClientID = 3,
	attributes = 4,
	suppressedReason = 5,
}, metatable)

--- @enum dr2c.MessageFields.SWorldSessionFinish
MessageFields.SWorldSessionFinish = setmetatable({
	roomID = 1,
	worldSessionID = 2,
	sponsorClientID = 3,
	attributes = 4,
	suppressedReason = 5,
}, metatable)

--- @enum dr2c.MessageFields.CWorldSessionPause
MessageFields.CWorldSessionPause = setmetatable({
	roomID = 1,
	worldSessionID = 2,
	sponsorClientID = 3,
	suppressedReason = 4,
	timePaused = 5,
}, metatable)

--- @enum dr2c.MessageFields.SWorldSessionPause
MessageFields.SWorldSessionPause = setmetatable({
	roomID = 1,
	worldSessionID = 2,
	sponsorClientID = 3,
	suppressedReason = 4,
	timePaused = 5,
}, metatable)

--- @enum dr2c.MessageFields.CWorldSessionUnpause
MessageFields.CWorldSessionUnpause = setmetatable({
	roomID = 1,
	worldSessionID = 2,
	sponsorClientID = 3,
	suppressedReason = 4,
	timePaused = 5,
}, metatable)

--- @enum dr2c.MessageFields.CPlayerInputBuffer
MessageFields.CPlayerInputBuffers = setmetatable({
	worldSessionID = 1,
	buffer = 2,
}, metatable)

--- @enum dr2c.MessageFields.CPlayerInputs
MessageFields.CPlayerInputs = setmetatable({
	worldSessionID = 1,
	worldTick = 2,
	playerID = 3,
	playerInputs = 4,
}, metatable)

--- @enum dr2c.MessageFields.SPlayerInputs
MessageFields.SPlayerInputs = setmetatable({
	worldSessionID = 1,
	worldTick = 2,
	playerID = 3,
	playerInputs = 4,
}, metatable)

MessageFields.CClientPlayerInputBuffersRequest = setmetatable({
	buffer = 1,
}, metatable)

MessageFields.SClientPlayerInputBuffersRequest = setmetatable({}, metatable)

--- @enum dr2c.MessageFields.CClientSnapshotRequest
MessageFields.CClientSnapshotRequest = setmetatable({
	snapshotID = 1,
	snapshotData = 2,
}, metatable)

--- @enum dr2c.MessageFields.SClientSnapshotRequest
MessageFields.SClientSnapshotRequest = setmetatable({
	snapshotID = 1,
}, metatable)

--- @enum dr2c.MessageFields.CServerSnapshotRequest
MessageFields.CServerSnapshotRequest = setmetatable({
	worldSessionID = 1,
	snapshotID = 2,
}, metatable)

--- @enum dr2c.MessageFields.SServerSnapshotRequest
MessageFields.SServerSnapshotRequest = setmetatable({
	snapshotID = 1,
	snapshotData = 2,
}, metatable)

return MessageFields
