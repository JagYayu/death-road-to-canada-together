--- @meta
error("this is a lua library module")

--- @diagnostic disable: duplicate-doc-alias

--- @class TE.EventID : integer

--- @class TE.E.DebugCommand

--- @class TE.E.DebugCommand
--- @field data TE.E.DebugCommand.Data

--- @class TE.E.DebugCommand.Data

--- @class TE.E.DebugSnapshot

--- @class TE.E.KeyboardPress
--- @field data TE.E.KeyboardPress.Data

--- @class TE.E.KeyboardPress.Data
--- @field window TE.Window?
--- @field windowID TE.WindowID
--- @field keyboard TE.Keyboard?
--- @field keyboardID TE.KeyboardID
--- @field scanCode TE.EScanCode
--- @field keyCode integer
--- @field mod integer

--- @class TE.E.KeyboardRelease : TE.E.KeyboardPress

--- @alias TE.E.KeyRepeat TE.E.KeyboardPress

--- @alias TE.E.KeyUp TE.E.KeyboardPress

--- @class TE.E.MouseButtonDown

--- @class TE.E.MouseButtonUp

--- @class TE.E.MouseMotion
--- @field data TE.E.MouseMotion.Data

--- @class TE.E.MouseMotion.Data
--- @field window TE.Window
--- @field windowID TE.WindowID
--- @field mouse TE.Mouse
--- @field mouseID TE.MouseID
--- @field x number
--- @field y number
--- @field relativeX number
--- @field relativeY number

--- @class TE.E.MousePress
--- @field data TE.E.MousePress.Data

--- @class TE.E.MousePress.Data

--- @class TE.E.MouseRelease : TE.E.MousePress

--- @class TE.E.MouseWheel

--- @class TE.E.TickLoad

--- @class TE.E.TickRender
--- @field window TE.Window

--- @class TE.E.TickUpdate
--- @field data table

--- @class TE.E.ScriptLoad
--- @field data table

--- @class TE.E.ScriptUnload
--- @field data table

--- @class TE.E.ScriptsLoaded
--- @field data table

--- @class TE.E.ClientConnect
--- @field data TE.E.ClientConnect.Data

--- @class TE.E.LocalClientConnect
--- @field data TE.E.LocalClientConnect.Data

--- @class TE.E.RUDPClientConnect
--- @field data TE.E.RUDPClientConnect.Data

--- @class TE.E.ClientConnect.Data : userdata
--- @field socketType TE.ESocketType
--- @field clientID TE.Network.ClientID

--- @class TE.E.LocalClientConnect.Data : TE.E.ClientConnect.Data
--- @field clientSlot integer
--- @field serverSlot integer

--- @class TE.E.RUDPClientConnect.Data : TE.E.ClientConnect.Data
--- @field host string
--- @field port integer

--- @class TE.E.ClientDisconnect
--- @field data TE.E.ClientDisconnect.Data

--- @class TE.E.LocalClientDisconnect
--- @field data TE.E.LocalClientDisconnect.Data

--- @class TE.E.RUDPClientDisconnect
--- @field data TE.E.RUDPClientDisconnect.Data

--- @class TE.E.ClientDisconnect.Data : userdata
--- @field socketType TE.ESocketType
--- @field message string

--- @class TE.E.LocalClientDisconnect.Data : TE.E.ClientDisconnect.Data
--- @field clientSlot integer
--- @field serverSlot integer

--- @class TE.E.RUDPClientDisconnect.Data : TE.E.ClientDisconnect.Data
--- @field host string
--- @field port integer

--- @class TE.E.ClientMessage
--- @field data TE.E.ClientMessage.Data

--- @class TE.E.LocalClientMessage
--- @field data TE.E.LocalClientMessage.Data

--- @class TE.E.RUDPClientMessage
--- @field data TE.E.RUDPClientMessage.Data

--- @class TE.E.ClientMessage.Data : userdata
--- @field socketType TE.ESocketType
--- @field message string

--- @class TE.E.LocalClientMessage.Data : TE.E.ClientMessage.Data
--- @field clientSlot integer
--- @field serverSlot integer

--- @class TE.E.RUDPClientMessage.Data : TE.E.ClientMessage.Data
--- @field host string
--- @field port integer

--- @class TE.E.ServerConnect.Data : userdata
--- @field socketType TE.ESocketType
--- @field clientID TE.Network.ClientID

