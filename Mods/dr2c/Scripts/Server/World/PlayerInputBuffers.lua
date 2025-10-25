--[[
-- @module dr2c.Server.World.PlayerInputBuffers
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("TE.Table")

local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local GWorldPlayerInputBuffers = require("dr2c.Shared.World.PlayerInputBuffers")
local GWorldSession = require("dr2c.Shared.World.Session")
local SNetworkClients = require("dr2c.Server.Network.Clients")
-- local SNetworkPlayers = require("dr2c.Server.Network.Players")
local SNetworkRoom = require("dr2c.Server.Network.Room")
local SNetworkServer = require("dr2c.Server.Network.Server")
local SWorldSession = require("dr2c.Server.World.Session")

--- @class dr2c.SWorldPlayerInputBuffers
local SWorldPlayerInputBuffers = {}

--- @type table<dr2c.NetworkRoomID, dr2c.MPlayerInputBuffers>
local room2PlayerInputBuffers = {}

room2PlayerInputBuffers = persist("roomsPlayerInputBuffers", function()
	return room2PlayerInputBuffers
end)

--- @param roomID dr2c.NetworkRoomID
--- @return boolean
--- @nodiscard
function SWorldPlayerInputBuffers.contains(roomID)
	return not not room2PlayerInputBuffers[roomID]
end

--- @param roomID dr2c.NetworkRoomID?
function SWorldPlayerInputBuffers.clear(roomID)
	if roomID then
		room2PlayerInputBuffers[roomID] = nil
	else
		room2PlayerInputBuffers = {}
	end
end

--- @param playerID dr2c.PlayerID
--- @return boolean
--- @nodiscard
function SWorldPlayerInputBuffers.hasPlayer(roomID, playerID)
	local playerInputBuffers = room2PlayerInputBuffers[roomID]
	if not playerInputBuffers then
		error(("Room %s does not exists"):format(roomID), 2)
	end

	return playerInputBuffers.hasPlayer(playerID)
end

--- @param playerID dr2c.PlayerID
function SWorldPlayerInputBuffers.addPlayer(roomID, playerID)
	local playerInputBuffers = room2PlayerInputBuffers[roomID]
	if not playerInputBuffers then
		error(("Room %s does not exists"):format(roomID), 2)
	end

	return playerInputBuffers.addPlayer(playerID)
end

--- @param playerID dr2c.PlayerID
function SWorldPlayerInputBuffers.removePlayer(roomID, playerID)
	local playerInputBuffers = room2PlayerInputBuffers[roomID]
	if not playerInputBuffers then
		error(("Room %s does not exists"):format(roomID), 2)
	end

	return playerInputBuffers.removePlayer(playerID)
end

function SWorldPlayerInputBuffers.getInputs(roomID, playerID, worldTick)
	local playerInputBuffers = room2PlayerInputBuffers[roomID]
	if not playerInputBuffers then
		error()
	end

	return playerInputBuffers.getInputs(playerID, worldTick)
end

function SWorldPlayerInputBuffers.addInput(roomID, playerID, worldTick, playerInputID, playerInputArg)
	local playerInputBuffers = room2PlayerInputBuffers[roomID]
	if not playerInputBuffers then
		error()
	end

	return playerInputBuffers.addInput(playerID, worldTick, playerInputID, playerInputArg)
end

function SWorldPlayerInputBuffers.setInputs(roomID, playerID, worldTick, tickInputs)
	local playerInputBuffers = room2PlayerInputBuffers[roomID]
	if not playerInputBuffers then
		error()
	end

	return playerInputBuffers.setInputs(playerID, worldTick, tickInputs)
end

function SWorldPlayerInputBuffers.removeInputs(roomID, playerID, worldTick)
	local playerInputBuffers = room2PlayerInputBuffers[roomID]
	if not playerInputBuffers then
		error()
	end

	return playerInputBuffers.removeInputs(playerID, worldTick)
end

function SWorldPlayerInputBuffers.discardInputs(roomID, targetTick)
	local playerInputBuffers = room2PlayerInputBuffers[roomID]
	if not playerInputBuffers then
		error()
	end

	return playerInputBuffers.discardInputs(targetTick)
end

function SWorldPlayerInputBuffers.isArchived(roomID, worldTick)
	local playerInputBuffers = room2PlayerInputBuffers[roomID]
	if not playerInputBuffers then
		error()
	end

	return playerInputBuffers.isArchived(worldTick)
end

--- @param roomID dr2c.NetworkRoomID
--- @return dr2c.PlayersInputsNetworkBuffers networkBuffers
function SWorldPlayerInputBuffers.collectNetworkBuffers(roomID)
	local buffers = room2PlayerInputBuffers[roomID]
	if not buffers then
		error("no room", 2)
	end

	--- @class dr2c.PlayersInputsNetworkBuffer
	--- @field [1] dr2c.WorldTick
	--- @field [2] dr2c.WorldTick
	--- @field [3] (dr2c.PlayerTickInputs | false)[]

	--- @class dr2c.PlayersInputsNetworkBuffers
	--- @field [dr2c.PlayerID] dr2c.PlayersInputsNetworkBuffer
	local networkBuffers = {}

	--- @param playerID dr2c.PlayerID
	--- @param playerTicksInputs dr2c.PlayerTicksInputs
	for playerID, playerTicksInputs in pairs(buffers.getData()) do
		local beginTick = playerTicksInputs.beginTick
		if beginTick then
			local endTick = playerTicksInputs.endTick

			local tickInputsList = {}
			for tick = beginTick, endTick do
				tickInputsList[#tickInputsList + 1] = playerTicksInputs[tick] or false
			end

			networkBuffers[playerID] = { beginTick, endTick, tickInputsList }
		end
	end

	return networkBuffers
end

--- @param e dr2c.E.SCreateRoom
TE.events:add(N_("SCreateRoom"), function(e)
	room2PlayerInputBuffers[e.roomID] = GWorldPlayerInputBuffers.new(e.roomID)
end, "CreatePlayerInputBuffer", "PlayerInputBuffers")

--- @param e dr2c.E.SDestroyRoom
TE.events:add(N_("SDestroyRoom"), function(e)
	room2PlayerInputBuffers[e.roomID] = nil
end, "DestroyPlayerInputBuffer", "PlayerInputBuffers")

--- @param e dr2c.E.SMessage
TE.events:add(N_("SMessage"), function(e)
	if type(e.content) ~= "table" then
		return
	end

	local clientID = e.clientID

	local sFields = GNetworkMessageFields.SPlayerInputs

	local roomID = SNetworkClients.getRoomID(clientID)
	if not (roomID and SNetworkRoom.hasRoom(roomID)) then
		if log.canDebug() then
			log.debug(
				("Received player inputs from client %d, ignoring because client's room %s does not exists"):format(
					clientID,
					roomID
				)
			)
		end

		return
	end

	local sessionID = SWorldSession.getSessionID(roomID)
	if sessionID ~= e.content[sFields.worldSessionID] then
		if log.canDebug() then
			log.debug(
				("Received player inputs from client %d, ignoring because world session id doesn't match: server=%s, client=%s"):format(
					clientID,
					sessionID,
					e.content[sFields.worldSessionID]
				)
			)
		end

		return
	end

	local playerID = e.content[sFields.playerID]
	if not SWorldPlayerInputBuffers.hasPlayer(roomID, playerID) then
		if log.canDebug() then
			log.debug(
				("Received player inputs from client %d, ignoring because player %s does not exists or owned by client"):format(
					clientID,
					playerID
				)
			)
		end

		return
	end

	local worldTick = e.content[sFields.worldTick]
	local playerInputs = e.content[sFields.playerInputs]

	SWorldPlayerInputBuffers.setInputs(roomID, playerID, worldTick, playerInputs)

	local cFields = GNetworkMessageFields.CPlayerInputs

	SNetworkRoom.sendReliable(roomID, GNetworkMessage.Type.CPlayerInputs, {
		[cFields.worldSessionID] = sessionID,
		[cFields.worldTick] = worldTick,
		[cFields.playerID] = playerID,
		[cFields.playerInputs] = playerInputs,
	})
end, "ReceiveAndSendPlayerInput", "Receive", GNetworkMessage.Type.SPlayerInputs)

--- @param e dr2c.E.SClientChangeRoom
TE.events:add(N_("SClientChangeRoom"), function(e)
	local playerID = e.clientID

	if log.canDebug() then
		log.debug(
			("Player %d changed room, remove from room %s, add to room %s"):format(playerID, e.fromRoomID, e.toRoomID)
		)
	end

	if e.fromRoomID then
		SWorldPlayerInputBuffers.removePlayer(e.fromRoomID, playerID)
	end
	SWorldPlayerInputBuffers.addPlayer(e.toRoomID, playerID)
end, "AddPlayerToPlayerInputBuffers", "PlayerInputBuffers")

--- @param e dr2c.E.SClientVerified
TE.events:add(N_("SClientVerified"), function(e)
	local clientID = e.clientID
	local roomID = SNetworkClients.getRoomID(clientID)
	if not (roomID and SWorldPlayerInputBuffers.contains(roomID)) then
		if log.canWarn() then
			log.warn(("Client %d is supposed to be in a room"):format(clientID))
		end

		return
	end

	local fields = GNetworkMessageFields.CPlayerInputBuffers

	SNetworkServer.sendReliable(e.clientID, GNetworkMessage.Type.CPlayerInputBuffers, {
		[fields.worldSessionID] = SWorldSession.getSessionID(roomID),
		[fields.buffer] = SWorldPlayerInputBuffers.collectNetworkBuffers(roomID),
	})
end, "SendPlayerInputNetworkBuffers", "PlayerInputBuffers")

--- @param e dr2c.E.SWorldSessionStart
TE.events:add(N_("SWorldSessionStart"), function(e)
	-- SPlayerInputBuffers.contains()
end, "ClearPlayerInputBuffers", "Reset")

--- @param e dr2c.E.SWorldSessionFinish
TE.events:add(N_("SWorldSessionFinish"), function(e)
	-- SPlayerInputBuffers.contains(e.attributes)
end, "ClearPlayerInputBuffers", "Reset")

return SWorldPlayerInputBuffers
