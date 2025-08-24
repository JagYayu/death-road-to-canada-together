--- @meta
error("this is a lua library module")

--- @diagnostic disable: duplicate-doc-alias

--- @class Events.EventID : integer

--- @class Events.E.DebugCommand

--- @class Events.E.DebugSnapshot

--- @class Events.E.KeyDown
--- @field window Window?
--- @field windowID WindowID
--- @field keyboard Keyboard?
--- @field keyboardID KeyboardID
--- @field scanCode EScanCode
--- @field keyCode integer
--- @field mod integer

--- @class Events.E.ClientMessage.Data : userdata
--- @field socketType ESocketType
--- @field message string

--- @class Events.E.LocalClientMessage.Data : Events.E.ClientMessage.Data
--- @field clientSlot integer
--- @field serverSlot integer

--- @class Events.E.RUDPClientMessage.Data : Events.E.ClientMessage.Data
--- @field host string
--- @field port integer

--- @class Events.E.ClientMessage
--- @field data Events.E.ClientMessage.Data

--- @class Events.E.LocalClientMessage
--- @field data Events.E.LocalClientMessage.Data

--- @class Events.E.RUDPClientMessage
--- @field data Events.E.RUDPClientMessage.Data

--- @class Events.E.KeyRepeat

--- @class Events.E.KeyUp

--- @class Events.E.MouseButtonDown

--- @class Events.E.MouseButtonUp

--- @class Events.E.MouseMove

--- @class Events.E.MouseWheel

--- @class Events.E.TickLoad

--- @class Events.E.TickRender

--- @class Events.E.TickUpdate

--- @alias Events.Event string | Events.EventID
--- @alias Events.Event "ClientConnect"
--- @alias Events.Event "ClientDisconnect"
--- @alias Events.Event "ClientMessage"
--- @alias Events.Event "DebugCommand"
--- @alias Events.Event "DebugSnapshot"
--- @alias Events.Event "KeyDown"
--- @alias Events.Event "KeyUp"
--- @alias Events.Event "MouseButtonDown"
--- @alias Events.Event "MouseButtonUp"
--- @alias Events.Event "MouseMove"
--- @alias Events.Event "MouseWheel"
--- @alias Events.Event "ServerConnect"
--- @alias Events.Event "ServerDisconnect"
--- @alias Events.Event "ServerMessage"
--- @alias Events.Event "TickLoad"
--- @alias Events.Event "TickRender"
--- @alias Events.Event "TickUpdate"

--- @diagnostic enable: duplicate-doc-alias

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

--- @class Events
events = {}

--- @alias Events.Key number | string

--- @class Events.AddHandlerArgs
--- @field event Events.Event
--- @field func fun(e: any)
--- @field name string?
--- @field order string?
--- @field key integer?
--- @field sequence integer?

--- @param event Events.Event
--- @param func fun(e: any)
--- @param name string?
--- @param order string?
--- @param key (string | integer)?
--- @param sequence integer?
function events:add(event, func, name, order, key, sequence) end

--- @param event Events.Event
--- @param e any
--- @param key Events.Key?
--- @param options Events.EEventInvocation? @default: `EEventInvocation.Default`
function events:invoke(event, e, key, options) end

--- @param event string
--- @param orders string[]?
--- @param keys (number|string)[]?
--- @return Events.EventID
function events:new(event, orders, keys) end

return events
