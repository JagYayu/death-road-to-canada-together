--[[
-- @module dr2c.Client.ECS.Entities
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CECSSchema = require("dr2c.Client.ECS.ECSSchema")

local CEntities = {}

--- @param name string
--- @param components dr2c.CComponents
function CEntities.registerEntity(name, components, mergeDepth, ...)
	local extras = { ... }

	TE.events:add(CECSSchema.eventEntitySchemaLoadEntities, function(e)
		e.new[name] = {
			components = components,
			mergeDepth = mergeDepth,
			unpack(extras),
		}
	end, N_("registerEntity" .. name), "Register")

	CECSSchema.reload()
end

return CEntities
