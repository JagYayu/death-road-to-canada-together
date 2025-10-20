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
local GWorldTick = require("dr2c.Shared.World.Tick")

local Table_empty = Table.empty

--- @class dr2c.CPlayerInputBuffers : dr2c.PlayerInputBuffers
local CPlayerInputBuffers = GPlayerInputBuffers.new()

--- @class dr2c.PlayerInputCollectedEntry
--- @field deterministic boolean
--- @field map table<dr2c.PlayerInputID, Serializable>
--- @field list { [1]: dr2c.PlayerInputID, [2]: Serializable }[]

local CPlayerInputBuffers_getInputs = CPlayerInputBuffers.getInputs

--- @type table<dr2c.PlayerID, table<dr2c.WorldTick, dr2c.PlayerTickInputs>>
local playersTicksInputsPredictions = {}

playersTicksInputsPredictions = persist("playersTicksInputsPredictions", function()
	return playersTicksInputsPredictions
end)

do
	local clear = CPlayerInputBuffers.clear
	local addPlayer = CPlayerInputBuffers.addPlayer
	local removePlayer = CPlayerInputBuffers.removePlayer

	function CPlayerInputBuffers.clear()
		clear()

		for playerID in pairs(playersTicksInputsPredictions) do
			playersTicksInputsPredictions[playerID] = {}
		end
	end

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
--- @return boolean
--- @return dr2c.PlayerTickInputs
--- @nodiscard
local function getOrPredictInputs(playerID, worldTick, distance)
	local tickInputs = CPlayerInputBuffers_getInputs(playerID, worldTick)
	if tickInputs then
		return true, tickInputs
	end

	tickInputs = playersTicksInputsPredictions[playerID][worldTick]
	if tickInputs then
		return false, tickInputs
	end

	if distance > 30 then
		playersTicksInputsPredictions[playerID][worldTick] = tickInputs
		return false, {}
	end

	local previousTickInputs
	if worldTick > 1 then
		previousTickInputs = CPlayerInputBuffers_getInputs(playerID, worldTick - 1)
	else
		previousTickInputs = Table_empty
	end

	--- 这个值应该总是`false`
	local deterministic = false
	if not previousTickInputs then
		if worldTick > 1 then
			deterministic, previousTickInputs = getOrPredictInputs(playerID, worldTick - 1, distance + 1)
		else
			previousTickInputs = Table_empty
		end
	end

	local continuousInputs = previousTickInputs[1]
	tickInputs = continuousInputs and { Table.copy(continuousInputs) } or {}

	playersTicksInputsPredictions[playerID][worldTick] = tickInputs
	return deterministic, tickInputs
end

--- 获取玩家在某一世界刻的所有输入，若没有则返回预测值
--- @param playerID dr2c.PlayerID
--- @param worldTick dr2c.WorldTick
--- @return boolean deterministic
--- @return dr2c.PlayerTickInputs tickInputs
--- @nodiscard
function CPlayerInputBuffers.getOrPredictInputs(playerID, worldTick)
	return getOrPredictInputs(playerID, worldTick, 0)
end

--- @param playerID dr2c.PlayerID
--- @param beginWorldTick dr2c.WorldTick
--- @param endWorldTick dr2c.WorldTick
--- @return table<dr2c.WorldTick, dr2c.PlayerInputCollectedEntry>
local function collectInputsInRangeImpl(playerID, beginWorldTick, endWorldTick)
	local collectedInputs = Table.new(endWorldTick - beginWorldTick + 1, 0)

	for worldTick = beginWorldTick, endWorldTick do
		local deterministic, tickInputs = CPlayerInputBuffers.getOrPredictInputs(playerID, worldTick)
		collectedInputs[worldTick] = {
			deterministic = deterministic,
			map = tickInputs[1] or Table_empty,
			list = tickInputs[2] or Table_empty,
		}
	end

	return collectedInputs
end

