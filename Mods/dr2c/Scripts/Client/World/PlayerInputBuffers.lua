--[[
-- @module dr2c.Client.World.PlayerInputBuffers
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

local CWorldSession = require("dr2c.Client.World.Session")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local GPlayerInput = require("dr2c.Shared.World.PlayerInput")
local GPlayerInputBuffers = require("dr2c.Shared.World.PlayerInputBuffers")
local GWorldSession = require("dr2c.Shared.World.Session")

--- @class dr2c.CPlayerInputBuffers : dr2c.PlayerInputBuffers
local CPlayerInputBuffers = GPlayerInputBuffers.new()

local CPlayerInputBuffers_getInputs = CPlayerInputBuffers.getInputs

--- @type dr2c.PlayersTicksInputs
local playersTicksInputsPredictions = {}

do
	local addPlayer = CPlayerInputBuffers.addPlayer
	local removePlayer = CPlayerInputBuffers.removePlayer

	function CPlayerInputBuffers.addPlayer(playerID)
		addPlayer(playerID)

		playersTicksInputsPredictions[playerID] = {}
	end

	function CPlayerInputBuffers.removePlayer(playerID)
		removePlayer(playerID)

		playersTicksInputsPredictions[playerID] = nil
	end
end

--- @return dr2c.PlayersTicksInputs
function CPlayerInputBuffers.getPredictions()
	return playersTicksInputsPredictions
end

--- @param playerID dr2c.PlayerID
--- @param worldTick dr2c.WorldTick
--- @param distance integer
--- @return dr2c.PlayerTickInputs
local function getOrPredictInputs(playerID, worldTick, distance)
	local tickInputs = CPlayerInputBuffers_getInputs(playerID, worldTick)
	if tickInputs then
		return tickInputs
	end

	local previousTickInputs = CPlayerInputBuffers_getInputs(playerID, worldTick)
	if not previousTickInputs then
		if distance > 30 then
			return {}
		end

		previousTickInputs = getOrPredictInputs(playerID, worldTick - 1, distance + 1)
	end

	return { List.copy(previousTickInputs[1]) }
end

--- 获取玩家在某一世界刻的所有输入，若没有则返回预测值
--- @param playerID dr2c.PlayerID
--- @param worldTick dr2c.WorldTick
--- @return dr2c.PlayerTickInputs?
function CPlayerInputBuffers.getOrPredictInputs(playerID, worldTick)
	return getOrPredictInputs(playerID, worldTick, 0)
end

--- @param ticksInputs dr2c.PlayerTicksInputs
--- @param beginWorldTick dr2c.WorldTick
--- @param endWorldTick dr2c.WorldTick
--- @return table<dr2c.WorldTick, dr2c.PlayerInputCollectedEntry>
local function collectInputsInRangeImpl(ticksInputs, beginWorldTick, endWorldTick)
	local collectedInputs = Table.new(endWorldTick - beginWorldTick + 1, 0)

	--- @type dr2c.PlayerTickInputs
	local prevInputsData = {}

	for worldTick = beginWorldTick, endWorldTick do
		local tickInputs = ticksInputs[worldTick]
		if tickInputs then
			prevInputsData = tickInputs

			collectedInputs[worldTick] = {
				deterministic = true,
				map = tickInputs[1] or Table.empty,
				list = tickInputs[2] or Table.empty,
			}
		else -- Predict continuous inputs
			tickInputs = {}
			local prevContinuousSet = prevInputsData[GPlayerInput.Type.Continuous]

			if prevContinuousSet then
				local set = {}
				tickInputs[GPlayerInput.Type.Continuous] = set

				for key, value in pairs(prevContinuousSet) do
					set[key] = value
				end
			end

			collectedInputs[worldTick] = {
				deterministic = false,
				map = tickInputs[1] or Table.empty,
				list = tickInputs[2] or Table.empty,
			}
		end
	end

	return collectedInputs
end

--- 收集一个玩家在两个时刻范围内的输入
--- @param playerID dr2c.PlayerID
--- @param beginWorldTick dr2c.WorldTick
--- @param endWorldTick dr2c.WorldTick
--- @return table<dr2c.WorldTick, dr2c.PlayerInputCollectedEntry>
--- @nodiscard
function CPlayerInputBuffers.collectPlayerInputsInRange(playerID, beginWorldTick, endWorldTick)
	local ticksInputsData = CPlayerInputBuffers.getData()[playerID]
	if ticksInputsData then
		return Table.empty
	end

	return collectInputsInRangeImpl(ticksInputsData, beginWorldTick, endWorldTick)
end

--- 收集所有玩家在两个时刻范围内的输入
--- @param beginWorldTick dr2c.WorldTick
--- @param endWorldTick dr2c.WorldTick
--- @return table<dr2c.PlayerID, table<dr2c.WorldTick, dr2c.PlayerInputCollectedEntry>>
--- @nodiscard
function CPlayerInputBuffers.collectPlayersInputsInRange(beginWorldTick, endWorldTick)
	local playersInputs = {}

	for playerID, playerTicksInputs in pairs(CPlayerInputBuffers.getData()) do
		if type(playerTicksInputs) == "table" then
			playersInputs[playerID] = collectInputsInRangeImpl(playerTicksInputs, beginWorldTick, endWorldTick)
		end
	end

	return playersInputs
end

--- @param tick dr2c.WorldTick
--- @param targetTick dr2c.WorldTick
--- @return table<dr2c.WorldTick, dr2c.PlayerInputCollectedEntry>
function CPlayerInputBuffers.makePlayerInputsArg(tick, targetTick)
	local playersInputs = CPlayerInputBuffers.collectPlayersInputsInRange(tick, targetTick)
	local playerIDs = Table.getKeyList(playersInputs)

	local playerInputs = Table.new(0, #playerIDs)
	for _, playerID in ipairs(playerIDs) do
		playerInputs[playerID] = playersInputs[playerID][tick]
	end

	return playerInputs
end

TE.events:add(N_("CUpdate"), function(e)
	local inputsLifetime = CWorldSession.getAttribute(GWorldSession.Attribute.DataLifetime)
	if inputsLifetime then
		CPlayerInputBuffers.removeOldInputs(inputsLifetime)
	end
end, "RemoveOldInputsFromPlayerInputBuffers", "ClearCaches")

--- @param e dr2c.E.CWorldTickProcess
TE.events:add(N_("CWorldTickProcess"), function(e)
	--- @class dr2c.E.CWorldTickProcess
	--- @field playerInputs? table<dr2c.WorldTick, dr2c.PlayerInputCollectedEntry>
	e = e

	if not e.playerInputs then
		e.playerInputs = CPlayerInputBuffers.makePlayerInputsArg(e.tick, e.targetTick)
	end
end, "ReadPlayerInputs", "PlayerInputs")

--- @param e dr2c.E.CClientAdded
TE.events:add(N_("CClientAdded"), function(e)
	CPlayerInputBuffers.addPlayer(e.clientID)
end, "AddPlayerToPlayerInputBuffers", "PlayerInputBuffer")

--- @param e dr2c.E.CMessage
TE.events:add(N_("CMessage"), function(e)
	local fields = GNetworkMessageFields.PlayerInputs
	local playerID = e.content[fields.playerID]

	if CPlayerInputBuffers.hasPlayer(playerID) then
		local playerInputs = e.content[fields.playerInputs]
		local worldTick = e.content[fields.worldTick]

		if log.canTrace() then
			log.trace(("Received player %s inputs at tick %s"):format(playerID, worldTick))
		end

		CPlayerInputBuffers.setInputs(playerID, worldTick, playerInputs)
	end
end, "ReceivePlayerInput", "Receive", GNetworkMessage.Type.PlayerInputs)

TE.events:add(N_("CConnect"), function(e)
	CPlayerInputBuffers.reset()
end, "ClearPlayerInputBuffers", "Initialize")

TE.events:add(N_("CDisconnect"), CPlayerInputBuffers.reset, "ClearPlayerInputBuffers", "Reset")

return CPlayerInputBuffers
