#pragma once
// Hook patching remains a fixture leaf. Its Result/Error shape matches the
// installed DMK4.2 API; a separate /std:c++latest build uses its real error.hpp.
#include <optional>
#include <cstdint>
#include <string_view>
#include <utility>
#ifdef KCD2VR_REAL_DMK_ERROR
#include <DetourModKit/error.hpp>
#else
namespace DetourModKit {
enum class ErrorCode:unsigned {BackendFailed,EnableFailed,MidHookCapacityExhausted};
struct Error {ErrorCode code=ErrorCode::BackendFailed;const char* where="";std::uintptr_t detail=0;std::uint32_t extra=0;};
inline std::string_view to_string(ErrorCode code) noexcept {
 return code==ErrorCode::MidHookCapacityExhausted?"MidHookCapacityExhausted":code==ErrorCode::EnableFailed?"EnableFailed":"BackendFailed";
}
}
#endif
namespace kcd2vr_test {
template<class T> struct DmkResult {
 std::optional<T> value;DetourModKit::Error failure{};
 DmkResult(T v):value(std::move(v)){} DmkResult(DetourModKit::Error e):failure(e){}
 explicit operator bool()const noexcept{return value.has_value();}
 T& operator*() noexcept{return *value;}T* operator->() noexcept{return &*value;}
 const DetourModKit::Error& error()const noexcept{return failure;}
};
struct DmkOperation {
 bool success=true;DetourModKit::Error failure{};
 DmkOperation()=default;DmkOperation(DetourModKit::Error e):success(false),failure(e){}
 explicit operator bool()const noexcept{return success;}
 const DetourModKit::Error& error()const noexcept{return failure;}
};
}