--- 收集一个玩家在两个时刻范围内的所有输入，含预测的输入
--- @param playerID dr2c.PlayerID
--- @param beginWorldTick dr2c.WorldTick
--- @param endWorldTick dr2c.WorldTick
--- @return table<dr2c.WorldTick, dr2c.PlayerInputCollectedEntry>
--- @nodiscard
function CPlayerInputBuffers.collectPlayerInputsInRange(playerID, beginWorldTick, endWorldTick)
	local ticksInputsData = CPlayerInputBuffers.getData()[playerID]
	if ticksInputsData then
		return Table_empty
	end

	return collectInputsInRangeImpl(ticksInputsData, beginWorldTick, endWorldTick)
end

--- 收集所有玩家在两个时刻范围内的输入，含预测的输入
--- @param beginWorldTick dr2c.WorldTick
--- @param endWorldTick dr2c.WorldTick
--- @return table<dr2c.PlayerID, table<dr2c.WorldTick, dr2c.PlayerInputCollectedEntry>>
--- @nodiscard
function CPlayerInputBuffers.collectPlayersInputsInRange(beginWorldTick, endWorldTick)
	local playersInputs = {}

	for playerID in pairs(CPlayerInputBuffers.getData()) do
		playersInputs[playerID] = collectInputsInRangeImpl(playerID, beginWorldTick, endWorldTick)
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

--- @param inputs dr2c.PlayerTickInputs
--- @param predictedInputs dr2c.PlayerTickInputs
local function isMatchPrediction(inputs, predictedInputs)
	-- 只需比较数组部分（离散输入）与哈希部分（连续输入）是否一致
	return List.equals(inputs[2] or Table_empty, predictedInputs[2] or Table_empty)
		and Table.equals(inputs[1] or Table_empty, predictedInputs[1] or Table_empty)
end

CPlayerInputBuffers.eventCPlayerInputsPredictFailed = TE.events:new(N_("CPlayerInputsPredictFailed"), {
	"Rollback",
})

--- @param playerID dr2c.PlayerID
--- @param worldTick dr2c.WorldTick
--- @param inputs dr2c.PlayerTickInputs
--- @param predictedInputs dr2c.PlayerTickInputs
local function onPredictFailed(playerID, worldTick, inputs, predictedInputs)
	--- @class dr2c.E.CPlayerInputsPredictFailed
	--- @field playerID dr2c.PlayerID
	--- @field worldTick dr2c.WorldTick
	--- @field inputs dr2c.PlayerTickInputs
	--- @field predictedInputs dr2c.PlayerTickInputs
	local e = {
		playerID = playerID,
		worldTick = worldTick,
		inputs = inputs,
		predictedInputs = predictedInputs,
	}

	TE.events:invoke(CPlayerInputBuffers.eventCPlayerInputsPredictFailed, e)
end

TE.events:add(N_("CUpdate"), function(e)
	local inputsLifetime = tonumber(CWorldSession.getAttribute(GWorldSession.Attribute.DataLifetime))
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

	if not CPlayerInputBuffers.hasPlayer(playerID) then
		return
	end

	local playerInputs = e.content[fields.playerInputs]
	local worldTick = e.content[fields.worldTick]

	if log.canTrace() then
		log.trace(("Received player %s inputs at tick %s"):format(playerID, worldTick))
	end

	local deterministic, predictedInputs = CPlayerInputBuffers.getOrPredictInputs(playerID, worldTick)

	CPlayerInputBuffers.setInputs(playerID, worldTick, playerInputs)

	if not deterministic and not isMatchPrediction(playerInputs, predictedInputs) then
		onPredictFailed(playerID, worldTick, playerInputs, predictedInputs)
	end
end, "ReceivePlayerInput", "Receive", GNetworkMessage.Type.PlayerInputs)

TE.events:add(N_("CConnect"), CPlayerInputBuffers.clear, "ClearPlayerInputBuffers", "Reset")
TE.events:add(N_("CDisconnect"), CPlayerInputBuffers.clear, "ClearPlayerInputBuffers", "Reset")

TE.events:add("DebugSnapshot", function(e)
	e.playersTicksInputsPredictions = playersTicksInputsPredictions
end, scriptName, nil, scriptName)

return CPlayerInputBuffers
