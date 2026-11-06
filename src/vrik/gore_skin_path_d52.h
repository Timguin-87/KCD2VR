#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>
namespace kcd2vr::gore_skin_path_d52 {
// Read-only exact-retail model identity. Never scan other attachment fields for
// an arbitrary .skin string: a neighbouring beard may otherwise become hair.
enum class Result { Found, Empty, Unknown, InvalidPath };
template<class Read> Result path(Read&& read, bool retail, std::uintptr_t base,
    std::uintptr_t attachment, char* out, std::size_t cap) noexcept {
    if (!out || !cap) return Result::InvalidPath;
    out[0]=0;
    if (!retail || !base || !attachment) return Result::Unknown;
    std::uintptr_t vt=0,skin=0,model=0,getter=0;
    if (!read(attachment,vt)) return Result::Unknown;
    const std::size_t off=vt==base+0x3A65A08?0x78:vt==base+0x3A8F798?0x98:0;
    if (!off || !read(attachment+off,skin)) return Result::Unknown;
    if (!skin) return Result::Empty;
    // CSkin::GetModelFilePath at its verified +0x20 slot is mov rax,[rcx+0x28].
    if (!read(skin,vt) || vt!=base+0x406C9F0 ||
        !read(vt+0x20,getter) || getter!=base+0x113CE50 ||
        !read(skin+0x28,model)) return Result::Unknown;
    if (!model) return Result::Empty;
    for (std::size_t i=0;i<cap && i<220;++i) {
        char ch=0;
        if (!read(model+i,ch)) { out[0]=0; return Result::Unknown; }
        if (!ch) {
            out[i]=0;
            if (i>=8 && (!std::strcmp(out+i-5,".skin") || !std::strcmp(out+i-5,".SKIN"))) return Result::Found;
            out[0]=0; return Result::InvalidPath;
        }
        if (ch<32 || ch>126 || i+1==cap) { out[0]=0; return Result::InvalidPath; }
        out[i]=ch;
    }
    out[0]=0; return Result::InvalidPath;
}
}
