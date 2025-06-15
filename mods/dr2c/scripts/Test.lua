Events.add("Update", function(e)
	-- local tbl = { x = 0, y = 0 }
	-- local v = math.random()
	-- for i = 1, 10000 do
	-- 	tbl.x = tbl.x + v * 2
	-- 	tbl.y = tbl.y + v * 3
	-- end
end)

Events.add("Render", function(e)
	-- print(1)
	Render.drawSprite()
	-- print(2)
	-- local tbl = { x = 0, y = 0 }
	-- local v = math.random()
	-- for i = 1, 10000 do
	-- 	tbl.x = tbl.x + v * 2 
	-- 	tbl.y = tbl.y + v * 3
	-- end
end)

return {}
