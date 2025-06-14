#pragma once

#ifdef _MSC_VER

#ifndef FORCEINLINE
#define FORCEINLINE __forceinline
#endif

#else

#ifndef FORCEINLINE
#define FORCEINLINE inline
#endif

#endif
