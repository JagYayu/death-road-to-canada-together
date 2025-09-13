--- @meta
error("this is a lua library module")

--- @class Engine
engine = {}

--- @type Window
engine.mainWindow = {}

--- @return string
function engine:getVersion() end

function engine:quit() end

function engine:triggerLoadPending() end
