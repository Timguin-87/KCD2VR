#include "retail_build112.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <bcrypt.h>
#include <array>
#include <atomic>
#include <mutex>

namespace kcd2vr::retail_build112 {
namespace {
std::atomic<std::uintptr_t> verified{0};
std::mutex mutex;
// Keep the verified file open without write/delete sharing for this ASI's life.
struct FileLease {HANDLE file=INVALID_HANDLE_VALUE;~FileLease(){if(file!=INVALID_HANDLE_VALUE)CloseHandle(file);}} lease;
constexpr std::array<unsigned char,32> expected{
    0xbd,0xf8,0xf9,0xe4,0xa1,0x12,0x57,0xa7,0x2b,0x64,0xc8,0x47,0x00,0xe2,0x84,0xc2,
    0x9e,0x4c,0x4c,0xca,0xf5,0xb8,0xd4,0xbf,0xa7,0xd0,0xb2,0xa7,0x29,0x44,0x79,0xf7};
bool Matches(HANDLE file) noexcept {
    BCRYPT_ALG_HANDLE algorithm=nullptr;BCRYPT_HASH_HANDLE hash=nullptr;
    bool ok=BCryptOpenAlgorithmProvider(&algorithm,BCRYPT_SHA256_ALGORITHM,nullptr,0)>=0;
    if(ok)ok=BCryptCreateHash(algorithm,&hash,nullptr,0,nullptr,0,0)>=0;
    std::array<unsigned char,65536> bytes{};DWORD count=0;
    while(ok){
        if(!ReadFile(file,bytes.data(),DWORD(bytes.size()),&count,nullptr)){ok=false;break;}
        if(!count)break;
        ok=BCryptHashData(hash,bytes.data(),count,0)>=0;
    }
    std::array<unsigned char,32> digest{};
    if(ok)ok=BCryptFinishHash(hash,digest.data(),ULONG(digest.size()),0)>=0&&digest==expected;
    if(hash)BCryptDestroyHash(hash);
    if(algorithm)BCryptCloseAlgorithmProvider(algorithm,0);
    return ok;
}
}
bool Ready(std::uintptr_t module) noexcept {return module&&verified.load(std::memory_order_acquire)==module;}
bool Verify(std::uintptr_t module) noexcept {
    if(!module)return false;
    std::lock_guard<std::mutex> lock(mutex);
    if(Ready(module))return true;
    if(verified.load())return false;
    std::array<wchar_t,32768> path{};
    const auto length=GetModuleFileNameW(reinterpret_cast<HMODULE>(module),path.data(),DWORD(path.size()));
    if(!length||length>=path.size())return false;
    const auto file=CreateFileW(path.data(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,FILE_FLAG_SEQUENTIAL_SCAN,nullptr);
    if(file==INVALID_HANDLE_VALUE)return false;
    if(!Matches(file)){CloseHandle(file);return false;}
    lease.file=file;verified.store(module,std::memory_order_release);return true;
}
}
