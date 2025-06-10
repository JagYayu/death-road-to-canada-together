#pragma once

#ifdef _MSC_VER

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif

#ifndef INTERFACE
#define INTERFACE __interface
#endif

#else

#ifndef FORCEINLINE
#define FORCEINLINE inline
#endif

#ifndef INTERFACE
#define INTERFACE struct
#endif

#endif
