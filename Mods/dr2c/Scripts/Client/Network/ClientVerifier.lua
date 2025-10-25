--[[
-- @module dr2c.Client.Network.ClientVerifier
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Enum = require("TE.Enum")
local List = require("TE.List")
local Table = require("TE.Table")

local CNetworkClient = require("dr2c.Client.Network.Client")
local CNetworkClients = require("dr2c.Client.Network.Clients")
local GNetworkClient = require("dr2c.Shared.Network.Client")
local GNetworkMessage = require("dr2c.Shared.Network.Message")
local GNetworkMessageFields = require("dr2c.Shared.Network.MessageFields")
local GUtilsThrottle = require("dr2c.Shared.Utils.Throttle")

--- @class dr2c.SNetworkClientVerifier
local CNetworkClientVerifier = {}

CNetworkClientVerifier.eventCCollectVerifyAttributes = TE.events:new(N_("CCollectVerifyAttributes"), {
	"Public",
	"Private",
	"State",
})

TE.events:add(CNetworkClientVerifier.eventCCollectVerifyAttributes, function(e)
	e.attributes[#e.attributes + 1] = {
		isPublicOrPrivate = true,
		attribute = GNetworkClient.PublicAttribute.State,
		value = GNetworkClient.State.Verified,
	}
end, "SendState", "State")

local function invokeEventClientCollectVerifyAttributes()
	local attributes = {}

	--- @class dr2c.E.ClientCollectVerifyAttributes
	local e = {
		attributes = attributes,
	}

	TE.events:invoke(CNetworkClientVerifier.eventCCollectVerifyAttributes, e)

	return attributes
end

function CNetworkClientVerifier.sendVerifyingAttributes()
	local entries = invokeEventClientCollectVerifyAttributes()

	for _, entry in ipairs(entries) do
		if entry.isPublicOrPrivate then
			local fields = GNetworkMessageFields.SClientSetPublicAttribute
			CNetworkClient.sendReliable(GNetworkMessage.Type.SClientSetPublicAttribute, {
				[fields.attribute] = entry.attribute,
				[fields.value] = entry.value,
			})
		else
			local fields = GNetworkMessageFields.SClientSetPrivateAttribute
			CNetworkClient.sendReliable(GNetworkMessage.Type.SClientSetPrivateAttribute, {
				[fields.attribute] = entry.attribute,
				[fields.value] = entry.value,
			})
		end
	end

	return entries
end


local sendClientVerifyingAttributesTime = 0

--- @param e dr2c.E.CUpdate
TE.events:add(N_("CUpdate"), function(e)
	if e.clientNetworkThrottle then
		return
	end

	local clientID = CNetworkClient.getClientID()
	if not clientID then
		return
	end

	local time = Time.getSystemTime()
	if time < sendClientVerifyingAttributesTime then
		return
	end

	if
		CNetworkClients.getPublicAttribute(clientID, GNetworkClient.PublicAttribute.State)
		~= GNetworkClient.State.Verifying
	then
		return
	end

	local attributes = CNetworkClientVerifier.sendVerifyingAttributes()

	if log.canTrace() then
		log.trace("Sent verifying attributes: ", attributes)
	end

	sendClientVerifyingAttributesTime = time + 5
end, "SendClientVerifyingAttributes", "Network")

return CNetworkClientVerifier
