#pragma once
// R118 (Miki 6 Nov: "running the game (dll) automatically added a default KCD2VR.ini if not in the folder (the mod
// won't start without the ini)"). The repo KCD2VR.ini is built into the DLL as RCDATA IDR_KCD2VR_DEFAULT_INI
// (src/kcd2vr_resources26.rc.in). It is written beside the DLL ONLY when no file of that name exists; an existing
// KCD2VR.ini (also an empty or broken one) is never touched. One file, no temp/backup files in the game folder.
#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <string_view>
#ifdef _WIN32
#include <windows.h>
#include "resource_ids26.h" // IDR_KCD2VR_DEFAULT_INI
#else
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace kcd2vr::core::default_ini_r118 {
namespace fs=std::filesystem;
enum class Result : int {Created,Exists,NoDefault,Failed};
inline const char* name(Result r) noexcept {
    switch(r){case Result::Created:return "CREATED";case Result::Exists:return "EXISTS";case Result::NoDefault:return "NO_EMBEDDED_DEFAULT";default:return "WRITE_FAILED";}
}
// Exclusive create: if the file appears between the check and the write (setup dialog + game at once) the other
// writer wins and this returns Exists. A failed partial write removes only the file this call created.
inline Result WriteIfMissing(const fs::path& path,std::string_view bytes) noexcept {
    try{
        std::error_code error;
        // not_found reports an error code too (libstdc++ and MSVC): only "not found" lets the write go ahead.
        const auto status=fs::symlink_status(path,error);
        if(status.type()!=fs::file_type::not_found)return status.type()==fs::file_type::none?Result::Failed:Result::Exists;
        if(bytes.empty())return Result::NoDefault;
#ifdef _WIN32
        HANDLE file=CreateFileW(path.c_str(),GENERIC_WRITE,0,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);
        if(file==INVALID_HANDLE_VALUE)return GetLastError()==ERROR_FILE_EXISTS?Result::Exists:Result::Failed;
        bool ok=true;std::size_t done=0;
        while(ok&&done<bytes.size()){
            const DWORD chunk=DWORD(std::min<std::size_t>(bytes.size()-done,1u<<20));DWORD wrote=0;
            ok=WriteFile(file,bytes.data()+done,chunk,&wrote,nullptr)!=FALSE&&wrote==chunk;done+=wrote;
        }
        ok=FlushFileBuffers(file)!=FALSE&&ok;CloseHandle(file);
        if(!ok){DeleteFileW(path.c_str());return Result::Failed;}
#else
        const int fd=::open(path.c_str(),O_WRONLY|O_CREAT|O_EXCL,0644);
        if(fd<0)return fs::exists(path,error)?Result::Exists:Result::Failed;
        bool ok=true;std::size_t done=0;
        while(ok&&done<bytes.size()){const auto n=::write(fd,bytes.data()+done,bytes.size()-done);ok=n>0;if(ok)done+=std::size_t(n);}
        ok=::close(fd)==0&&ok;
        if(!ok){::unlink(path.c_str());return Result::Failed;}
#endif
        return Result::Created;
    }catch(...){return Result::Failed;}
}
#ifdef _WIN32
// The embedded bytes of the DLL that contains this code (game ASI/dinput8 or the rundll32 setup call).
inline std::string_view Embedded() noexcept {
    HMODULE self=nullptr;
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
        reinterpret_cast<LPCWSTR>(&Embedded),&self))return {};
    HRSRC info=FindResourceW(self,MAKEINTRESOURCEW(IDR_KCD2VR_DEFAULT_INI),MAKEINTRESOURCEW(10/*RT_RCDATA*/));
    if(!info)return {};
    HGLOBAL data=LoadResource(self,info);const DWORD size=SizeofResource(self,info);
    const void* bytes=data?LockResource(data):nullptr;
    return bytes&&size?std::string_view(static_cast<const char*>(bytes),size):std::string_view{};
}
inline Result EnsureBesideModule(const fs::path& ini) noexcept {return WriteIfMissing(ini,Embedded());}
#endif
// Last startup result for the KCD2VR.log line written after the log exists (LoadConfig).
inline int lastResult=-1;
} // namespace kcd2vr::core::default_ini_r118
