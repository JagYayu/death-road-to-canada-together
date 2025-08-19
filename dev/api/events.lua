--- @meta
error("this is a lua library module")

--- @diagnostic disable: duplicate-doc-alias

--- @class Events.EventID : integer

--- @alias Events.E any?

--- @alias Events.Event string | Events.EventID
--- @alias Events.Event "DebugCommand"
--- @alias Events.Event "DebugSnapshot"
--- @alias Events.Event "KeyDown"
--- @alias Events.Event "KeyUp"
--- @alias Events.Event "MouseButtonDown"
--- @alias Events.Event "MouseButtonUp"
--- @alias Events.Event "MouseMove"
--- @alias Events.Event "MouseWheel"
--- @alias Events.Event "TickLoad"
--- @alias Events.Event "TickRender"
--- @alias Events.Event "TickUpdate"

--- @diagnostic enable: duplicate-doc-alias

--- @enum Events.EEventInvocation
EEventInvocation = {
	All = 255,
	CacheHandlers = 1,
	Default = 1,
	NoProfiler = 2,
	None = 0,
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

--- @param event string | integer
--- @param e Events.E
--- @param key Events.Key?
--- @param options Events.EEventInvocation? @default: `EEventInvocation.Default`
function events:invoke(event, e, key, options) end

--- @param event string
--- @param orders string[]?
--- @param keys (number|string)[]?
--- @return Events.EventID
function events:new(event, orders, keys) end

return events
