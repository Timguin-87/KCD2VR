#pragma once
#include <cstddef>
#include <cstdint>
#include <cstring>

// V66.02. Portable, read-only snapshot logic. Offsets are for the byte-gated
// bdf8f9e4... KCD2 image only. No CryEngine SDK objects are constructed or called.
namespace kcd2vr::preflight_model {
constexpr std::uintptr_t kStandardRva = 0x57362C;
constexpr std::uintptr_t kStandardVtableRva = 0x3DFEBD0;
constexpr std::uintptr_t kCrvVtableRva = 0x3DF25C0;
constexpr std::uint32_t kTimestamp = 0x6A350E20;
constexpr std::uint32_t kImageSize = 0x5B2D000;
constexpr std::size_t kCameraSize = 0x310;
constexpr std::size_t kCameraEyeTag = 0x20C;
constexpr std::uintptr_t kCameraBanks[2] = {0x8EE8, 0x9508};
constexpr std::uintptr_t kVelocitySlots[2] = {0x858, 0x860};

enum class ResourceStatus : unsigned {
    Unreadable, NoObject, NoBacking, NoDimensions, NoViews, Changed,
    BasicFieldsPresent
};
constexpr const char* Name(ResourceStatus s) noexcept {
    switch(s) {
    case ResourceStatus::NoObject: return "NO-OBJECT";
    case ResourceStatus::NoBacking: return "NO-BACKING";
    case ResourceStatus::NoDimensions: return "NO-DIMENSIONS";
    case ResourceStatus::NoViews: return "NO-VIEW-TABLE";
    case ResourceStatus::Changed: return "CHANGED-DURING-READ";
    case ResourceStatus::BasicFieldsPresent: return "BASIC-FIELDS-PRESENT";
    default: return "UNREADABLE";
    }
}
struct Texture {
    std::uintptr_t object{}, backing{}, views{};
    std::uint16_t width{}, height{};
    bool objectRead{}, backingRead{}, dimensionsRead{}, viewsRead{}, stable{};
    ResourceStatus status{ResourceStatus::Unreadable};
};
struct Camera {
    bool readable{}, stable{};
    std::uint32_t eyeTag{~0u};
    std::uint64_t hash{};
};
struct Snapshot {
    std::uintptr_t pipeline{}, crv{}, resourceOwner{}, output{};
    std::uint32_t frame{}, selector{~0u}, phase{}, dispatchMode{}, deferSetting{};
    std::uint64_t builtCount{};
    std::uint8_t scheduleEnabled{};
    bool qualified{}, ownerMatches{}, stateRead{}, outputRead{};
    Texture velocity[2]{};
    Camera camera[2][2]{}; // bank: current/previous, then physical eye slot
    bool builtRead[2]{};
    std::uint64_t builtHash[2]{};
};

inline std::uint64_t Hash(const unsigned char* p, std::size_t size) noexcept {
    std::uint64_t h = 14695981039346656037ull;
    for(std::size_t i=0;i<size;++i) { h ^= p[i]; h *= 1099511628211ull; }
    return h;
}
// Reader signature: bool(address, destination, number_of_bytes). It must return
// false on failed/partial reads. A readable address is NOT a lifetime guarantee.
template<class Reader, class T>
bool Read(Reader& read, std::uintptr_t base, std::uintptr_t offset, T& out) noexcept {
    if(!base || base > ~std::uintptr_t{0}-offset ||
       base+offset > ~std::uintptr_t{0}-(sizeof(T)-1)) return false;
    T value{};
    if(!read(base+offset, &value, sizeof(value))) return false;
    out=value; return true;
}

template<class Reader>
Texture CaptureTexture(Reader& read, std::uintptr_t owner, std::uintptr_t slot) noexcept {
    Texture t{};
    t.objectRead=Read(read,owner,slot,t.object);
    if(!t.objectRead) return t;
    if(!t.object) { t.status=ResourceStatus::NoObject; return t; }
    t.backingRead=Read(read,t.object,0x80,t.backing);
    t.dimensionsRead=Read(read,t.object,0x90,t.width) && Read(read,t.object,0x92,t.height);
    if(t.backingRead && t.backing) t.viewsRead=Read(read,t.backing,0x28,t.views);
    // Never call the engine's view getter: it may allocate or take a native lock.
    // The table pointer itself is all that this non-owning snapshot can establish.
    std::uintptr_t objectAgain{},backingAgain{},viewsAgain{};
    std::uint16_t widthAgain{},heightAgain{};
    t.stable=Read(read,owner,slot,objectAgain) && objectAgain==t.object &&
        Read(read,t.object,0x80,backingAgain) && backingAgain==t.backing &&
        Read(read,t.object,0x90,widthAgain) && widthAgain==t.width &&
        Read(read,t.object,0x92,heightAgain) && heightAgain==t.height;
    if(t.viewsRead) t.stable=t.stable && Read(read,t.backing,0x28,viewsAgain) && viewsAgain==t.views;
    if(!t.backingRead || !t.dimensionsRead || (t.backing && !t.viewsRead)) return t;
    if(!t.stable) { t.status=ResourceStatus::Changed; return t; }
    if(!t.backing) t.status=ResourceStatus::NoBacking;
    else if(!t.width || !t.height) t.status=ResourceStatus::NoDimensions;
    else if(!t.views) t.status=ResourceStatus::NoViews;
    else t.status=ResourceStatus::BasicFieldsPresent;
    return t; // This is NEVER an authorization to render eye 1.
}

template<class Reader>
Camera CaptureCamera(Reader& read, std::uintptr_t crv, unsigned bank, unsigned eye) noexcept {
    Camera c{};
    if(bank>1 || eye>1 || !crv) return c;
    const auto off=kCameraBanks[bank]+eye*kCameraSize;
    if(crv>~std::uintptr_t{0}-off-kCameraSize) return c;
    unsigned char bytes[kCameraSize]{},again[kCameraSize]{};
    c.readable=read(crv+off,bytes,sizeof(bytes));
    if(!c.readable) return c;
    std::memcpy(&c.eyeTag,bytes+kCameraEyeTag,sizeof(c.eyeTag));
    c.hash=Hash(bytes,sizeof(bytes));
    c.stable=read(crv+off,again,sizeof(again)) && std::memcmp(bytes,again,sizeof(bytes))==0;
    return c; // No camera producer/frame epoch is inferred from this hash.
}

template<class Reader>
Snapshot Capture(Reader& read, std::uintptr_t pipeline, std::uintptr_t wh) noexcept {
    Snapshot s{}; s.pipeline=pipeline;
    std::uintptr_t vt{},cvt{}; std::uint32_t viewType=~0u;
    if(!wh || !Read(read,pipeline,0,vt) || vt!=wh+kStandardVtableRva ||
       !Read(read,pipeline,0x1978,s.crv) || !Read(read,s.crv,0,cvt) || cvt!=wh+kCrvVtableRva ||
       !Read(read,s.crv,0x14,viewType) || viewType!=0) return s;
    s.qualified=true;
    s.ownerMatches=Read(read,s.crv,0x8D60,s.resourceOwner) && s.resourceOwner==pipeline;
    s.outputRead=Read(read,s.crv,0x8D78,s.output);
    s.stateRead=Read(read,s.crv,0x2C,s.frame) && Read(read,s.crv,0x8EE0,s.selector) &&
        Read(read,s.crv,0xA330,s.builtCount) && Read(read,pipeline,0x1974,s.phase) &&
        Read(read,pipeline,0x1868,s.dispatchMode) && Read(read,pipeline,0x18F4,s.deferSetting) &&
        Read(read,pipeline,0x1A0,s.scheduleEnabled);
    if(s.ownerMatches) for(unsigned eye=0;eye<2;++eye)
        s.velocity[eye]=CaptureTexture(read,s.resourceOwner,kVelocitySlots[eye]);
    for(unsigned bank=0;bank<2;++bank) for(unsigned eye=0;eye<2;++eye)
        s.camera[bank][eye]=CaptureCamera(read,s.crv,bank,eye);
    if(s.stateRead && s.builtCount>0 && s.builtCount<=2) {
        for(unsigned i=0;i<static_cast<unsigned>(s.builtCount);++i) {
            unsigned char b[0x400]{};
            s.builtRead[i]=read(s.crv+0x9B30+i*0x400,b,sizeof(b));
            if(s.builtRead[i]) s.builtHash[i]=Hash(b,sizeof(b));
        }
    }
    return s;
}

struct Progress {
    static constexpr unsigned kSamples=64;
    static constexpr std::uint64_t kSamplePeriodMs=100;
    static constexpr std::uint64_t kTimeoutMs=30000;
    unsigned samples{};
    std::uint64_t startedMs{},lastSampleMs{};
    void Reset(std::uint64_t now) noexcept {samples=0;startedMs=now;lastSampleMs=0;}
    bool Due(std::uint64_t now) const noexcept {
        return samples<8 || (now>=lastSampleMs && now-lastSampleMs>=kSamplePeriodMs);
    }
    void Record(std::uint64_t now) noexcept {++samples;lastSampleMs=now;}
    bool Complete() const noexcept {return samples>=kSamples;}
    bool TimedOut(std::uint64_t now) const noexcept {return now>=startedMs && now-startedMs>=kTimeoutMs;}
};
} // namespace kcd2vr::preflight_model
