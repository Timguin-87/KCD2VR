#pragma once
// Decoder shared verbatim by production and executable tests.
// Exact retail contracts: FIFO at anim+layer*0x40+0x48, count at entries-4,
// stride 0x78, ID +0x10, segment index byte +0x16, segment time +0x44,
// activated bit +0x50. These are NOT SDK offsets transplanted by analogy.
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
namespace kcd2vr::diagnostics::playback {
constexpr std::size_t MaxLayers=16, MaxPerLayer=16, MaxEntries=128;
struct Entry {
    std::uint8_t layer{}, index{}, segment{}; bool activated{}, sampler{};
    int id{-1}; float segmentTime{},layerWeight{};
    std::uint32_t flags{},dynamicFlags{};
    std::array<std::uint32_t,8> raw18To34{}; // opaque values, NOT named weights/health
};
struct Snapshot {std::array<Entry,MaxEntries> entries{};std::size_t count{};std::uint64_t tick{},serial{};int failureLayer=-1,failureEntry=-1,failureField=0;};
enum class Result {Ok,Unreadable,Overflow,Changed,Invalid};
inline const char* ResultName(Result r) noexcept {
    switch(r){case Result::Ok:return "OK";case Result::Unreadable:return "UNREADABLE";
    case Result::Overflow:return "CAPACITY_EXCEEDED";case Result::Changed:return "FIFO_CHANGED_DURING_COPY";
    default:return "INVALID_FIELD";}
}
inline bool SameIdentity(const Entry&a,const Entry&b) noexcept {
    return a.layer==b.layer&&a.index==b.index&&a.id==b.id;
}
inline bool SameMembership(const Snapshot&a,const Snapshot&b) noexcept {
    if(a.count!=b.count)return false;
    for(std::size_t i=0;i<a.count;++i)if(!SameIdentity(a.entries[i],b.entries[i])||a.entries[i].activated!=b.entries[i].activated)return false;
    return true;
}
inline const Entry* Find(const Snapshot&s,const Entry&e) noexcept {
    for(std::size_t i=0;i<s.count;++i)if(SameIdentity(s.entries[i],e))return &s.entries[i];return nullptr;
}
inline const char* PhaseRelation(const Entry&now,const Entry*old) noexcept {
    if(!old)return "FIRST_SAMPLE";
    if(now.segment!=old->segment)return "SEGMENT_CHANGED";
    const float d=now.segmentTime-old->segmentTime;
    if(std::fabs(d)<1e-5f)return "UNCHANGED";
    return d>0?"ADVANCED":"REWOUND_OR_LOOPED";
}
template<class Reader> Result Capture(std::uintptr_t anim,Reader&& read,Snapshot&out,std::size_t layers=MaxLayers) noexcept {
    out={};if(!anim)return Result::Unreadable;if(layers<1||layers>MaxLayers)return Result::Invalid;
    std::array<std::uintptr_t,MaxLayers> pointers{};std::array<std::uint32_t,MaxLayers> counts{};
    for(std::size_t layer=0;layer<layers;++layer){
        std::uintptr_t p=0;std::uint32_t n=0;float lw=0;
        if(!read(anim+layer*0x40+0x48,&p,8))return Result::Unreadable;
        pointers[layer]=p;if(!p)continue;
        if(p<4||!read(p-4,&n,4))return Result::Unreadable;
        if(n>MaxPerLayer||out.count+n>MaxEntries)return Result::Overflow;
        counts[layer]=n;
        // Empty allocated layers have no contributing clip or meaningful layer
        // field to validate. Still verify their pointer/count in the final pass.
        if(!n)continue;
        if(!read(anim+layer*0x40+0x5c,&lw,4))return Result::Unreadable;
        if(!std::isfinite(lw)){out.failureLayer=int(layer);out.failureField=1;return Result::Invalid;}
        for(std::uint32_t i=0;i<n;++i){
            std::array<unsigned char,0x78> raw{};
            if(!read(p+std::size_t(i)*0x78,raw.data(),raw.size()))return Result::Unreadable;
            Entry e{};e.layer=std::uint8_t(layer);e.index=std::uint8_t(i);e.layerWeight=lw;
            std::memcpy(&e.id,raw.data()+0x10,4);e.segment=raw[0x16];
            std::memcpy(&e.segmentTime,raw.data()+0x44,4);
            std::memcpy(&e.flags,raw.data()+0x4c,4);std::memcpy(&e.dynamicFlags,raw.data()+0x50,4);
            std::uintptr_t sampler=0;std::memcpy(&sampler,raw.data()+8,8);e.sampler=sampler!=0;
            e.activated=(e.dynamicFlags&1u)!=0;
            std::memcpy(e.raw18To34.data(),raw.data()+0x18,32);
            if(e.id<0||!std::isfinite(e.segmentTime)||e.segmentTime<-.01f||e.segmentTime>1.01f){
                out.failureLayer=int(layer);out.failureEntry=int(i);out.failureField=e.id<0?2:!std::isfinite(e.segmentTime)?3:4;return Result::Invalid;}
            out.entries[out.count++]=e;
        }
    }
    // Detect obvious queue replacement/reorder. Not a native lock nor same-frame proof.
    for(std::size_t layer=0;layer<layers;++layer){
        std::uintptr_t p=0;std::uint32_t n=0;
        if(!read(anim+layer*0x40+0x48,&p,8))return Result::Unreadable;
        if(p!=pointers[layer])return Result::Changed;
        if(p&&(!read(p-4,&n,4)||n!=counts[layer]))return Result::Changed;
    }
    for(std::size_t i=0;i<out.count;++i){const auto&e=out.entries[i];int id=-1;
        if(!read(pointers[e.layer]+std::size_t(e.index)*0x78+0x10,&id,4)||id!=e.id)return Result::Changed;}
    return Result::Ok;
}
}
