local String = require("tudov.String")

local SPlayerInputBuffer = {}

local providedByClient, CPlayerInputBuffers = pcall(require, "dr2c.client.game.PlayerInputBuffers")

function SPlayerInputBuffer.isProvidedByClient()
	return providedByClient
end

local playerInputBuffers
playerInputBuffers = persist(
	"playerInputBuffers",
	providedByClient and CPlayerInputBuffers.getRawTable() or {},
	function()
		return playerInputBuffers
	end
)

function SPlayerInputBuffer.getRawTable()
	return playerInputBuffers
end

return SPlayerInputBuffer
