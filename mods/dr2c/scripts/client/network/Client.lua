local String = require("tudov.String")
local Enum = require("tudov.Enum")

local GMessage = require("dr2c.shared.network.Message")

local network = network

--- @class dr2c.CClient
local CClient = {}

--- @type integer
CClient.sessionSlot = 0

CClient.eventClientConnect = events:new(N_("CConnect"), {
	"Initialize",
})

CClient.eventClientDisconnect = events:new(N_("CDisconnect"), {
	"Reset",
})

CClient.eventClientMessage = events:new(N_("CMessage"), {
	"Overrides",
	"Receive",
}, Enum.eventKeys(GMessage.Type))

--- @return Network.Client
function CClient.getNetworkClient()
	return network:getClient(CClient.sessionSlot)
end

--- @return Network.ClientID
function CClient.getClientID()
	return CClient.getNetworkClient():getSessionID()
end

local function invokeEventClientConnect()
	--- @class dr2c.E.ClientConnect
	local e = {}

	events:invoke(CClient.eventClientConnect, e)
end

--- @param e Events.E.ClientConnect
events:add("ClientConnect", function(e)
	invokeEventClientConnect()
end)

local function invokeEventClientDisconnect()
	--- @class dr2c.E.ClientDisconnect
	local e = {}

	events:invoke(CClient.eventClientDisconnect, e)
end

--- @param e Events.E.ClientConnect
events:add("ClientDisconnect", function(e)
	invokeEventClientDisconnect()
end)

local function invokeEventClientMessage(messageContent, messageType)
	--- @class dr2c.E.ClientMessage
	local e = {
		content = messageContent,
		type = messageType,
	}

	events:invoke(CClient.eventClientMessage, e, messageType)
end

--- @param e Events.E.ClientMessage
events:add("ClientMessage", function(e)
	if type(e.data) ~= "userdata" then
		return
	end

	local messageType, messageContent = GMessage.unpack(e.data.message)

	invokeEventClientMessage(messageContent, messageType)
end, "clientMessage", nil, CClient.sessionSlot)

return CClient
