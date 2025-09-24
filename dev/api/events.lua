--- @meta
error("this is a lua library module")

--- @diagnostic disable: duplicate-doc-alias

--- @class Events.EventID : integer

--- @class Events.E.DebugCommand

--- @class Events.E.DebugCommand
--- @field data Events.E.DebugCommand.Data

--- @class Events.E.DebugSnapshot

--- @class Events.E.KeyboardPress
--- @field window TE.Window?
--- @field windowID TE.WindowID
--- @field keyboard TE.Keyboard?
--- @field keyboardID TE.KeyboardID
--- @field scanCode TE.EScanCode
--- @field keyCode integer
--- @field mod integer

--- @alias Events.E.KeyRepeat Events.E.KeyboardPress

--- @alias Events.E.KeyUp Events.E.KeyboardPress

--- @class Events.E.MouseButtonDown

--- @class Events.E.MouseButtonUp

--- @class Events.E.MouseMove

--- @class Events.E.MouseWheel

--- @class Events.E.TickLoad

--- @class Events.E.TickRender
--- @field window TE.Window

--- @class Events.E.TickUpdate

--- @class Events.E.ScriptsLoaded

--- @class Events.E.ClientConnect
--- @field data Events.E.ClientConnect.Data

--- @class Events.E.LocalClientConnect
--- @field data Events.E.LocalClientConnect.Data

--- @class Events.E.RUDPClientConnect
--- @field data Events.E.RUDPClientConnect.Data

--- @class Events.E.ClientConnect.Data : userdata
--- @field socketType ESocketType
--- @field clientID Network.ClientID

--- @class Events.E.LocalClientConnect.Data : Events.E.ClientConnect.Data
--- @field clientSlot integer
--- @field serverSlot integer

--- @class Events.E.RUDPClientConnect.Data : Events.E.ClientConnect.Data
--- @field host string
--- @field port integer

--- @class Events.E.ClientDisconnect
--- @field data Events.E.ClientDisconnect.Data

--- @class Events.E.LocalClientDisconnect
--- @field data Events.E.LocalClientDisconnect.Data

--- @class Events.E.RUDPClientDisconnect
--- @field data Events.E.RUDPClientDisconnect.Data

--- @class Events.E.ClientDisconnect.Data : userdata
--- @field socketType ESocketType
--- @field message string

--- @class Events.E.LocalClientDisconnect.Data : Events.E.ClientDisconnect.Data
--- @field clientSlot integer
--- @field serverSlot integer

--- @class Events.E.RUDPClientDisconnect.Data : Events.E.ClientDisconnect.Data
--- @field host string
--- @field port integer

--- @class Events.E.ClientMessage
--- @field data Events.E.ClientMessage.Data

--- @class Events.E.LocalClientMessage
--- @field data Events.E.LocalClientMessage.Data

--- @class Events.E.RUDPClientMessage
--- @field data Events.E.RUDPClientMessage.Data

--- @class Events.E.ClientMessage.Data : userdata
--- @field socketType ESocketType
--- @field message string

--- @class Events.E.LocalClientMessage.Data : Events.E.ClientMessage.Data
--- @field clientSlot integer
--- @field serverSlot integer

--- @class Events.E.RUDPClientMessage.Data : Events.E.ClientMessage.Data
--- @field host string
--- @field port integer

--- @class Events.E.ServerConnect.Data : userdata
--- @field socketType ESocketType
--- @field clientID Network.ClientID

--- @class Events.E.LocalServerConnect.Data : Events.E.ServerConnect.Data
--- @field clientSlot integer
--- @field serverSlot integer

--- @class Events.E.RUDPServerConnect.Data : Events.E.ServerConnect.Data
--- @field host string
--- @field port integer

--- @class Events.E.ServerConnect
--- @field data Events.E.ServerConnect.Data

--- @class Events.E.LocalServerConnect
--- @field data Events.E.LocalServerConnect.Data

--- @class Events.E.RUDPServerConnect
--- @field data Events.E.RUDPServerConnect.Data

--- @class Events.E.ServerDisconnect.Data : userdata
--- @field socketType ESocketType
--- @field clientID Network.ClientID

