// Real Windows BCrypt/file API, no WHGame loading or execution.
#include "../src/retail_build112.cpp"
#include <cstdio>
int wmain(int argc,wchar_t** argv){
    using namespace kcd2vr::retail_build112;
    if(Ready(0)||Verify(0)||Verify(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr))))return 1;
    if(argc==2){
        auto f=CreateFileW(argv[1],GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr);
        if(f==INVALID_HANDLE_VALUE)return 2;
        const bool ok=Matches(f);CloseHandle(f);if(!ok)return 3;
        // A disk hash alone must never authorize an unrelated loaded module.
        if(Ready(reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr))))return 4;
    }
    std::puts("PASS production SHA verifier; retail DLL read as bytes only");return 0;
}
