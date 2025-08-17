local eventLoadGame = events:new(N_("LoadGame"), {
	"ECS",
})

local loadOnce = true

events:add("TickLoad", function(e)
	if loadOnce then
		loadOnce = false

		local t = {}
		for i = 1, 9e7 do
			t[#t + 1] = i
		end
	end

	events:invoke(eventLoadGame, e)
end, N_("gameLoad"))
