#pragma once
// Initialize on the mod worker BEFORE installing any DetourModKit hook.
// All ordinary records use the process logger; its only file is KCD2VR.log.
#include <DetourModKit/logger.hpp>
#include <DetourModKit/async_logger_config.hpp>
#include <windows.h>
#include <atomic>
#include <cstring>
#include <mutex>

namespace kcd2vr::diagnostics::unified_log_d81 {
inline bool Initialize() noexcept {
    static std::once_flag once;
    static std::atomic<bool> ready{false};
    try {
        std::call_once(once, [] {
            char path[MAX_PATH * 2]{}; HMODULE module=nullptr;
            if(GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<LPCSTR>(&Initialize), &module)) {
                const auto n=GetModuleFileNameA(module,path,DWORD(sizeof(path)));
                if(!n||n>=sizeof(path))path[0]=0;
            }
            char* slash=std::strrchr(path,'\\');
            if(!slash)slash=std::strrchr(path,'/');
            const auto offset=slash?std::size_t(slash+1-path):0;
            constexpr char leaf[]="KCD2VR.log";
            if(offset+sizeof(leaf)>sizeof(path))return;
            std::memcpy(path+offset,leaf,sizeof(leaf));
            // Begin one session, then keep append semantics for the crash-only
            // emergency handle too. A config reload never truncates this file.
            HANDLE file=CreateFileA(path,GENERIC_WRITE,FILE_SHARE_READ|FILE_SHARE_WRITE|
                FILE_SHARE_DELETE,nullptr,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
            if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);
            DetourModKit::Logger::configure("KCD2VR",path,DetourModKit::DEFAULT_TIMESTAMP_FORMAT,
                DetourModKit::LogOpenMode::Append);
            DetourModKit::AsyncLoggerConfig config{};
            config.overflow_policy=DetourModKit::OverflowPolicy::DropNewest;
            DetourModKit::log().enable_async_mode(config);
            ready.store(DetourModKit::log().is_async_mode_enabled(),std::memory_order_release);
        });
    } catch(...) {return false;}
    return ready.load(std::memory_order_acquire);
}
}
