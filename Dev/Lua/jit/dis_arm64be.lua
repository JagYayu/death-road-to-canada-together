----------------------------------------------------------------------------
-- LuaJIT ARM64BE disassembler wrapper module.
--
-- Copyright (C) 2005-2022 Mike Pall. All rights reserved.
-- Released under the MIT license. See Copyright Notice in luajit.h
----------------------------------------------------------------------------
return require((string.match(...,".*%.")or"").."dis_arm64")