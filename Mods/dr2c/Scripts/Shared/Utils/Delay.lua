--[[
-- @module dr2c.Shared.Utils.Delay
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
--- @field event TE.Event?
--- @field eventKey TE.EventKey?
--- @field eventOrder TE.EventOrder?
--- @field eventSequence TE.EventSequence?
--- @field events { event: TE.Event, eventKey: TE.EventKey?, eventOrder: TE.EventOrder?, eventSequence: TE.EventSequence? }[]?

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
