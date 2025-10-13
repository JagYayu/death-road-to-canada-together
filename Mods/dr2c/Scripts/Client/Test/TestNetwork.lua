local connectionState = 0

connectionState = persist("isConnected", function()
	return connectionState
end)

local function errorHandler(message)
	if not success and log.canWarn() then
		log.warn(message)
	end
end

TE.events:add(N_("CUpdate"), function()
	-- if connectionState == 0 then
	-- 	xpcall(network.serverHostRUDP, errorHandler, network, {
	-- 		host = "localhost",
	-- 		port = 8878,
	-- 	})

	-- 	connectionState = 1
	-- elseif connectionState == 1 then
	-- 	xpcall(network.clientConnectRUDP, errorHandler, network, {
	-- 		host = "localhost",
	-- 		port = 8878,
	-- 	})

	-- 	connectionState = 2
	-- end
end, "TestNetwork", "Network", nil, -1)
