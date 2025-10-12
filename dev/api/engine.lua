--- @meta
error("this is a lua library module")

--- @class Engine
local engine = {}

--- @type TE.Window
engine.primaryWindow = {}

--- @return number
function engine:getBeginTick() end

--- @return number
function engine:getTick() end

--- @return number
function engine:getLoadingBeginTick() end

--- @return string
function engine:getVersion() end

--- Quit the whole application.
function engine:quit() end

--- @warn It is not recommended to call this function every frame. It will cause the screen to flicker at current version of engine.
function engine:triggerLoadPending() end

TE.engine = engine
