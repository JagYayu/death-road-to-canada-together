--- @meta

--- @diagnostic disable
--- @alias Events.EventName string
--- @alias Events.EventName "Update"
--- @alias Events.EventName "Draw"
--- @diagnostic enable

--- @class Events
Events = {}

--- @class Events.AddHandlerArgs
--- @field name string
--- @field func fun(e: any)
--- @field order string?
--- @field key integer?
--- @field sequence integer?

--- @param args Events.AddHandlerArgs
function Events.add(args) end

return Events
