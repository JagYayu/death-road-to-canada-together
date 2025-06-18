--- @meta

--- @diagnostic disable: duplicate-doc-alias
--- @alias Events.EventName string
--- @alias Events.EventName "Update"
--- @alias Events.EventName "KeyDown"
--- @alias Events.EventName "KeyUp"
--- @alias Events.EventName "MouseButtonDown"
--- @alias Events.EventName "MouseButtonUp"
--- @alias Events.EventName "MouseMove"
--- @alias Events.EventName "MouseWheel"
--- @alias Events.EventName "Render"
--- @diagnostic enable: duplicate-doc-alias

--- @class Events
Events = {}

--- @alias Events.Key number | string

--- @class Events.AddHandlerArgs
--- @field event Events.EventName
--- @field func fun(e: any)
--- @field name string?
--- @field order string?
--- @field key integer?
--- @field sequence integer?

-- --- @param args Events.AddHandlerArgs
-- function Events.add(args) end

--- @param event Events.EventName
--- @param func fun(e: any)
--- @param name string?
--- @param order string?
--- @param key integer?
--- @param sequence integer?
function Events.add(event, func, name, order, key, sequence) end

--- @param event string | integer
--- @param e any?
--- @param key Events.Key?
function Events.invoke(event, e, key) end

--- @param event string
--- @param orders string[]?
--- @param keys (number|string)[]?
--- @return integer
function Events.new(event, orders, keys) end

return Events
