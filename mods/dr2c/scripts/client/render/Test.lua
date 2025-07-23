if 1 then
	return
end

-- draw a rect in 2d space.
drawRect({
	imageID = nil,
	destination = { 0, 0,  }
})

-- draw faces in 3d space.
drawVerts({
	imageID = nil,
	vertices = {
		{ -- top left
			0, -- x
			0, -- y
			0, -- z
			-1, -- color
			0, -- tx
			0, -- ty
			0, -- anim
		},
		{ -- top right
			-- ...
		},
		{ -- bottom right
			-- ...
		},
		{ -- bottom left
			-- ...
		},
	},
	indices = { 1, 2, 3, 1, 3, 4 },
	order = 0,
})
