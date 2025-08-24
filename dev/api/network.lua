--- @meta
error("this is a lua library module")

--- @class Network.ClientID : integer

--- @class Network.ServerID : integer

--- @enum ESocketType
ESocketType = {
	Local = 0,
	RUDP = 1,
	Steam = 2,
	TCP = 3,
}

--- @class Network
network = {}

--- @param clientSessionSlot integer?
--- @return Network.Client
function network:getClient(clientSessionSlot) end

--- @param serverSessionSlot integer?
--- @return Network.Server
function network:getServer(serverSessionSlot) end

--- @class Network.ClientConnectArgs
--- @field password string

--- @class Network.Client
local client = {}

--- @return Network.ClientID
function client:getSessionID() end

--- @param args Network.ClientConnectArgs
function client:connect(args) end

function client:disconnect() end

--- @return boolean connected
function client:isConnected() end

--- @return boolean connecting
function client:isConnecting() end

--- @param data string
function client:sendReliable(data) end

--- @param data string
function client:sendUnreliable(data) end

function client:tryDisconnect() end

--- @class Network.ServerHostArgs
--- @field title string
--- @field password string
--- @field maximumClients integer

--- @class Network.Server
local server = {}

--- @return Network.ServerID
function server:getSessionID() end

--- @param data string
function server:broadcastReliable(data) end

--- @param data string
function server:broadcastUnreliable(data) end

--- @param args Network.ServerHostArgs
function server:host(args) end

--- @return boolean hosting
function server:isHosting() end

--- @return boolean shutdown
function server:isShutdown() end

--- @param clientID Network.ClientID
--- @param data string
function server:sendReliable(clientID, data) end

--- @param clientID Network.ClientID
--- @param data string
function server:sendUnreliable(clientID, data) end

function server:shutdown() end
