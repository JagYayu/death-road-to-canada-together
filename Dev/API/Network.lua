--- @meta
error("this is a lua library module")

--- @class TE.Network.ClientID : integer

--- @class Network.ServerID : integer

--- @enum TE.EClientSessionState
EClientSessionState = {
	Disconnected = 0,
	Connecting = 1,
	Connected = 2,
	Disconnecting = 3,
}

--- @enum TE.EServerSessionState
EServerSessionState = {
	Shutdown = 0,
	Starting = 1,
	Hosting = 2,
	Stopping = 3,
}

--- @enum TE.EDisconnectionCode
EDisconnectionCode = {
	Unknown = 0,
	ClientClosed = 1,
	ServerClosed = 2,
}

--- @enum TE.ESocketType
ESocketType = {
	Local = 0,
	RUDP = 1,
	Steam = 2,
	TCP = 3,
}

--- @class TE.Network
local network = {}

--- @class TE.Network.ClientConnectRUDPArgs
--- @field host string
--- @field port integer
--- @field slot integer?

--- @param args TE.Network.ClientConnectRUDPArgs
function network:clientConnectRUDP(args) end

--- @class TE.Network.ClientDisconnectArgs
--- @field slot integer

--- @param args TE.Network.ClientDisconnectArgs
function network:clientDisconnect(args) end

--- @param clientSessionSlot integer?
--- @return TE.Network.Client
function network:getClient(clientSessionSlot) end

--- @param serverSessionSlot integer?
--- @return TE.Network.Server?
function network:getServer(serverSessionSlot) end

--- @class TE.Network.ServerHostRUDPArgs
--- @field host string
--- @field port integer
--- @field slot integer?

--- @param args TE.Network.ServerHostRUDPArgs
function network:serverHostRUDP(args) end

--- @class TE.Network.ServerShutdownArgs
--- @field slot integer

--- @param args TE.Network.ServerShutdownArgs
function network:serverShutdown(args) end

--- @return boolean updated
function network:update() end

--- @class TE.Network.ClientConnectArgs
--- @field password string

--- @class TE.Network.Client
local client = {}

--- @return TE.Network.ClientID
function client:getSessionID() end

--- @return TE.EClientSessionState
function client:getSessionState() end

--- @return TE.ESocketType
function client:getSocketType() end

--- @param args TE.Network.ClientConnectArgs
function client:connect(args) end

--- @param code TE.EDisconnectionCode
function client:disconnect(code) end

--- @param code TE.EDisconnectionCode
function client:tryDisconnect(code) end

--- @param data string
--- @param channel integer
function client:sendReliable(data, channel) end

--- @param data string
--- @param channel integer
function client:sendUnreliable(data, channel) end

--- @class TE.Network.ServerHostArgs
--- @field title string
--- @field password string
--- @field maximumClients integer

--- @class TE.Network.Server
local server = {}

function server:connect() end

--- @param clientID TE.Network.ClientID
--- @param code TE.EDisconnectionCode
function server:disconnect(clientID, code) end

--- @return integer
function server:getClients() end

--- @return integer
function server:getMaxClients() end

--- @return string
function server:getPassword() end

--- @return Network.ServerID
function server:getSessionID() end

--- @return TE.EServerSessionState
function server:getSessionState() end

--- @return TE.ESocketType
function server:getSocketType() end

--- @return string
function server:getTitle() end

--- @param args TE.Network.ServerHostArgs
function server:host(args) end

function server:shutdown() end

--- @param clientID TE.Network.ClientID
--- @param data string
--- @param channel integer
function server:sendReliable(clientID, data, channel) end

--- @param clientID TE.Network.ClientID
--- @param data string
--- @param channel integer
function server:sendUnreliable(clientID, data, channel) end

--- @param data string
--- @param channel integer
function server:broadcastReliable(data, channel) end

--- @param data string
--- @param channel integer
function server:broadcastUnreliable(data, channel) end

TE.network = network
