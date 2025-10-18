----------------------------------------------------------------------------
-- LuaJIT MIPS64R6EL disassembler wrapper module.
--
-- Copyright (C) 2005-2022 Mike Pall. All rights reserved.
-- Released under the MIT license. See Copyright Notice in luajit.h
----------------------------------------------------------------------------
local a=require((string.match(...,".*%.")or"").."dis_mips")return{create=a.create_r6_el,disass=a.disass_r6_el,regname=a.regname}