--- @meta
error("this is a lua library module")

--- @class Engine
TE.engine = {}

--- @type TE.Window
TE.engine.primaryWindow = {}

--- @return string
function TE.engine:getVersion() end

function TE.engine:quit() end

--- @warn It is not recommended to call this function every frame. It will cause the screen to flicker at current version of engine.
function TE.engine:triggerLoadPending() end
