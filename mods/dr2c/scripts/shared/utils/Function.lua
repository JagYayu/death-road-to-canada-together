local Function = {}

function Function.add(a, b)
	return a + b
end

function Function.empty()
end

function Function.newIndexReadonly()
	error("Attempt to modify a readonly table", 2)
end

return Function
