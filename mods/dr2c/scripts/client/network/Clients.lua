local String = require("tudov.String")
local Enum = require("tudov.Enum")

local CNetwork = require("dr2c.client.misc.Network")
local GMessage = require("dr2c.shared.network.Message")

--- @class dr2c.CClients
local CClients = {}

CClients.SessionSlot = 0

local clientsPublicAttributes = {}
local clientsPrivateAttributes = {}

function CClients.getPublicAttribute(clientPublicAttribute)
	local attribute = clientsPublicAttributes[clientPublicAttribute]
end

function CClients.requestPrivateAttribute(clientPrivateAttribute, callback)
	--
end

--- @return Network.ClientID
function CClients.getLocalClientID()
	return network:getClient(CClients.SessionSlot):getSessionID()
end

-- local function invokeEventClientConnect()
-- 	local e = {
-- 		type = "connect",
-- 		clientID = CClients.getLocalClientID(),
-- 	}
-- 	events:invoke(CNetwork.eventClientConnect, e)
-- end

events:add("ClientConnect", function(e) end)

local function invokeEventClientMessage(messageContent, messageType)
	local e = {
		content = messageContent,
		type = messageType,
	}
	events:invoke(CNetwork.eventClientMessage, e, messageType)
end

--- @param e Events.E.LocalClientMessage
events:add("ClientMessage", function(e)
	local messageType, messageContent = GMessage.unpack(e.data)

	invokeEventClientMessage(messageContent, messageType)

	-- print("CClients.getLocalClientID()", CClients.getLocalClientID())
	-- print("e.data", e.data)
	-- print("e.data.socketType", e.data.socketType)
	-- print("e.data.message", e.data.message)
	-- print("e.data.host", e.data.host)
	-- print("e.data.port", e.data.port)

	-- events:invoke(CNetworkClient.eventClientMessage, e, key?, options?)
end, "", nil, CClients.SessionSlot)

return CClients
