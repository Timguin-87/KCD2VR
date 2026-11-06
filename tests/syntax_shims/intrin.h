#pragma once
inline void* hostReturnAddressValue=nullptr;
#ifdef _MSC_VER
// The MSVC CRT includes intrin.h for vector types even in host-shim tests.
#include <immintrin.h>
inline void* HostReturnAddress(){return hostReturnAddressValue;}
#else
inline void* _ReturnAddress(){return hostReturnAddressValue;}
#endif
