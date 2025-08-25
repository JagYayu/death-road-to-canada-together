--- @class dr2c.CRender
local CRender = {}

--- @type integer
local renderFrame
renderFrame = persist("renderFrame", 0, function()
	return renderFrame
end)

--- @return integer
function CRender.getFrame()
	return renderFrame
end

local eventClientRender = events:new(N_("CRender"), {
	"Camera",
	"UI",
})

events:add("TickRender", function(e)
	events:invoke(eventClientRender, e)
end, N_("ClientRender"), nil, "Main")

return CRender
