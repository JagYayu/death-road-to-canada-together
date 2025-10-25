--[[
-- @module dr2c.Client.World.CWorldSynchronizer
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class dr2c.CWorldSynchronizer
local CWorldSynchronizer = {}

local CNetworkClient = require("dr2c.Client.Network.Client")
local CNetworkClients = require("dr2c.Client.Network.Clients")
local CNetworkClientUtils = require("dr2c.Client.Network.ClientUtils")
local CNetworkRPC = require("dr2c.Client.Network.RPC")
local GNetworkMessage = require("dr2c.Shared.Network.Message")

--- @type boolean
local synchronized = false
--- @type dr2c.NetworkRemoteRequestHandle[]?
local synchronizing

synchronized = persist("synchronized", function()
	return synchronized
end)
synchronizing = persist("synchronizing", function()
	return synchronizing
end)

function CWorldSynchronizer.resetState()
	synchronized = false

	if synchronizing then
		for _, requestHandle in ipairs(synchronizing) do
			CNetworkRPC.removeRequest(requestHandle)
		end

		synchronizing = nil
	end
end

--- @return boolean
--- @nodiscard
function CWorldSynchronizer.isSynchronized()
	if not CNetworkClient.getClientID() then
		return true
	end

	if CNetworkClientUtils.hasPermissionAuthority() then
		return true
	end

	return false
end

--- @return boolean
--- @nodiscard
function CWorldSynchronizer.isSynchronizing()
	return not not synchronizing
end

TE.events:add(N_("CUpdate"), function(e)
	-- if CWorldSynchronizer.isSynchronized() or synchronizing then
	-- 	return
	-- end

	-- log.info("Game state is unsynced, requesting remote snapshots and input buffers...")

	-- local snapshotRequestHandle = CNetworkRPC.sendReliable(
	-- 	GNetworkMessage.Type.ClientSnapshotRequest,
	-- 	nil,
	-- 	nil,
	-- 	function(messageContent, ...)
	-- 		synchronizing = nil
	-- 	end
	-- )
	-- assert(snapshotRequestHandle)

	-- -- ? Do we need to sync player input buffer?

	-- synchronizing = { snapshotRequestHandle }
end, "Synchronize", "Synchronizer")

TE.events:add(N_("CConnect"), CWorldSynchronizer.resetState, "ResetSynchronizerState", "Reset")
TE.events:add(N_("CDisconnect"), CWorldSynchronizer.resetState, "ResetSynchronizerState", "Reset")

return CWorldSynchronizer
