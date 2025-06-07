#pragma once

#ifdef _MSC_VER
#define FORCEINLINE __forceinline
#define INTERFACE __interface
#else
#define FORCEINLINE inline
#define INTERFACE struct
#endif
