--#region Update

local eventUpdateGame = events:new(N_("UpdateGame"), {
	"Client",
	"Server",
})

events:add("TickUpdate", function(e)
	-- local t = {}
	-- for i = 1, 1e7 do
	-- 	t[#t + 1] = math.random()
	-- end

	events:invoke(eventUpdateGame, e)
end, N_("gameUpdate"))

--#endregion

--#region Load

local eventContentLoad = events:new(N_("ContentLoad"), {
	"Sprites",
	"ECS",
})

events:add("TickLoad", function(e)
	events:invoke(eventContentLoad, e)
end, N_("gameLoad"))

engine:triggerLoadPending()

--#endregion
