--[[
-- @module dr2c.Client.World.Snapshot
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local String = require("TE.String")
local Table = require("TE.Table")

local CNetworkClient = require("dr2c.Client.Network.Client")
local CNetworkRPC = require("dr2c.Client.Network.RPC")
local CWorldSession = require("dr2c.Client.World.Session")
local GNetworkClient = require("dr2c.Shared.Network.Client")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local GWorldSession = require("dr2c.Shared.World.Session")
local GWorldSnapshot = require("dr2c.Shared.World.Snapshot")
local GWorldTick = require("dr2c.Shared.World.Tick")

local math_floor = math.floor

--- @class dr2c.SnapshotID : integer

--- @class dr2c.CWorldSnapshot
local CWorldSnapshot = {}

--- 快照数据列表
--- 快照ID都是连续的，中间不允许出现ID空洞或跳跃，因此只能连续批量地移除。
--- @type string[]
local snapshotDataList = {}
--- 第一个快照的ID
--- @type dr2c.SnapshotID?
local firstSnapshotID = nil

snapshotDataList = persist("snapshotDataList", function()
	return snapshotDataList
end)
firstSnapshotID = persist("firstSnapshotID", function()
	return firstSnapshotID
end)

function CWorldSnapshot.clear()
	snapshotDataList = {}
	firstSnapshotID = nil
end

--- @return string[]
function CWorldSnapshot.getSnapshotDataList()
	return snapshotDataList
end

function CWorldSnapshot.setSnapshotDataList(snapshotDataList_)
	snapshotDataList = snapshotDataList_
	firstSnapshotID = snapshotDataList[1]
end

--- @return integer
function CWorldSnapshot.getSnapshotCount()
	return #snapshotDataList
end

--- @return dr2c.SnapshotID?
function CWorldSnapshot.getFirstSnapshotID()
	return firstSnapshotID
end

--- @return dr2c.SnapshotID?
function CWorldSnapshot.getLastSnapshotID()
	return firstSnapshotID and (firstSnapshotID + #snapshotDataList - 1) or nil
end

--- @param snapshotID dr2c.SnapshotID
--- @return string?
function CWorldSnapshot.getSnapshotData(snapshotID)
	if not firstSnapshotID then
		return
	elseif snapshotID < firstSnapshotID then
		return
	elseif snapshotID >= firstSnapshotID + #snapshotDataList then
		return
	end

	return snapshotDataList[snapshotID - firstSnapshotID + 1]
end

--- @param snapshotID integer
--- @param snapshotData string
function CWorldSnapshot.setSnapshotData(snapshotID, snapshotData)
	if snapshotID <= 0 then
		error("SnapshotID must be greater than 0", 2)
	end

	CWorldSnapshot.dropBackward(snapshotID)

	if firstSnapshotID then
		local index = snapshotID - firstSnapshotID
		if not snapshotDataList[index] then
			print(snapshotDataList)
			error("Snapshot list cannot have holes", 2)
		end

		snapshotDataList[index + 1] = snapshotData
	else
		snapshotDataList[1] = snapshotData
		firstSnapshotID = snapshotID
	end
end

--- Remove current and subsequent snapshots.
--- 移除当前及之后的快照。
--- @return integer count
function CWorldSnapshot.dropBackward(snapshotID)
	if not firstSnapshotID then
		return 0
	end

	local len = #snapshotDataList
	local index = snapshotID - firstSnapshotID + 1

	if index <= 1 then
		Table.clear(snapshotDataList)
		firstSnapshotID = nil

		return len
	else
		for i = len, index, -1 do
			snapshotDataList[i] = nil
		end

		return len - snapshotID + firstSnapshotID
	end
end

--- Remove current and previous snapshots.
--- 移除当前及先前的快照。
--- @return integer count
function CWorldSnapshot.dropForward(snapshotID)
	if not firstSnapshotID then
		return 0
	end

	local len = #snapshotDataList
	local count = snapshotID - firstSnapshotID + 1

	if count >= len then
		Table.clear(snapshotDataList)
		firstSnapshotID = nil

		return len
	else
		for i = count + 1, len do
			local j = i - count
			snapshotDataList[j] = snapshotDataList[i]
			snapshotDataList[i] = nil
		end

		for i = len, len - count + 1, -1 do
			snapshotDataList[i] = nil
		end

		firstSnapshotID = firstSnapshotID + count
		return count
	end
end

--- @param snapshotLifetime number
--- @return integer?
function CWorldSnapshot.dropOldSnapshots(snapshotLifetime, factor)
	factor = factor or 2
	local maxCount = math_floor(snapshotLifetime * GWorldTick.getTPS() * GWorldSnapshot.getIDsPerTick()) * factor -- 3 * CWorldTick.getTPS()

	if #snapshotDataList > maxCount then
		local snapshotID = CWorldSnapshot.getLastSnapshotID() - math.floor(maxCount / factor)

		local droppedNumber = CWorldSnapshot.dropForward(snapshotID)

		return droppedNumber
	end
end

local eventClientSnapshotCollect = TE.events:new(N_("CSnapshotCollect"), {
	"ECS",
	"Registry",
})

local eventClientSnapshotDispense = TE.events:new(N_("CSnapshotDispense"), {
	"Registry",
	"ECS",
})

--- @return string data
function CWorldSnapshot.serialize()
	-- Utility.assertRuntime()

	--- @class dr2c.E.CSnapshotCollect
	local e = {
		snapshot = {},
	}
	TE.events:invoke(eventClientSnapshotCollect, e)

	local data = String.bufferEncode(e.snapshot)
	return data
end

--- @param data string
function CWorldSnapshot.deserialize(data)
	-- Utility.assertRuntime()

	local success, collection = pcall(String.bufferDecode, data)
	if not success then
		error("Failed to deserialize!")
	end

	--- @class dr2c.E.CSnapshotDispense
	local e = {
		snapshot = collection,
	}
	TE.events:invoke(eventClientSnapshotDispense, e)
end

--- @param content? table
--- @param snapshotID dr2c.SnapshotID
--- @param callback fun(snapshotID: dr2c.SnapshotID, success: boolean, snapshotData?: dr2c.SnapshotData)
local function requestRPCCallback(content, snapshotID, callback)
	if content then
		local fields = GNetworkMessageFields.CClientSnapshotRequest
		local snapshotData = content[fields.snapshotData]

		CWorldSnapshot.setSnapshotData(snapshotID, snapshotData)

		if callback then
			callback(snapshotID, true, snapshotData)
		end
	else
		if callback then
			callback(snapshotID, false)
		end
	end
end

--- Request a snapshot from server.
--- 请求服务器的快照信息。
--- @param snapshotID? dr2c.SnapshotID
--- @param callback? fun(snapshotID: dr2c.SnapshotID)
function CWorldSnapshot.request(snapshotID, callback)
	local fields = GNetworkMessageFields.SClientSnapshotRequest

	return CNetworkRPC.sendReliable(GNetworkMessage.Type.ClientSnapshotRequest, {
		[fields.snapshotID] = snapshotID,
	}, nil, requestRPCCallback, nil, snapshotID, callback)
end

--- @param snapshotID dr2c.SnapshotID
function CWorldSnapshot.save(snapshotID)
	local snapshotData = CWorldSnapshot.serialize()
	CWorldSnapshot.setSnapshotData(snapshotID, snapshotData)
end

--- @param snapshotID integer
--- @return boolean
function CWorldSnapshot.load(snapshotID)
	local snapshotData = CWorldSnapshot.getSnapshotData(snapshotID)
	if snapshotData then
		CWorldSnapshot.deserialize(snapshotData)

		return true
	else
		return false
	end
end

TE.events:add(N_("CConnect"), CWorldSnapshot.clear, N_("ResetWorldSnapshot"), "Reset")
TE.events:add(N_("CDisconnect"), CWorldSnapshot.clear, N_("ResetWorldSnapshot"), "Reset")
TE.events:add(N_("CWorldSessionStart"), CWorldSnapshot.clear, N_("ResetWorldSnapshot"), "Reset")
TE.events:add(N_("CWorldSessionFinish"), CWorldSnapshot.clear, N_("ResetWorldSnapshot"), "Reset")

--- @param e dr2c.E.CClientPublicAttribute
TE.events:add(N_("CClientPublicAttribute"), function(e)
	if e.clientID == CNetworkClient.getClientID() then
		CWorldSnapshot.request()
	end
end, "ClientVerified", "Snapshot", GNetworkClient.PublicAttribute.State)

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	if type(e.content) ~= "table" then
		return
	end

	local cFields = GNetworkMessageFields.CServerSnapshotRequest
	if CWorldSession.getSessionID() ~= e.content[cFields.worldSessionID] then
		return
	end

	local snapshotID = e.content[cFields.snapshotID]
	local snapshotData
	if snapshotID then
		snapshotData = CWorldSnapshot.getSnapshotData(snapshotID)
		if not snapshotData then
			return
		end
	else
		snapshotID = CWorldSnapshot.getLastSnapshotID()
		if not snapshotID then
			return
		end

		snapshotData = CWorldSnapshot.getSnapshotData(snapshotID)
		assert(snapshotData)
	end

	local sFields = GNetworkMessageFields.SServerSnapshotRequest
	CNetworkClient.sendReliable(GNetworkMessage.Type.ServerSnapshotRequest, {
		[sFields.snapshotID] = snapshotID,
		[sFields.snapshotData] = snapshotData,
	})
	print("Sent snapshot")
end, "ResponseServerSnapshotRequest", "Response", GNetworkMessage.Type.ServerSnapshotRequest)

TE.events:add(N_("CWorldTickProcess"), function(e)
	-- Save snapshot every tick, for now :)
	CWorldSnapshot.save(GWorldSnapshot.tick2id(e.tick))

	local snapshotLifetime = CWorldSession.getAttribute(GWorldSession.Attribute.DataLifetime)
	if snapshotLifetime then
		local droppedNumber = CWorldSnapshot.dropOldSnapshots(snapshotLifetime)

		if droppedNumber and log.canDebug() then
			log.debug(("Dropped %d old snapshots"):format(droppedNumber))
		end
	end
end, N_("SaveSnapshot"), "Snapshot")

--- @param e dr2c.E.CWorldRollback
TE.events:add(N_("CWorldRollback"), function(e)
	if e.suppressed then
		return
	end

	local tick = e.tick
	local snapshotID = GWorldSnapshot.tick2id(tick)

	if CWorldSnapshot.load(snapshotID) then
		CWorldSnapshot.dropBackward(snapshotID)

		if log.canTrace() then
			log.trace(("Rollback and loaded snapshot, tick: %d, snapshotID: %d"):format(tick, snapshotID))
		end
	else
		e.suppressed = true

		if log.canDebug() then
			log.debug(("Rollback but failed to load snapshot, tick: %d, snapshotID: %d"):format(tick, snapshotID))
		end
	end
end, N_("UpdateSnapshots"), "Snapshot")

return CWorldSnapshot
