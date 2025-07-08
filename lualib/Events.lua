--- @meta

--- @diagnostic disable: duplicate-doc-alias
--- @alias Tudov.Events.EventName string
--- @alias Tudov.Events.EventName "Update"
--- @alias Tudov.Events.EventName "KeyDown"
--- @alias Tudov.Events.EventName "KeyUp"
--- @alias Tudov.Events.EventName "MouseButtonDown"
--- @alias Tudov.Events.EventName "MouseButtonUp"
--- @alias Tudov.Events.EventName "MouseMove"
--- @alias Tudov.Events.EventName "MouseWheel"
--- @alias Tudov.Events.EventName "Render"
--- @diagnostic enable: duplicate-doc-alias

--- @class Tudov.Events
events = {}

--- @alias Tudov.Events.Key number | string

--- @class Tudov.Events.AddHandlerArgs
--- @field event Tudov.Events.EventName
--- @field func fun(e: any)
--- @field name string?
--- @field order string?
--- @field key integer?
--- @field sequence integer?

--- @param event Tudov.Events.EventName
--- @param func fun(e: any)
--- @param name string?
--- @param order string?
--- @param key (string | integer)?
--- @param sequence integer?
function events:add(event, func, name, order, key, sequence) end

--- @param event string | integer
--- @param e any?
--- @param key Tudov.Events.Key?
--- @param uncached true?
function events:invoke(event, e, key, uncached) end

--- @param event string
--- @param orders string[]?
--- @param keys (number|string)[]?
--- @return integer
function events:new(event, orders, keys) end

return events
