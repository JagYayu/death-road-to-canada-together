--[[
-- @module dr2c.Shared.utils.Delay
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

--- @class dr2c.GUtilsDelay
local GUtilsDelay = {}

--- @class dr2c.UtilsDelayRegisterArgs
--- @field callback fun()
--- @field event Events.Event?
--- @field eventKey Events.Key?
--- @field eventOrder Events.Order?
--- @field eventSequence Events.Sequence?
--- @field events { event: Events.Event, eventKey: Events.Key?, eventOrder: Events.Order?, eventSequence: Events.Sequence? }[]?

local registeredDelayLatestID = 0

local registeredDelays = {}

registeredDelayLatestID = persist("updateDelayLatestID", function()
	return registeredDelayLatestID
end)
registeredDelays = persist("registeredDelays", function()
	return registeredDelays
end)

--- @param args dr2c.UtilsDelayRegisterArgs
--- @return fun()
function GUtilsDelay.register(args)
	local function func()
		--
	end

	local function eventsAdd(entry)
		TE.events:add(
			entry.event or "CUpdate",
			func,
			"UtilsDelayHandler" .. registeredDelayLatestID,
			entry.eventOrder,
			entry.eventKey,
			entry.eventSequence
		)
	end

	eventsAdd(args)
	for _, entry in ipairs(args.events) do
		eventsAdd(entry)
	end

	return function()
		--
	end
end

return GUtilsDelay
