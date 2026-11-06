#pragma once
#include "stereo_preflight_model_v6602.h"

// Exact-retail adapter data only. No SDK C++ objects, virtual calls, or resource
// writes occur in this model. A nonzero view entry does NOT prove GPU readiness.
namespace kcd2vr::resource_model {
namespace pf = preflight_model;
constexpr std::uintptr_t kHelperRva=0x73CEF4;
constexpr std::uintptr_t kTextureVtableRva=0x3A53FA0;
constexpr std::uintptr_t kDeviceTextureVtableRva=0x3A54170; // exact CDeviceTexture RTTI
constexpr std::uintptr_t kRendererGlobalRva=0x547DB60;
constexpr std::size_t kNameCapacity=256;
constexpr std::uint8_t kVelocityFormat=0x0E; // retail left-target call argument
constexpr std::uint32_t kRightInitialFlags=0x51100; // retail object initializer
struct Color { float values[4]{}; };
static_assert(sizeof(Color)==16);
struct TextureMeta {
    std::uintptr_t vtable{},namePointer{};
    std::uint32_t flags{},clearBits[4]{},nativeReferences{};
    std::int32_t customId{};
    std::uint16_t depth{};
    std::uint8_t mipCount{},type{},sourceFormat{},deviceFormat{};
    char name[kNameCapacity]{};
    bool readable{},stable{},referencesRead{};
};
struct Views {
    std::uintptr_t backingVtable{};
    std::uintptr_t begin{},end{},capacity{},rtv{};
    std::uint64_t rtvKey{};
    std::uint32_t count{};
    bool typeRead{},typeStable{},headerRead{},bounded{},entryRead{},stable{};
};
struct RenderThread {
    std::uintptr_t renderer{},threadObject{};
    std::uint32_t idF0{},idF4{},loadingMode{};
    bool readable{},matches{},stable{};
};
struct Evidence {
    TextureMeta texture[2]{};
    Views view[2]{};
    RenderThread rt{};
    std::uintptr_t resourcePipeline{};
    bool ownerRead{};
};
inline char Lower(char c) noexcept { return c>='A' && c<='Z'?static_cast<char>(c+('a'-'A')):c; }
inline bool EqualNames(const char* a,const char* b) noexcept {
    for(std::size_t i=0;i<kNameCapacity;++i) {
        if(Lower(a[i])!=Lower(b[i]))return false;
        if(!a[i])return true;
    }
    return false;
}
inline bool Prefix(const char* s,const char* prefix) noexcept {
    for(std::size_t i=0;prefix[i];++i) if(!s[i] || Lower(s[i])!=Lower(prefix[i]))return false;
    return true;
}
inline bool NamesMatchPair(const char* left,const char* right) noexcept {
    constexpr char l[]="$VelocityObjects",r[]="$VelocityObjects_R";
    return Prefix(left,l) && !Prefix(left,r) && Prefix(right,r) &&
        EqualNames(left+sizeof(l)-1,right+sizeof(r)-1);
}
template<class Reader>
bool ReadName(Reader& read,std::uintptr_t p,char (&out)[kNameCapacity]) noexcept {
    if(!p)return false;
    for(std::size_t i=0;i<kNameCapacity;++i) {
        char c=0;if(!pf::Read(read,p,i,c))return false;
        out[i]=c;
        if(c==0)return i!=0;
        if(static_cast<unsigned char>(c)<32 || static_cast<unsigned char>(c)>126)return false;
    }
    out[kNameCapacity-1]=0;return false;
}
template<class Reader>
TextureMeta CaptureMetaOnce(Reader& read,std::uintptr_t object) noexcept {
    TextureMeta m{};
    m.readable=pf::Read(read,object,0,m.vtable) && pf::Read(read,object,0x70,m.namePointer) &&
        pf::Read(read,object,0x98,m.flags) && pf::Read(read,object,0x94,m.depth) &&
        pf::Read(read,object,0xA2,m.mipCount) && pf::Read(read,object,0xA3,m.type) &&
        pf::Read(read,object,0xA4,m.sourceFormat) && pf::Read(read,object,0x96,m.deviceFormat) &&
        pf::Read(read,object,0xB8,m.customId) && object<=~std::uintptr_t{0}-0x100 &&
        read(object+0xF0,m.clearBits,sizeof(m.clearBits)) &&
        ReadName(read,m.namePointer,m.name);
    // Observational only: the registry hit increments object+0x5C. Reference
    // changes do not make this snapshot a resource-lifetime lock.
    m.referencesRead=pf::Read(read,object,0x5C,m.nativeReferences);
    return m;
}
template<class Reader>
TextureMeta CaptureMeta(Reader& read,std::uintptr_t object) noexcept {
    auto m=CaptureMetaOnce(read,object);const auto again=CaptureMetaOnce(read,object);
    m.stable=m.readable && again.readable && m.vtable==again.vtable && m.namePointer==again.namePointer &&
        m.flags==again.flags && m.depth==again.depth && m.mipCount==again.mipCount && m.type==again.type &&
        m.sourceFormat==again.sourceFormat && m.deviceFormat==again.deviceFormat && m.customId==again.customId &&
        std::memcmp(m.clearBits,again.clearBits,sizeof(m.clearBits))==0 &&
        std::memcmp(m.name,again.name,kNameCapacity)==0;
    return m;
}
template<class Reader>
Views CaptureViews(Reader& read,std::uintptr_t backing) noexcept {
    Views v{};
    if(!backing)return v;
    v.typeRead=pf::Read(read,backing,0,v.backingVtable);
    v.headerRead=pf::Read(read,backing,0x28,v.begin) && pf::Read(read,backing,0x30,v.end) &&
        pf::Read(read,backing,0x38,v.capacity);
    if(!v.headerRead)return v;
    v.bounded=v.begin && v.begin<=v.end && v.end<=v.capacity && (v.end-v.begin)%16==0 &&
        (v.capacity-v.begin)%16==0 && v.capacity-v.begin<=16*256;
    if(!v.bounded)return v;
    v.count=static_cast<std::uint32_t>((v.end-v.begin)/16);
    // Clear helper passes handle 2; each native resource-view entry is 16 bytes.
    if(v.count>2)v.entryRead=pf::Read(read,v.begin,2*16,v.rtvKey) && pf::Read(read,v.begin,2*16+8,v.rtv);
    std::uintptr_t type{},begin{},end{},cap{},entry{};std::uint64_t key{};
    v.typeStable=v.typeRead && pf::Read(read,backing,0,type) && type==v.backingVtable;
    v.stable=pf::Read(read,backing,0x28,begin) && begin==v.begin &&
        pf::Read(read,backing,0x30,end) && end==v.end && pf::Read(read,backing,0x38,cap) && cap==v.capacity;
    if(v.entryRead)v.stable=v.stable && pf::Read(read,v.begin,2*16,key) && key==v.rtvKey &&
        pf::Read(read,v.begin,2*16+8,entry) && entry==v.rtv;
    return v;
}
template<class Reader>
RenderThread CaptureThread(Reader& read,std::uintptr_t wh,std::uint32_t tid) noexcept {
    RenderThread t{};
    t.readable=pf::Read(read,wh,kRendererGlobalRva,t.renderer) &&
        pf::Read(read,t.renderer,0xF8,t.threadObject) && pf::Read(read,t.threadObject,0xF0,t.idF0) &&
        pf::Read(read,t.threadObject,0xF4,t.idF4) && pf::Read(read,t.threadObject,0x140,t.loadingMode);
    t.matches=t.readable && tid && (tid==t.idF0 || tid==t.idF4) && t.loadingMode==0;
    std::uintptr_t renderer{},thread{};std::uint32_t f0{},f4{},mode{};
    t.stable=t.readable && pf::Read(read,wh,kRendererGlobalRva,renderer) && renderer==t.renderer &&
        pf::Read(read,t.renderer,0xF8,thread) && thread==t.threadObject &&
        pf::Read(read,t.threadObject,0xF0,f0) && f0==t.idF0 &&
        pf::Read(read,t.threadObject,0xF4,f4) && f4==t.idF4 &&
        pf::Read(read,t.threadObject,0x140,mode) && mode==t.loadingMode;
    return t;
}
template<class Reader>
Evidence CaptureEvidence(Reader& read,const pf::Snapshot& s,std::uintptr_t wh,std::uint32_t tid) noexcept {
    Evidence e{};
    for(unsigned eye=0;eye<2;++eye) {
        e.texture[eye]=CaptureMeta(read,s.velocity[eye].object);
        e.view[eye]=CaptureViews(read,s.velocity[eye].backing);
    }
    e.rt=CaptureThread(read,wh,tid);
    e.ownerRead=pf::Read(read,s.pipeline,0x840+0x388,e.resourcePipeline);
    return e;
}
inline const char* Reject(const pf::Snapshot& s,const Evidence& e,std::uintptr_t wh) noexcept {
    const auto& l=s.velocity[0];const auto& r=s.velocity[1];
    const auto& lm=e.texture[0];const auto& rm=e.texture[1];
    if(!s.qualified || !s.stateRead || !s.ownerMatches || !s.outputRead || !s.output)return "main-view-state";
    if(s.selector!=0 || s.builtCount!=1)return "not-ordinary-count1-eye0";
    if(s.phase!=1 || s.scheduleEnabled || s.dispatchMode || s.deferSetting)return "scheduling-state";
    if(!e.ownerRead || e.resourcePipeline!=s.pipeline)return "resource-owner";
    if(!e.rt.matches || !e.rt.stable)return "not-verified-native-render-thread";
    if(l.status!=pf::ResourceStatus::BasicFieldsPresent || !l.stable)return "left-control-unprepared";
    if(l.width<2 || l.height<2 || l.width>16384 || l.height>16384)return "unsupported-dimensions";
    if(r.status!=pf::ResourceStatus::NoBacking || !r.stable)return "right-not-stable-unbacked-object";
    if(!r.object || l.object==r.object)return "aliased-object";
    if(!lm.readable || !rm.readable || !lm.stable || !rm.stable)return "texture-metadata-unreadable-or-changing";
    if(lm.vtable!=wh+kTextureVtableRva || rm.vtable!=wh+kTextureVtableRva)return "texture-type";
    if(!NamesMatchPair(lm.name,rm.name))return "texture-name-or-pipeline-suffix";
    if(lm.type!=1 || rm.type!=1 || lm.depth!=1 || rm.depth!=1 || rm.mipCount!=1)return "texture-shape";
    if(lm.sourceFormat!=kVelocityFormat || rm.sourceFormat!=0)return "format-contract";
    if(rm.customId!=-1)return "right-custom-id";
    // Preserve the existing right object's own flags; do NOT copy the left's
    // extra 0x20000020 bits. Accept only the exact initial flag family.
    if(rm.flags!=kRightInitialFlags && rm.flags!=(kRightInitialFlags|1u))return "right-flags-contract";
    if(!e.view[0].bounded || !e.view[0].stable || !e.view[0].entryRead || !e.view[0].rtv)return "left-rtv-control";
    return nullptr;
}
inline bool SamePrerequisites(const pf::Snapshot& a,const Evidence& ae,const pf::Snapshot& b,const Evidence& be) noexcept {
    if(a.pipeline!=b.pipeline || a.crv!=b.crv || a.frame!=b.frame || a.output!=b.output)return false;
    for(unsigned i=0;i<2;++i) {
        const auto& x=a.velocity[i];const auto& y=b.velocity[i];
        if(x.object!=y.object || x.backing!=y.backing || x.views!=y.views || x.width!=y.width || x.height!=y.height)return false;
        const auto& m=ae.texture[i];const auto& n=be.texture[i];
        if(m.namePointer!=n.namePointer || m.flags!=n.flags || m.type!=n.type || m.sourceFormat!=n.sourceFormat ||
           m.customId!=n.customId || std::memcmp(m.name,n.name,kNameCapacity)!=0)return false;
    }
    return ae.rt.renderer==be.rt.renderer && ae.rt.threadObject==be.rt.threadObject &&
        ae.view[0].begin==be.view[0].begin && ae.view[0].end==be.view[0].end &&
        ae.view[0].capacity==be.view[0].capacity && ae.view[0].rtv==be.view[0].rtv &&
        ae.view[0].rtvKey==be.view[0].rtvKey;
}
inline bool PreparedBasic(const pf::Snapshot& before,const Evidence& initial,const pf::Snapshot& after,const Evidence& current) noexcept {
    const auto& l=after.velocity[0];const auto& r=after.velocity[1];
    return after.qualified && after.stateRead && after.ownerMatches && after.outputRead &&
        after.selector==0 && after.builtCount==1 && after.phase==1 && !after.scheduleEnabled &&
        !after.dispatchMode && !after.deferSetting && current.ownerRead && current.resourcePipeline==after.pipeline &&
        current.rt.matches && current.rt.stable && current.rt.renderer==initial.rt.renderer &&
        current.rt.threadObject==initial.rt.threadObject &&
        after.pipeline==before.pipeline && after.crv==before.crv && after.frame==before.frame && after.output==before.output &&
        l.stable && r.stable && l.status==pf::ResourceStatus::BasicFieldsPresent &&
        current.texture[0].stable && current.texture[0].flags==initial.texture[0].flags &&
        current.texture[0].sourceFormat==initial.texture[0].sourceFormat &&
        EqualNames(current.texture[0].name,initial.texture[0].name) &&
        current.view[0].entryRead && current.view[0].stable && current.view[0].rtv==initial.view[0].rtv &&
        l.object==before.velocity[0].object && l.backing==before.velocity[0].backing &&
        l.width==before.velocity[0].width && l.height==before.velocity[0].height &&
        r.object==before.velocity[1].object && r.status==pf::ResourceStatus::BasicFieldsPresent &&
        r.backing!=l.backing && r.width==l.width && r.height==l.height &&
        current.texture[1].stable && current.texture[1].sourceFormat==kVelocityFormat &&
        EqualNames(current.texture[1].name,initial.texture[1].name) &&
        current.view[1].entryRead && current.view[1].stable && current.view[1].rtv &&
        current.view[1].rtv!=current.view[0].rtv;
}

// D85R5R4: initial creation and resizing an already backed RIGHT are different
// native paths. The latter bypasses the name registry, preserves the CTexture
// object and lets the engine retire the previous GPU resource. Never admit an
// unbacked object through this contract or reuse the initial-allocation budget.
inline bool NeedsBackedResize(const pf::Snapshot& s) noexcept {
    const auto& l=s.velocity[0];const auto& r=s.velocity[1];
    return l.status==pf::ResourceStatus::BasicFieldsPresent &&
        r.status==pf::ResourceStatus::BasicFieldsPresent &&
        (l.width!=r.width || l.height!=r.height);
}
inline const char* RejectBackedPair(const pf::Snapshot& s,const Evidence& e,std::uintptr_t wh) noexcept {
    if(!s.qualified || !s.stateRead || !s.ownerMatches || !s.outputRead || !s.output)return "main-view-state";
    if(s.selector!=0 || s.builtCount!=1)return "not-ordinary-count1-eye0";
    if(s.phase!=1 || s.scheduleEnabled || s.dispatchMode || s.deferSetting)return "scheduling-state";
    if(!e.ownerRead || e.resourcePipeline!=s.pipeline)return "resource-owner";
    if(!e.rt.readable || !e.rt.matches || !e.rt.stable || e.rt.loadingMode)return "not-verified-native-render-thread";
    for(unsigned eye=0;eye<2;++eye) {
        const auto& t=s.velocity[eye];const auto& m=e.texture[eye];const auto& v=e.view[eye];
        if(t.status!=pf::ResourceStatus::BasicFieldsPresent || !t.stable || !t.object || !t.backing)
            return "not-stable-backed-pair";
        if(t.width<64 || t.height<64 || t.width>16384 || t.height>16384)return "unsupported-dimensions";
        if(!m.readable || !m.stable || m.vtable!=wh+kTextureVtableRva)return "texture-metadata-or-type";
        if(m.type!=1 || m.depth!=1 || m.mipCount!=1 ||
           m.sourceFormat!=kVelocityFormat || m.deviceFormat!=kVelocityFormat)return "velocity-shape-or-format";
        if(!v.typeRead || !v.typeStable || v.backingVtable!=wh+kDeviceTextureVtableRva ||
           !v.headerRead || !v.bounded || !v.stable || !v.entryRead || !v.rtv)return "backing-type-or-rtv";
    }
    const auto& l=s.velocity[0];const auto& r=s.velocity[1];
    if(l.object==r.object || l.backing==r.backing || e.view[0].rtv==e.view[1].rtv)return "eye-resource-alias";
    if(!NamesMatchPair(e.texture[0].name,e.texture[1].name))return "texture-name-or-pipeline-suffix";
    const auto& right=e.texture[1];
    // This is the previously provisioned velocity target, not a pool/streaming
    // texture. Preserve its flag family; the LEFT adds unrelated flag bits.
    if(right.flags!=(kRightInitialFlags|1u) || right.customId!=-1)return "right-flags-or-custom-id";
    for(auto bits:right.clearBits)if(bits!=0)return "right-clear-contract";
    return nullptr;
}
inline const char* RejectBackedResize(const pf::Snapshot& s,const Evidence& e,std::uintptr_t wh) noexcept {
    if(const char* why=RejectBackedPair(s,e,wh))return why;
    return NeedsBackedResize(s)?nullptr:"dimensions-already-match";
}
inline bool SameTextureCheckpoint(const pf::Texture& a,const TextureMeta& am,const Views& av,
                                  const pf::Texture& b,const TextureMeta& bm,const Views& bv) noexcept {
    return a.object==b.object && a.backing==b.backing && a.views==b.views &&
        a.width==b.width && a.height==b.height && am.vtable==bm.vtable &&
        am.namePointer==bm.namePointer && am.flags==bm.flags && am.customId==bm.customId &&
        am.depth==bm.depth && am.mipCount==bm.mipCount && am.type==bm.type &&
        am.sourceFormat==bm.sourceFormat && am.deviceFormat==bm.deviceFormat &&
        std::memcmp(am.clearBits,bm.clearBits,sizeof(am.clearBits))==0 &&
        std::memcmp(am.name,bm.name,kNameCapacity)==0 && av.backingVtable==bv.backingVtable &&
        av.begin==bv.begin && av.end==bv.end && av.capacity==bv.capacity &&
        av.rtv==bv.rtv && av.rtvKey==bv.rtvKey;
}
inline bool SameResizePrerequisites(const pf::Snapshot& a,const Evidence& ae,
                                    const pf::Snapshot& b,const Evidence& be) noexcept {
    if(!SamePrerequisites(a,ae,b,be))return false;
    for(unsigned eye=0;eye<2;++eye)
        if(!SameTextureCheckpoint(a.velocity[eye],ae.texture[eye],ae.view[eye],
                                  b.velocity[eye],be.texture[eye],be.view[eye]))return false;
    return true;
}
inline bool PreparedBackedResize(const pf::Snapshot& before,const Evidence& initial,
                                 const pf::Snapshot& after,const Evidence& current,std::uintptr_t wh) noexcept {
    if(RejectBackedPair(after,current,wh) || !PreparedBasic(before,initial,after,current))return false;
    if(!SameTextureCheckpoint(before.velocity[0],initial.texture[0],initial.view[0],
                              after.velocity[0],current.texture[0],current.view[0]))return false;
    const auto& a=initial.texture[1];const auto& b=current.texture[1];
    // The allocator may reuse the old wrapper/descriptor address. Require the
    // native object/ownership contract and actual post-call dimensions instead
    // of treating a changed pointer as proof of successful GPU allocation.
    return before.velocity[1].object==after.velocity[1].object && a.namePointer==b.namePointer &&
        a.vtable==b.vtable && a.flags==b.flags && a.customId==b.customId &&
        a.type==b.type && a.depth==b.depth && a.mipCount==b.mipCount &&
        a.sourceFormat==b.sourceFormat && a.deviceFormat==b.deviceFormat &&
        std::memcmp(a.clearBits,b.clearBits,sizeof(a.clearBits))==0 &&
        std::memcmp(a.name,b.name,kNameCapacity)==0;
}
} // namespace kcd2vr::resource_model