--- @class Events.E.LocalServerDisconnect.Data : Events.E.ServerDisconnect.Data
--- @field clientSlot integer
--- @field serverSlot integer

--- @class Events.E.RUDPServerDisconnect.Data : Events.E.ServerDisconnect.Data
--- @field host string
--- @field port integer

--- @class Events.E.ServerDisconnect
--- @field data Events.E.ServerDisconnect.Data

--- @class Events.E.LocalServerDisconnect
--- @field data Events.E.LocalServerDisconnect.Data

--- @class Events.E.RUDPServerDisconnect
--- @field data Events.E.RUDPServerDisconnect.Data

--- @class Events.E.ServerMessage.Data : userdata
--- @field socketType ESocketType
--- @field clientID Network.ClientID
--- @field message string
--- @field broadcast string?

--- @class Events.E.LocalServerMessage.Data : Events.E.ServerMessage.Data
--- @field clientSlot integer
--- @field serverSlot integer

--- @class Events.E.RUDPServerMessage.Data : Events.E.ServerMessage.Data
--- @field host string
--- @field port integer

--- @class Events.E.ServerMessage
--- @field data Events.E.ServerMessage.Data

--- @class Events.E.LocalServerMessage
--- @field data Events.E.LocalServerMessage.Data

--- @class Events.E.RUDPServerMessage
--- @field data Events.E.RUDPServerMessage.Data

--- @alias Events.Event string | Events.EventID
--- @alias Events.Event "ClientConnect"
--- @alias Events.Event "ClientDisconnect"
--- @alias Events.Event "ClientMessage"
--- @alias Events.Event "DebugCommand"
--- @alias Events.Event "DebugSnapshot"
--- @alias Events.Event "KeyboardPress"
--- @alias Events.Event "KeyUp"
--- @alias Events.Event "MouseButtonDown"
--- @alias Events.Event "MouseButtonUp"
--- @alias Events.Event "MouseMove"
--- @alias Events.Event "MouseWheel"
--- @alias Events.Event "ScriptGlobalIndex"
--- @alias Events.Event "ScriptUnload"
--- @alias Events.Event "ScriptsLoaded"
--- @alias Events.Event "ServerConnect"
--- @alias Events.Event "ServerDisconnect"
--- @alias Events.Event "ServerMessage"
--- @alias Events.Event "TickLoad"
--- @alias Events.Event "TickRender"
--- @alias Events.Event "TickUpdate"

--- @diagnostic enable: duplicate-doc-alias

--- @class Events.Event

--- @enum Events.EEventInvocation
EEventInvocation = {
	-- All flags
	All = 255,
	-- Try caching all handlers that meet the key criteria.
	-- Disabling it can reduce memory usage, but may result in slightly worse performance when called frequently with a large number of handlers.
	CacheHandlers = 1,
	-- Default flags
	Default = 1,
	-- It will forcibly disable the Profiler functionality, even if the client has enabled performance monitoring.
	NoProfiler = 2,
	-- No flags
	None = 0,
	-- Trace event invocation progression.
	TrackProgression = 4,
}

--- @class RuntimeEvent
local RuntimeEvent = {}

--- @return integer scriptID
function RuntimeEvent.getInvokingScriptID() end

--- @class Events
TE.events = {}

--- @alias Events.Key number | string

--- @alias Events.Order string

--- @alias Events.Sequence number

--- @class Events.AddHandlerArgs
--- @field event Events.Event
--- @field func fun(e: any)
--- @field name string?
--- @field order Events.Order?
--- @field key Events.Key?
--- @field sequence integer?

--- @param event Events.Event
--- @param func fun(e: any)
--- @param name string?
--- @param order Events.Order?
--- @param key (string | integer)?
--- @param sequence integer?
function TE.events:add(event, func, name, order, key, sequence) end

--- @param event Events.Event
--- @param e any
--- @param key Events.Key?
--- @param options Events.EEventInvocation? @default: `EEventInvocation.Default`
function TE.events:invoke(event, e, key, options) end

--- @param event string
--- @param orders string[]?
--- @param keys (number|string)[]?
--- @return Events.EventID
function TE.events:new(event, orders, keys) end
