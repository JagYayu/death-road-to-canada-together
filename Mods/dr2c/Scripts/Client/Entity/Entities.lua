--[[
-- @module dr2c.Client.Entity.Entities
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local CEntityECSSchema = require("dr2c.Client.Entity.ECSSchema")

local CEntities = {}

--- @param name string
--- @param components dr2c.CComponents
function CEntities.registerEntity(name, components, mergeDepth, ...)
	local extras = { ... }

	TE.events:add(CEntityECSSchema.eventEntitySchemaLoadEntities, function(e)
		e.new[name] = {
			components = components,
			mergeDepth = mergeDepth,
			unpack(extras),
		}
	end, N_("registerEntity" .. name), "Register")

	CEntityECSSchema.reload()
end

return CEntities
