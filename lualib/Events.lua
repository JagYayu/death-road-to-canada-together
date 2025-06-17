--- @meta

--- @diagnostic disable
--- @alias Events.EventName string
--- @alias Events.EventName "Update"
--- @alias Events.EventName "Draw"
--- @diagnostic enable

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
