--- @class dr2c.SNetworkServer
local SServers = {}

SServers.sessionSlot = 0

SServers.eventClientMessage = events:new(N_("ServerMessage"), {
	"ServerList",
})

events:add("ServerMessage", function(e)
	print(e)
	-- events:invoke(CNetworkClient.eventClientMessage, e, key?, options?)
end, "", nil, SServers.sessionSlot)

return SServers
