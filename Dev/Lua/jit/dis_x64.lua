----------------------------------------------------------------------------
-- LuaJIT x64 disassembler wrapper module.
--
-- Copyright (C) 2005-2022 Mike Pall. All rights reserved.
-- Released under the MIT license. See Copyright Notice in luajit.h
----------------------------------------------------------------------------
local a=require((string.match(...,".*%.")or"").."dis_x86")return{create=a.create64,disass=a.disass64,regname=a.regname64}