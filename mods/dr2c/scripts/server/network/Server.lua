local GMessage = require("dr2c.shared.network.Message")

--- @class dr2c.SServer
local SServer = {}

SServer.sessionSlot = 0

SServer.eventServerConnect = events:new(N_("SConnect"), {
	"Clients",
})

SServer.eventServerDisconnect = events:new(N_("SDisconnect"), {
	"Clients",
	"Reset",
})

SServer.eventServerMessage = events:new(N_("SMessage"), {
	"Overrides",
	"Receive",
})

--- @param clientID Network.ClientID
local function invokeEventServerConnect(clientID)
	--- @class dr2c.E.ServerConnect
	local e = {
		clientID = clientID,
	}

	events:invoke(SServer.eventServerConnect, e)
end

--- @param e Events.E.ServerMessage
events:add("ServerConnect", function(e)
	invokeEventServerConnect(e.data.clientID)
end)

--- @param clientID Network.ClientID
local function invokeEventServerDisconnect(clientID)
	--- @class dr2c.E.ServerDisconnect
	local e = {
		clientID = clientID,
	}

	events:invoke(SServer.eventServerDisconnect, e, nil, EEventInvocation.None)
end

--- @param e Events.E.ServerDisconnect
events:add("ServerDisconnect", function(e)
	invokeEventServerDisconnect(e.data.clientID)
end)

--- @param messageContent any?
--- @param messageType dr2c.MessageType
local function invokeEventServerMessage(messageContent, messageType)
	--- @class dr2c.E.ServerMessage
	local e = {
		content = messageContent,
		type = messageType,
	}

	events:invoke(SServer.eventServerMessage, e, messageType)
end

--- @param e Events.E.ServerMessage
events:add("ServerMessage", function(e)
	if type(e.data) ~= "userdata" then
		return
	end

	local messageType, messageContent = GMessage.unpack(e.data)

	invokeEventServerMessage(messageContent, messageType)
end, "ServerMessage", nil, SServer.sessionSlot)

return SServer
