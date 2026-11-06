#pragma once
// Explicit unrelated Windows leaf shims for the full-publisher motion fixture.
// Hardware/retail fault handling is not exercised by this host test.
#include "syntax_shims/windows.h"
inline unsigned long GetCurrentProcessId(){return 12345;}
inline int MoveFileExW(const wchar_t*,const wchar_t*,unsigned long){return 0;}
inline unsigned GetPrivateProfileIntA(const char*,const char*,int fallback,const char*){return unsigned(fallback);}
#ifndef FALSE
#define FALSE 0
#endif
#define MOVEFILE_REPLACE_EXISTING 1
#define MOVEFILE_WRITE_THROUGH 8
#include "core/initial_draw_completion_h36r3.h"
#define __finally catch(...) { throw; } if(true)
