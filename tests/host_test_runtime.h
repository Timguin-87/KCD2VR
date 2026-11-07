#pragma once
#ifdef _MSC_VER
#include <cstdlib>
namespace kcd2vr_test {
struct ErrorOutput {ErrorOutput(){_set_error_mode(_OUT_TO_STDERR);_set_abort_behavior(0,_WRITE_ABORT_MSG|_CALL_REPORTFAULT);}};
inline ErrorOutput errorOutput;
}
#endif
