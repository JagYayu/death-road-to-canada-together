--- @meta
error("this is a lua library module")

--- @class Network.ClientID : integer

--- @class Network.ServerID : integer

--- @enum EClientSessionState
EClientSessionState = {
	Disconnected = 0,
	Connecting = 1,
	Connected = 2,
	Disconnecting = 3,
}

--- @enum EServerSessionState
EServerSessionState = {
	Shutdown = 0,
	Starting = 1,
	Hosting = 2,
	Stopping = 3,
}

--- @enum EDisconnectionCode
EDisconnectionCode = {
	Unknown = 0,
	ClientClosed = 1,
	ServerClosed = 2,
}

--- @enum ESocketType
ESocketType = {
	Local = 0,
	RUDP = 1,
	Steam = 2,
	TCP = 3,
}

--- @class Network
local network = {}

--- @class Network.ClientConnectRUDPArgs
--- @field host string
--- @field port integer
--- @field slot integer?

--- @param args Network.ClientConnectRUDPArgs
function network:clientConnectRUDP(args) end

--- @class Network.ClientDisconnectArgs
--- @field slot integer

--- @param args Network.ClientDisconnectArgs
function network:clientDisconnect(args) end

--- @param clientSessionSlot integer?
--- @return Network.Client
function network:getClient(clientSessionSlot) end

--- @param serverSessionSlot integer?
--- @return Network.Server?
function network:getServer(serverSessionSlot) end

--- @class Network.ServerHostRUDPArgs
--- @field host string
--- @field port integer
--- @field slot integer?

--- @param args Network.ServerHostRUDPArgs
function network:serverHostRUDP(args) end

--- @class Network.ServerShutdownArgs
--- @field slot integer

--- @param args Network.ServerShutdownArgs
function network:serverShutdown(args) end

--- @return boolean updated
function network:update() end

--- @class Network.ClientConnectArgs
--- @field password string

--- @class Network.Client
local client = {}

--- @return Network.ClientID
function client:getSessionID() end

--- @return EClientSessionState
function client:getSessionState() end

--- @return ESocketType
function client:getSocketType() end

--- @param args Network.ClientConnectArgs
function client:connect(args) end

--- @param code EDisconnectionCode
function client:disconnect(code) end

--- @param code EDisconnectionCode
function client:tryDisconnect(code) end

--- @param data string
--- @param channel integer
function client:sendReliable(data, channel) end

--- @param data string
--- @param channel integer
function client:sendUnreliable(data, channel) end

--- @class Network.ServerHostArgs
--- @field title string
--- @field password string
--- @field maximumClients integer

--- @class Network.Server
local server = {}

function server:connect() end

--- @param clientID Network.ClientID
--- @param code EDisconnectionCode
function server:disconnect(clientID, code) end

--- @return integer
function server:getClients() end

--- @return integer
function server:getMaxClients() end

--- @return string
function server:getPassword() end

--- @return Network.ServerID
function server:getSessionID() end

--- @return EServerSessionState
function server:getSessionState() end

--- @return ESocketType
function server:getSocketType() end

--- @return string
function server:getTitle() end

--- @param args Network.ServerHostArgs
function server:host(args) end

function server:shutdown() end

--- @param clientID Network.ClientID
--- @param data string
--- @param channel integer
function server:sendReliable(clientID, data, channel) end

--- @param clientID Network.ClientID
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
