--[[
-- @module dr2c.Shared.World.Snapshot
-- @author JagYayu
-- @brief
-- @version 1.0
-- @date 2025
--
-- @copyright Copyright (c) 2025 JagYayu. Licensed under MIT License.
--
--]]

local Table = require("TE.Table")
local Utility = require("TE.Utility")

local math_floor = math.floor

--- @class dr2c.SnapshotData : string

--- @class dr2c.SnapshotID : integer

--- @class dr2c.GWorldSnapshot
local GWorldSnapshot = {}

local ticksPerID = 1
local idsPerTick = 1 / ticksPerID

function GWorldSnapshot.getIDsPerTick()
	return idsPerTick
end

--- @param snapshotID dr2c.SnapshotID
--- @return dr2c.WorldTick
--- @nodiscard
function GWorldSnapshot.id2tick(snapshotID)
	return snapshotID * ticksPerID
end

--- @param worldTick dr2c.WorldTick
--- @return dr2c.SnapshotID
--- @nodiscard
function GWorldSnapshot.tick2id(worldTick)
	return math_floor(worldTick * idsPerTick)
end

return GWorldSnapshot