--- @class TE.E.LocalServerConnect.Data : TE.E.ServerConnect.Data
--- @field clientSlot integer
--- @field serverSlot integer

--- @class TE.E.RUDPServerConnect.Data : TE.E.ServerConnect.Data
--- @field host string
--- @field port integer

--- @class TE.E.ServerConnect
--- @field data TE.E.ServerConnect.Data

--- @class TE.E.LocalServerConnect
--- @field data TE.E.LocalServerConnect.Data

--- @class TE.E.RUDPServerConnect
--- @field data TE.E.RUDPServerConnect.Data

--- @class TE.E.ServerDisconnect.Data : userdata
--- @field socketType TE.ESocketType
--- @field clientID TE.Network.ClientID

--- @class TE.E.LocalServerDisconnect.Data : TE.E.ServerDisconnect.Data
--- @field clientSlot integer
--- @field serverSlot integer

--- @class TE.E.RUDPServerDisconnect.Data : TE.E.ServerDisconnect.Data
--- @field host string
--- @field port integer

--- @class TE.E.ServerDisconnect
--- @field data TE.E.ServerDisconnect.Data

--- @class TE.E.LocalServerDisconnect
--- @field data TE.E.LocalServerDisconnect.Data

--- @class TE.E.RUDPServerDisconnect
--- @field data TE.E.RUDPServerDisconnect.Data

--- @class TE.E.ServerMessage.Data : userdata
--- @field socketType TE.ESocketType
--- @field clientID TE.Network.ClientID
--- @field message string
--- @field broadcast string?

--- @class TE.E.LocalServerMessage.Data : TE.E.ServerMessage.Data
--- @field clientSlot integer
--- @field serverSlot integer

--- @class TE.E.RUDPServerMessage.Data : TE.E.ServerMessage.Data
--- @field host string
--- @field port integer

--- @class TE.E.ServerMessage
--- @field data TE.E.ServerMessage.Data

--- @class TE.E.LocalServerMessage
--- @field data TE.E.LocalServerMessage.Data

--- @class TE.E.RUDPServerMessage
--- @field data TE.E.RUDPServerMessage.Data

--- @alias TE.Event string | TE.EventID
--- @alias TE.Event "ClientConnect"
--- @alias TE.Event "ClientDisconnect"
--- @alias TE.Event "ClientMessage"
--- @alias TE.Event "DebugCommand"
--- @alias TE.Event "DebugSnapshot"
--- @alias TE.Event "KeyboardAdded"
--- @alias TE.Event "KeyboardPress"
--- @alias TE.Event "KeyboardRelease"
--- @alias TE.Event "KeyboardRemoved"
--- @alias TE.Event "KeyboardRepeat"
--- @alias TE.Event "MouseButtonDown"
--- @alias TE.Event "MouseButtonUp"
--- @alias TE.Event "MouseMotion"
--- @alias TE.Event "MouseWheel"
--- @alias TE.Event "ScriptGlobalIndex"
--- @alias TE.Event "ScriptLoad"
--- @alias TE.Event "ScriptUnload"
--- @alias TE.Event "ScriptsLoaded"
--- @alias TE.Event "ServerConnect"
--- @alias TE.Event "ServerDisconnect"
--- @alias TE.Event "ServerMessage"
--- @alias TE.Event "TickLoad"
--- @alias TE.Event "TickRender"
--- @alias TE.Event "TickUpdate"
--- @alias TE.Event "WindowResize"

--- @diagnostic enable: duplicate-doc-alias

--- @class TE.Event

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

--- @class TE.RuntimeEvent
local RuntimeEvent = {}

--- @return integer scriptID
function RuntimeEvent.getInvokingScriptID() end

--- @class TE.Events
local events = {}

--- @alias TE.EventKey number | string

--- @alias TE.EventOrder string

--- @alias TE.EventSequence number

--- @param event TE.Event
--- @param func fun(e: any)
--- @param name? string
--- @param order? TE.EventOrder
--- @param key? string | integer
--- @param sequence? integer
function events:add(event, func, name, order, key, sequence) end

--- @param event TE.Event
--- @param e any
--- @param key TE.EventKey?
--- @param options Events.EEventInvocation? @default: `EEventInvocation.Default`
function events:invoke(event, e, key, options) end

--- @param event string
--- @param orders string[]?
--- @param keys (number|string)[]?
--- @return TE.EventID
function events:new(event, orders, keys) end

TE.events = events
