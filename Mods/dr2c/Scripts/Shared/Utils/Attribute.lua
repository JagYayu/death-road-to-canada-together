--[[
-- @module dr2c.Shared.Utils.Attribute
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Function = require("TE.Function")
local Table = require("TE.Table")

local GUtilsAttribute = {}

--- @alias dr2c.AttributePropertyDefault nil | Serializable | fun(): Serializable

--- @alias dr2c.AttributePropertyValidator fun(value: any): boolean

--- @class dr2c.AttributePropertyArgs
--- @field default? dr2c.AttributePropertyDefault
--- @field validator? dr2c.AttributePropertyValidator
--- @field noReset? boolean

--- @class dr2c.AttributeProperty : dr2c.AttributePropertyArgs
--- @field validator dr2c.AttributePropertyValidator
--- @field noReset boolean

--- @param defaultProperties table<TE.Enum.Value, dr2c.AttributePropertyArgs>
function GUtilsAttribute.newProperties(defaultProperties)
	--- @class dr2c.AttributePropertiesProxy
	local proxy = {}

	--- @type table<TE.Enum.Value, dr2c.AttributeProperty>
	--- @diagnostic disable-next-line: assign-type-mismatch
	local attributeProperties = defaultProperties or {}

	--- @param attribute TE.Enum.Value
	--- @return dr2c.AttributeProperty
	function proxy.getAttributeProperty(attribute)
		return attributeProperties[attribute]
	end

	--- @param attribute TE.Enum.Value
	--- @param args dr2c.AttributePropertyArgs
	function proxy.setAttributeProperty(attribute, args)
		attributeProperties[attribute] = {
			default = args.default,
			noReset = not not args.noReset,
			validator = args.validator or Function.alwaysTrue,
		}
	end

	--- @param attribute TE.Enum.Value
	--- @param value? Serializable
	--- @return boolean
	--- @nodiscard
	function proxy.validateAttribute(attribute, value)
		local property = attributeProperties[attribute]
		if property then
			return not not property.validator(value)
		else
			return true
		end
	end

	--- @param attributes table<TE.Enum.Value, any>
	function proxy.resetAttributes(attributes)
		for attribute, property in pairs(attributeProperties) do
			if not property.noReset then
				local default = property.default
				if type(default) == "table" then
					attributes[attribute] = Table.fastCopy(default)
				elseif type(default) == "function" then
					attributes[attribute] = default()
				elseif default ~= nil then
					attributes[attribute] = default
				end
			end
		end
	end

	return proxy
end

return GUtilsAttribute
