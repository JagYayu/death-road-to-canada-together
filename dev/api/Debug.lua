--- @class dr2c.EDebugConsoleCode
EDebugConsoleCode = {
	None = 0,
	Failure = 1,
	Success = 2,
	Warn = 3,
}

--- @class Events.E.DebugCommand.Data
local Events_E_DebugCommand_Data = {}

--- @param cmd { name: string, help: string, func: fun(arg: string): table }
function Events_E_DebugCommand_Data:setDebugCommand(cmd) end
