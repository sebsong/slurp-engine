#pragma once

#include "Debug.h"

#define DLL_EXPORT
#if PLATFORM_WINDOWS
#undef DLL_EXPORT
#define DLL_EXPORT __declspec( dllexport )
#endif

// NOTE: Define dynamic types for hot reloading
#define NO_ARG
#if DEBUG
#define _SLURP_DECLARE_DYNAMIC(fnMacro, fnName, fnPrefix, stubReturn) \
typedef fnMacro(dyn_##fnName);                                        \
fnMacro(stub_##fnName){                                               \
    ASSERT(false);                                                    \
    stubReturn                                                        \
}                                                                     \
fnPrefix fnMacro(fnName);
#else
#define _SLURP_DECLARE_DYNAMIC(fnMacro, fnName, fnPrefix, stubReturn) \
typedef fnMacro(dyn_##fnName);                                        \
fnMacro(stub_##fnName){ stubReturn }                                  \
fnPrefix fnMacro(fnName);
#endif
#define SLURP_DECLARE_DYNAMIC_VOID(fnMacro, fnName) _SLURP_DECLARE_DYNAMIC(fnMacro, fnName, NO_ARG, NO_ARG)
#define SLURP_DECLARE_DYNAMIC_RETURN(fnMacro, fnName, stubReturn) _SLURP_DECLARE_DYNAMIC(fnMacro, fnName, NO_ARG, return stubReturn;)
#define SLURP_DECLARE_DYNAMIC_DLL_VOID(fnMacro, fnName) _SLURP_DECLARE_DYNAMIC(fnMacro, fnName, extern "C" DLL_EXPORT, NO_ARG)
