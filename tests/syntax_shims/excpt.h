#pragma once
// HOST SYNTAX ONLY. C++ catch does not emulate Windows SEH/access violations.
// Tests using these shims must not claim native exception behavior was executed.
#ifndef __try
#define __try try
#endif
#ifndef __except
#define __except(x) catch(...)
#endif
#ifndef EXCEPTION_EXECUTE_HANDLER
#define EXCEPTION_EXECUTE_HANDLER 1
#endif
