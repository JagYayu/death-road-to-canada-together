local Format = {}

function Format.color(value) end

function Format.password(value) end

function Format.percent(value) end

function Format.time() end

function Format.format(fmt, ...)
	local args = { ... }
	return fmt:gsub("{(%d+)}", function(n)
		n = tonumber(n)
		local v = args[n]
		if v == nil then
			return ""
		elseif type(v) == "table" then
			return tostring(v)
		else
			return v
		end
	end)
end

-- print(Format.format("hello {1:color}, I'm {2:time}", "world", "JagYayu!"))

return Format
