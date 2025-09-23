--- @meta
error("this is a lua library module")

--- @class Engine
engine = {}

--- @type TE.Window
engine.primaryWindow = {}

--- @return string
function engine:getVersion() end

function engine:quit() end

--- @warn It is not recommended to call this function every frame. It will cause the screen to flicker at current version of engine.
function engine:triggerLoadPending() end
