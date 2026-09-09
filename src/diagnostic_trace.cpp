#include "diagnostic_trace.h"
#include <windows.h>
#include <cstdio>
#include <algorithm>
#include <cstring>
#include <string>

namespace kcd2vr::diagnostic_trace {
namespace {
std::wstring DiagnosticPath() noexcept {
    HMODULE self = nullptr;
    wchar_t path[MAX_PATH]{};
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                           reinterpret_cast<LPCWSTR>(&Write), &self) &&
        GetModuleFileNameW(self, path, MAX_PATH)) {
        wchar_t* slash = wcsrchr(path, L'\\');
        if (slash) {
            *(slash + 1) = L'\0';
            wcscat_s(path, L"KCD2VR-diagnostics.log");
            return path;
        }
    }
    return L"KCD2VR-diagnostics.log";
}
}

void Write(const char* message) noexcept {
    if(!message || !*message) return;
    const std::wstring path = DiagnosticPath();
    HANDLE h = CreateFileW(path.c_str(), FILE_APPEND_DATA,
        FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS,
        FILE_ATTRIBUTE_NORMAL, nullptr);
    if(h==INVALID_HANDLE_VALUE) return;
    SYSTEMTIME st{}; GetLocalTime(&st);
    char line[4096]{};
    const int n = std::snprintf(line,sizeof(line),
        "[%04u-%02u-%02u %02u:%02u:%02u.%03u] %s\r\n",
        st.wYear,st.wMonth,st.wDay,st.wHour,st.wMinute,st.wSecond,st.wMilliseconds,message);
    if(n>0) {
        DWORD written=0;
        WriteFile(h,line,static_cast<DWORD>(std::min<int>(n,static_cast<int>(sizeof(line)-1))),&written,nullptr);
    }
    CloseHandle(h);
}
}
