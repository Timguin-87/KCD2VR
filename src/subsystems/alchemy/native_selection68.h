#pragma once
// Metadata for ONE synchronous retail object-selector invocation. No engine
// calls, geometry lookup, native action or selection is created here.
#include "bridge67.h"
#include "geometry68.h"
#include <cstdint>

namespace kcd2vr::subsystems::alchemy::native_selection68 {
using Table=bridge67::NativeTable;
using Ref=near67::Ref;
inline bool registered(const Table& t,std::uint32_t id,Ref* out=nullptr) noexcept {
    if(!id)return false;
    for(const auto& r:t.refs)if(near67::valid(r)&&r.entity==id){if(out)*out=r;return true;}
    return false;
}
inline bool sameTable(const Table& a,const Table& b) noexcept {
    if(!a.instance||a.instance!=b.instance||!a.player||a.player!=b.player||
       !a.playerEntity||a.playerEntity!=b.playerEntity||!a.lifetime||a.lifetime!=b.lifetime)return false;
    for(unsigned i=0;i<near67::Capacity;++i)if(!near67::same(a.refs[i],b.refs[i]))return false;
    return true;
}
struct Candidate {std::uintptr_t entity=0;Ref ref{};geometry68::Bounds bounds{};bool valid=false;};
struct Scope {
    Table table{};std::uintptr_t interactor=0,policy=0;
    Candidate candidate{};bool active=false;
};
}
