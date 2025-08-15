--- @meta
error("this is a lua library module")

--- @diagnostic disable: duplicate-doc-alias
--- @alias Tudov.EventManager.EventName string
--- @alias Tudov.EventManager.EventName "DebugCommand"
--- @alias Tudov.EventManager.EventName "DebugSnapshot"
--- @alias Tudov.EventManager.EventName "KeyDown"
--- @alias Tudov.EventManager.EventName "KeyUp"
--- @alias Tudov.EventManager.EventName "MouseButtonDown"
--- @alias Tudov.EventManager.EventName "MouseButtonUp"
--- @alias Tudov.EventManager.EventName "MouseMove"
--- @alias Tudov.EventManager.EventName "MouseWheel"
--- @alias Tudov.EventManager.EventName "TickRender"
--- @alias Tudov.EventManager.EventName "TickUpdate"
--- @diagnostic enable: duplicate-doc-alias

--- @class Tudov.EventManager
events = {}

--- @alias Tudov.EventManager.Key number | string

--- @class Tudov.EventManager.AddHandlerArgs
--- @field event Tudov.EventManager.EventName
--- @field func fun(e: any)
--- @field name string?
--- @field order string?
--- @field key integer?
--- @field sequence integer?

--- @param event Tudov.EventManager.EventName
--- @param func fun(e: any)
--- @param name string?
--- @param order string?
--- @param key (string | integer)?
--- @param sequence integer?
function events:add(event, func, name, order, key, sequence) end

--- @param event string | integer
--- @param e any?
--- @param key Tudov.EventManager.Key?
--- @param uncached true?
function events:invoke(event, e, key, uncached) end

--- @param event string
--- @param orders string[]?
--- @param keys (number|string)[]?
--- @return integer
function events:new(event, orders, keys) end

return events
