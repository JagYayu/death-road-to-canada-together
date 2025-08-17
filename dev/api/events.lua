--- @meta
error("this is a lua library module")

--- @diagnostic disable: duplicate-doc-alias

--- @alias Events.E any?

--- @alias Events.Event string
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

--- @class Events.InvocationOption : integer

--- @type Events.InvocationOption
eventsOption_All = 255
--- @type Events.InvocationOption
eventsOption_None = 0
--- @type Events.InvocationOption
eventsOption_CacheHandlers = 1
--- @type Events.InvocationOption
eventsOption_NoProfiler = 2
--- @type Events.InvocationOption
eventsOption_TrackProgression = 4
--- @type Events.InvocationOption
eventsOption_Default = eventsOption_CacheHandlers

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
--- @param options Events.InvocationOption? @default: `_G.eventsOption_Default`
function events:invoke(event, e, key, options) end

--- @param event string
--- @param orders string[]?
--- @param keys (number|string)[]?
--- @return integer
function events:new(event, orders, keys) end

return events
