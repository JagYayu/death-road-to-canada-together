----------------------------------------------------------------------------
-- LuaJIT profiler zones.
--
-- Copyright (C) 2005-2022 Mike Pall. All rights reserved.
-- Released under the MIT license. See Copyright Notice in luajit.h
----------------------------------------------------------------------------
local a=table.remove;return setmetatable({flush=function(b)for c=#b,1,-1 do b[c]=nil end end,get=function(b)return b[#b]end},{__call=function(b,d)if d then b[#b+1]=d else return assert(a(b),"empty zone stack")end end})