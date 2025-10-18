----------------------------------------------------------------------------
-- LuaJIT module to save/list bytecode.
--
-- Copyright (C) 2005-2022 Mike Pall. All rights reserved.
-- Released under the MIT license. See Copyright Notice in luajit.h
----------------------------------------------------------------------------
local a=require("jit")assert(a.version_num==20100,"LuaJIT core/library version mismatch")local b=require("bit")local c="luaJIT_BC_"local type,assert=type,assert;local d=string.format;local e,f=table.remove,table.concat;local function g()io.stderr:write[[
Save LuaJIT bytecode: luajit -b[options] input output
  -l        Only list bytecode.
  -s        Strip debug info (default).
  -g        Keep debug info.
  -n name   Set module name (default: auto-detect from input name).
  -t type   Set output file type (default: auto-detect from output name).
  -a arch   Override architecture for object files (default: native).
  -o os     Override OS for object files (default: native).
  -F name   Override filename (default: input filename).
  -e chunk  Use chunk string as input.
  --        Stop handling options.
  -         Use stdin as input and/or stdout as output.

File types: c h obj o raw (default)
]]os.exit(1)end;local function h(i,...)if i then return i,...end;io.stderr:write("luajit: ",...)io.stderr:write("\n")os.exit(1)end;local function j(k,l)if type(l)=="function"then return l end;if k.filename then local m;if l=="-"then m=io.stdin:read("*a")else local n=assert(io.open(l,"rb"))m=assert(n:read("*a"))assert(n:close())end;return h(load(m,k.filename))else if l=="-"then l=nil end;return h(loadfile(l))end end;local function o(p,q)if p=="-"then return io.stdout end;return h(io.open(p,q))end;local function r(s)s.cdef[[int _setmode(int fd, int mode);]]s.C._setmode(1,0x8000)end;local t={raw="raw",c="c",h="h",o="obj",obj="obj"}local u={x86={e="le",b=32,m=3,p=0x14c},x64={e="le",b=64,m=62,p=0x8664},arm={e="le",b=32,m=40,p=0x1c0},arm64={e="le",b=64,m=183,p=0xaa64},arm64be={e="be",b=64,m=183},ppc={e="be",b=32,m=20},mips={e="be",b=32,m=8,f=0x50001006},mipsel={e="le",b=32,m=8,f=0x50001006},mips64={e="be",b=64,m=8,f=0x80000007},mips64el={e="le",b=64,m=8,f=0x80000007},mips64r6={e="be",b=64,m=8,f=0xa0000407},mips64r6el={e="le",b=64,m=8,f=0xa0000407}}local v={linux=true,windows=true,osx=true,freebsd=true,netbsd=true,openbsd=true,dragonfly=true,solaris=true}local function w(x,y,z)x=x:lower()local A=h(y[x],"unknown ",z)return type(A)=="string"and A or x end;local function B(x)local C=x:lower():match("%.(%a+)$")return t[C]or"raw"end;local function D(x)h(x:match("^[%w_.%-]+$"),"bad module name")return x:gsub("[%.%-]","_")end;local function E(x)if type(x)=="string"then local F=x:match("[^/\\]+$")if F then x=F end;local G=x:match("^(.*)%.[^.]*$")if G then x=G end;x=x:match("^[%w_.%-]+")else x=nil end;h(x,"cannot derive module name, use -n name")return x:gsub("[%.%-]","_")end;local function H(n,I,A)local i,z=n:write(A)if i and I~="-"then i,z=n:close()end;h(i,"cannot write ",I,": ",z)end;local function J(I,A)if I=="-"and a.os=="Windows"then local i,s=pcall(require,"ffi")h(i,"FFI library required to write binary file to stdout")r(s)end;local n=o(I,"wb")H(n,I,A)end;local function K(k,I,A)local n=o(I,"w")if k.type=="c"then n:write(d([[
#ifdef __cplusplus
extern "C"
#endif
#ifdef _WIN32
__declspec(dllexport)
#endif
const unsigned char %s%s[] = {
]],c,k.modname))else n:write(d([[
#define %s%s_SIZE %d
static const unsigned char %s%s[] = {
]],c,k.modname,#A,c,k.modname))end;local L,M,N={},0,0;for O=1,#A do local P=tostring(string.byte(A,O))N=N+#P+1;if N>78 then n:write(f(L,",",1,M),",\n")M,N=0,#P+1 end;M=M+1;L[M]=P end;H(n,I,f(L,",",1,M).."\n};\n")end;local function Q(k,I,A,s)s.cdef[[
typedef struct {
  uint8_t emagic[4], eclass, eendian, eversion, eosabi, eabiversion, epad[7];
  uint16_t type, machine;
  uint32_t version;
  uint32_t entry, phofs, shofs;
  uint32_t flags;
  uint16_t ehsize, phentsize, phnum, shentsize, shnum, shstridx;
} ELF32header;
typedef struct {
  uint8_t emagic[4], eclass, eendian, eversion, eosabi, eabiversion, epad[7];
  uint16_t type, machine;
  uint32_t version;
  uint64_t entry, phofs, shofs;
  uint32_t flags;
  uint16_t ehsize, phentsize, phnum, shentsize, shnum, shstridx;
} ELF64header;
typedef struct {
  uint32_t name, type, flags, addr, ofs, size, link, info, align, entsize;
} ELF32sectheader;
typedef struct {
  uint32_t name, type;
  uint64_t flags, addr, ofs, size;
  uint32_t link, info;
  uint64_t align, entsize;
} ELF64sectheader;
typedef struct {
  uint32_t name, value, size;
  uint8_t info, other;
  uint16_t sectidx;
} ELF32symbol;
typedef struct {
  uint32_t name;
  uint8_t info, other;
  uint16_t sectidx;
  uint64_t value, size;
} ELF64symbol;
typedef struct {
  ELF32header hdr;
  ELF32sectheader sect[6];
  ELF32symbol sym[2];
  uint8_t space[4096];
} ELF32obj;
typedef struct {
  ELF64header hdr;
  ELF64sectheader sect[6];
  ELF64symbol sym[2];
  uint8_t space[4096];
} ELF64obj;
]]local R=c..k.modname;local S=assert(u[k.arch])local T,U=S.b==64,S.e=="be"local function V(W)return W end;local X,Y=V,V;if s.abi("be")~=U then V=b.bswap;function X(W)return b.rshift(b.bswap(W),16)end;if T then local Z=s.cast("int64_t",2^32)function Y(W)return b.bswap(W)*Z end else Y=V end end;local _=s.new(T and"ELF64obj"or"ELF32obj")local a0=_.hdr;if k.os=="bsd"or k.os=="other"then local a1=assert(io.open("/bin/ls","rb"))local a2=a1:read(9)a1:close()s.copy(_,a2,9)h(a0.emagic[0]==127,"no support for writing native object files")else a0.emagic="\127ELF"a0.eosabi=({freebsd=9,netbsd=2,openbsd=12,solaris=6})[k.os]or 0 end;a0.eclass=T and 2 or 1;a0.eendian=U and 2 or 1;a0.eversion=1;a0.type=X(1)a0.machine=X(S.m)a0.flags=V(S.f or 0)a0.version=V(1)a0.shofs=Y(s.offsetof(_,"sect"))a0.ehsize=X(s.sizeof(a0))a0.shentsize=X(s.sizeof(_.sect[0]))a0.shnum=X(6)a0.shstridx=X(2)local a3,a4=s.offsetof(_,"space"),1;for O,p in ipairs{".symtab",".shstrtab",".strtab",".rodata",".note.GNU-stack"}do local a5=_.sect[O]a5.align=Y(1)a5.name=V(a4)s.copy(_.space+a4,p)a4=a4+#p+1 end;_.sect[1].type=V(2)_.sect[1].link=V(3)_.sect[1].info=V(1)_.sect[1].align=Y(8)_.sect[1].ofs=Y(s.offsetof(_,"sym"))_.sect[1].entsize=Y(s.sizeof(_.sym[0]))_.sect[1].size=Y(s.sizeof(_.sym))_.sym[1].name=V(1)_.sym[1].sectidx=X(4)_.sym[1].size=Y(#A)_.sym[1].info=17;_.sect[2].type=V(3)_.sect[2].ofs=Y(a3)_.sect[2].size=Y(a4)_.sect[3].type=V(3)_.sect[3].ofs=Y(a3+a4)_.sect[3].size=Y(#R+2)s.copy(_.space+a4+1,R)a4=a4+#R+2;_.sect[4].type=V(1)_.sect[4].flags=Y(2)_.sect[4].ofs=Y(a3+a4)_.sect[4].size=Y(#A)_.sect[5].type=V(1)_.sect[5].ofs=Y(a3+a4+#A)local n=o(I,"wb")n:write(s.string(_,s.sizeof(_)-4096+a4))H(n,I,A)end;local function a6(k,I,A,s)s.cdef[[
typedef struct {
  uint16_t arch, nsects;
  uint32_t time, symtabofs, nsyms;
  uint16_t opthdrsz, flags;
} PEheader;
typedef struct {
  char name[8];
  uint32_t vsize, vaddr, size, ofs, relocofs, lineofs;
  uint16_t nreloc, nline;
  uint32_t flags;
} PEsection;
typedef struct __attribute((packed)) {
  union {
    char name[8];
    uint32_t nameref[2];
  };
  uint32_t value;
  int16_t sect;
  uint16_t type;
  uint8_t scl, naux;
} PEsym;
typedef struct __attribute((packed)) {
  uint32_t size;
  uint16_t nreloc, nline;
  uint32_t cksum;
  uint16_t assoc;
  uint8_t comdatsel, unused[3];
} PEsymaux;
typedef struct {
  PEheader hdr;
  PEsection sect[2];
  // Must be an even number of symbol structs.
  PEsym sym0;
  PEsymaux sym0aux;
  PEsym sym1;
  PEsymaux sym1aux;
  PEsym sym2;
  PEsym sym3;
  uint32_t strtabsize;
  uint8_t space[4096];
} PEobj;
]]local R=c..k.modname;local S=assert(u[k.arch])local T=S.b==64;local a7="   /EXPORT:"..R..",DATA "local function V(W)return W end;local X=V;if s.abi("be")then V=b.bswap;function X(W)return b.rshift(b.bswap(W),16)end end;local _=s.new("PEobj")local a0=_.hdr;a0.arch=X(assert(S.p))a0.nsects=X(2)a0.symtabofs=V(s.offsetof(_,"sym0"))a0.nsyms=V(6)_.sect[0].name=".drectve"_.sect[0].size=V(#a7)_.sect[0].flags=V(0x00100a00)_.sym0.sect=X(1)_.sym0.scl=3;_.sym0.name=".drectve"_.sym0.naux=1;_.sym0aux.size=V(#a7)_.sect[1].name=".rdata"_.sect[1].size=V(#A)_.sect[1].flags=V(0x40300040)_.sym1.sect=X(2)_.sym1.scl=3;_.sym1.name=".rdata"_.sym1.naux=1;_.sym1aux.size=V(#A)_.sym2.sect=X(2)_.sym2.scl=2;_.sym2.nameref[1]=V(4)_.sym3.sect=X(-1)_.sym3.scl=2;_.sym3.value=V(1)_.sym3.name="@feat.00"s.copy(_.space,R)local a4=#R+1;_.strtabsize=V(a4+4)_.sect[0].ofs=V(s.offsetof(_,"space")+a4)s.copy(_.space+a4,a7)a4=a4+#a7;_.sect[1].ofs=V(s.offsetof(_,"space")+a4)local n=o(I,"wb")n:write(s.string(_,s.sizeof(_)-4096+a4))H(n,I,A)end;local function a8(k,I,A,s)s.cdef[[
typedef struct
{
  uint32_t magic, cputype, cpusubtype, filetype, ncmds, sizeofcmds, flags;
} mach_header;
typedef struct
{
  mach_header; uint32_t reserved;
} mach_header_64;
typedef struct {
  uint32_t cmd, cmdsize;
  char segname[16];
  uint32_t vmaddr, vmsize, fileoff, filesize;
  uint32_t maxprot, initprot, nsects, flags;
} mach_segment_command;
typedef struct {
  uint32_t cmd, cmdsize;
  char segname[16];
  uint64_t vmaddr, vmsize, fileoff, filesize;
  uint32_t maxprot, initprot, nsects, flags;
} mach_segment_command_64;
typedef struct {
  char sectname[16], segname[16];
  uint32_t addr, size;
  uint32_t offset, align, reloff, nreloc, flags;
  uint32_t reserved1, reserved2;
} mach_section;
typedef struct {
  char sectname[16], segname[16];
  uint64_t addr, size;
  uint32_t offset, align, reloff, nreloc, flags;
  uint32_t reserved1, reserved2, reserved3;
} mach_section_64;
typedef struct {
  uint32_t cmd, cmdsize, symoff, nsyms, stroff, strsize;
} mach_symtab_command;
typedef struct {
  int32_t strx;
  uint8_t type, sect;
  int16_t desc;
  uint32_t value;
} mach_nlist;
typedef struct {
  int32_t strx;
  uint8_t type, sect;
  uint16_t desc;
  uint64_t value;
} mach_nlist_64;
typedef struct
{
  int32_t magic, nfat_arch;
} mach_fat_header;
typedef struct
{
  int32_t cputype, cpusubtype, offset, size, align;
} mach_fat_arch;
typedef struct {
  struct {
    mach_header hdr;
    mach_segment_command seg;
    mach_section sec;
    mach_symtab_command sym;
  } arch[1];
  mach_nlist sym_entry;
  uint8_t space[4096];
} mach_obj;
typedef struct {
  struct {
    mach_header_64 hdr;
    mach_segment_command_64 seg;
    mach_section_64 sec;
    mach_symtab_command sym;
  } arch[1];
  mach_nlist_64 sym_entry;
  uint8_t space[4096];
} mach_obj_64;
typedef struct {
  mach_fat_header fat;
  mach_fat_arch fat_arch[2];
  struct {
    mach_header hdr;
    mach_segment_command seg;
    mach_section sec;
    mach_symtab_command sym;
  } arch[2];
  mach_nlist sym_entry;
  uint8_t space[4096];
} mach_fat_obj;
typedef struct {
  mach_fat_header fat;
  mach_fat_arch fat_arch[2];
  struct {
    mach_header_64 hdr;
    mach_segment_command_64 seg;
    mach_section_64 sec;
    mach_symtab_command sym;
  } arch[2];
  mach_nlist_64 sym_entry;
  uint8_t space[4096];
} mach_fat_obj_64;
]]local R='_'..c..k.modname;local a9,T,aa,ab=false,false,4,"mach_obj"if k.arch=="x64"then T,aa,ab=true,8,"mach_obj_64"elseif k.arch=="arm"then a9,ab=true,"mach_fat_obj"elseif k.arch=="arm64"then T,aa,a9,ab=true,8,true,"mach_fat_obj_64"else h(k.arch=="x86","unsupported architecture for OSX")end;local function ac(ad,ae)return b.band(ad+ae-1,-ae)end;local af=b.bswap;local _=s.new(ab)local ag=ac(s.offsetof(_,"space")+#R+2,aa)local ah=({x86={7},x64={0x01000007},arm={7,12},arm64={0x01000007,0x0100000c}})[k.arch]local ai=({x86={3},x64={3},arm={3,9},arm64={3,0}})[k.arch]if a9 then _.fat.magic=af(0xcafebabe)_.fat.nfat_arch=af(#ai)end;for O=0,#ai-1 do local a4=0;if a9 then local ae=_.fat_arch[O]ae.cputype=af(ah[O+1])ae.cpusubtype=af(ai[O+1])a4=s.offsetof(_,"arch")+O*s.sizeof(_.arch[0])ae.offset=af(a4)ae.size=af(ag-a4+#A)end;local ae=_.arch[O]ae.hdr.magic=T and 0xfeedfacf or 0xfeedface;ae.hdr.cputype=ah[O+1]ae.hdr.cpusubtype=ai[O+1]ae.hdr.filetype=1;ae.hdr.ncmds=2;ae.hdr.sizeofcmds=s.sizeof(ae.seg)+s.sizeof(ae.sec)+s.sizeof(ae.sym)ae.seg.cmd=T and 0x19 or 0x1;ae.seg.cmdsize=s.sizeof(ae.seg)+s.sizeof(ae.sec)ae.seg.vmsize=#A;ae.seg.fileoff=ag-a4;ae.seg.filesize=#A;ae.seg.maxprot=1;ae.seg.initprot=1;ae.seg.nsects=1;s.copy(ae.sec.sectname,"__data")s.copy(ae.sec.segname,"__DATA")ae.sec.size=#A;ae.sec.offset=ag-a4;ae.sym.cmd=2;ae.sym.cmdsize=s.sizeof(ae.sym)ae.sym.symoff=s.offsetof(_,"sym_entry")-a4;ae.sym.nsyms=1;ae.sym.stroff=s.offsetof(_,"sym_entry")+s.sizeof(_.sym_entry)-a4;ae.sym.strsize=ac(#R+2,aa)end;_.sym_entry.type=0xf;_.sym_entry.sect=1;_.sym_entry.strx=1;s.copy(_.space+1,R)local n=o(I,"wb")n:write(s.string(_,ag))H(n,I,A)end;local function aj(k,I,A)local i,s=pcall(require,"ffi")h(i,"FFI library required to write this file type")if I=="-"and a.os=="Windows"then r(s)end;if k.os=="windows"then return a6(k,I,A,s)elseif k.os=="osx"then return a8(k,I,A,s)else return Q(k,I,A,s)end end;local function ak(k,l,I)local al=j(k,l)require("jit.bc").dump(al,o(I,"w"),true)end;local function am(k,l,I)local al=j(k,l)local A=string.dump(al,k.strip)local L=k.type;if not L then L=B(I)k.type=L end;if L=="raw"then J(I,A)else if not k.modname then k.modname=E(l)end;if L=="obj"then aj(k,I,A)else K(k,I,A)end end end;local function an(...)local ao={...}local M=1;local ap=false;local k={strip=true,arch=a.arch,os=a.os:lower(),type=false,modname=false}while M<=#ao do local ae=ao[M]if type(ae)=="string"and ae:sub(1,1)=="-"and ae~="-"then e(ao,M)if ae=="--"then break end;for N=2,#ae do local aq=ae:sub(N,N)if aq=="l"then ap=true elseif aq=="s"then k.strip=true elseif aq=="g"then k.strip=false else if ao[M]==nil or N~=#ae then g()end;if aq=="e"then if M~=1 then g()end;ao[1]=h(loadstring(ao[1]))elseif aq=="n"then k.modname=D(e(ao,M))elseif aq=="t"then k.type=w(e(ao,M),t,"file type")elseif aq=="a"then k.arch=w(e(ao,M),u,"architecture")elseif aq=="o"then k.os=w(e(ao,M),v,"OS name")elseif aq=="F"then k.filename="@"..e(ao,M)else g()end end end else M=M+1 end end;if ap then if#ao==0 or#ao>2 then g()end;ak(k,ao[1],ao[2]or"-")else if#ao~=2 then g()end;am(k,ao[1],ao[2])end end;return{start=an}