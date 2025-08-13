local Function = {}

function Function.add(a, b)
	return a + b
end

function Function.generateCompareAsc(keyList)
	local len = #keyList

	return function(l, r)
		for i = 1, len do
			local key = keyList[i]
			local lv, rv = l[key], r[key]

			if lv ~= rv then
				return lv < rv
			end
		end
	end
end

function Function.generateCompareDesc(keyList)
	local len = #keyList

	return function(l, r)
		for i = 1, len do
			local key = keyList[i]
			local lv, rv = l[key], r[key]

			if lv ~= rv then
				return lv > rv
			end
		end
	end
end

function Function.empty() end

function Function.newIndexReadonly()
	error("Attempt to modify a readonly table", 2)
end

return Function
