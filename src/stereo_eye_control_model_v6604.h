#pragma once
#include "stereo_resource_model_v6603.h"

// Portable, exact-retail predicates. Zero separation deliberately isolates eye
// selection/lifecycle from IPD and frustum changes. No SDK object construction.
namespace kcd2vr::eye_control_model {
namespace pf=preflight_model;
namespace rm=resource_model;
constexpr std::uintptr_t kPipelineRva=0x864728;
constexpr std::uintptr_t kBuilderRva=0x865550;
constexpr std::uintptr_t kCurrentSetterRva=0x43E408;
constexpr std::uintptr_t kPreviousSetterRva=0x43E3AC;
constexpr std::uintptr_t kCameraAssignRva=0x426E80;
constexpr std::uintptr_t kGBufferRva=0x7C2110;
constexpr std::uintptr_t kOpaqueRva=0x7C24B8;
constexpr std::uintptr_t kRangesRva=0x7729CC;
constexpr std::uintptr_t kCommandReturnRva=0x1DF7928;
constexpr std::uintptr_t kGBufferVtableRva=0x3DFCD80;
constexpr std::size_t kOwnedMember=0x2C8;
constexpr unsigned kMinimumCompletedSamples=17;
struct alignas(16) CameraBytes {unsigned char bytes[pf::kCameraSize]{};};
struct CameraSet {
    CameraBytes source[2]{};       // original physical eye0 current/previous
    CameraBytes savedRight[2]{};   // original physical eye1 current/previous
    CameraBytes prepared[2]{};     // prepared physical eye1
    CameraBytes preparedLeft[2]{}; // V66.12 prepared physical eye0 for real-IPD design
};
struct Work {
    unsigned builds{},buildReturns{},standard{},standardReturns{},gbuffer{},gbufferReturns{},opaque{},opaqueReturns{};
    unsigned ranges{},nonempty{};
    std::uint64_t requestedItems{};
    unsigned returnByte{};
    bool cameraSourceValid{},builtPointerValid{},standardStateValid{};
};
inline bool Finite(float x) noexcept {
    std::uint32_t bits=0;std::memcpy(&bits,&x,sizeof(bits));return (bits&0x7F800000u)!=0x7F800000u;
}
template<class T>T Field(const CameraBytes& c,std::size_t off) noexcept {
    T x{};if(off<=sizeof(c.bytes)-sizeof(T))std::memcpy(&x,c.bytes+off,sizeof(x));return x;
}
template<class T>void Field(CameraBytes& c,std::size_t off,T x) noexcept {
    if(off<=sizeof(c.bytes)-sizeof(T))std::memcpy(c.bytes+off,&x,sizeof(x));
}
inline bool Padding(std::size_t i) noexcept {
    // Bytes skipped by the inspected native assignment (not semantic camera data).
    return (i>=0x49 && i<0x4C)||(i>=0x221 && i<0x224)||(i>=0x2F9 && i<0x2FC);
}
inline bool EqualAssigned(const CameraBytes& a,const CameraBytes& b) noexcept {
    for(std::size_t i=0;i<sizeof(a.bytes);++i)if(!Padding(i)&&a.bytes[i]!=b.bytes[i])return false;
    return true;
}
inline bool NativeDerivedBuildCache(std::size_t i) noexcept {
    // Exact-retail CCamera derived cache recomputed by WHGame+0x5E9890 after
    // native camera selection/build. Disassembly of the retained 1.5.6 DLL shows
    // writes spanning 0x224..0x2C7 while the transform/input fields are read.
    return i>=0x224 && i<0x2C8;
}
inline bool EqualStereoInputsAfterBuild(const CameraBytes& live,const CameraBytes& prepared) noexcept {
    // Post-builder validation must not require native-derived cache bytes to stay
    // equal to their pre-build values. Everything outside that exact cache range
    // remains strict, including transform, FOV/input fields, eye tag and owned member.
    for(std::size_t i=0;i<sizeof(live.bytes);++i) {
        if(Padding(i)||NativeDerivedBuildCache(i))continue;
        if(live.bytes[i]!=prepared.bytes[i])return false;
    }
    return true;
}
inline bool SameGeometry(const CameraBytes& a,const CameraBytes& b) noexcept {
    for(std::size_t i=0;i<sizeof(a.bytes);++i) {
        if(Padding(i)||(i>=pf::kCameraEyeTag && i<pf::kCameraEyeTag+4)||
           (i>=kOwnedMember && i<kOwnedMember+sizeof(std::uintptr_t)))continue;
        if(a.bytes[i]!=b.bytes[i])return false;
    }
    return true;
}
inline bool SaneSource(const CameraBytes& c) noexcept {
    if(Field<std::uint32_t>(c,pf::kCameraEyeTag)!=0)return false;
    float f[12]{};std::memcpy(f,c.bytes,sizeof(f));
    for(float x:f)if(!Finite(x))return false;
    // Reject malformed/sentinel transforms; do not normalize or move the camera.
    for(unsigned col=0;col<3;++col) {
        const float n=f[col]*f[col]+f[col+4]*f[col+4]+f[col+8]*f[col+8];
        if(!Finite(n)||n<0.9f||n>1.1f)return false;
    }
    for(unsigned a=0;a<3;++a)for(unsigned b=a+1;b<3;++b) {
        const float dot=f[a]*f[b]+f[a+4]*f[b+4]+f[a+8]*f[b+8];
        if(!Finite(dot)||dot<-.05f||dot>.05f)return false;
    }
    for(unsigned i=3;i<12;i+=4)if(f[i]<-10000000.f||f[i]>10000000.f)return false;
    return true;
}

inline bool ShiftCamera(CameraBytes& c,float meters) noexcept {
    if(!Finite(meters))return false;
    float f[12]{};std::memcpy(f,c.bytes,sizeof(f));
    if(!Finite(f[0])||!Finite(f[4])||!Finite(f[8])||!Finite(f[3])||!Finite(f[7])||!Finite(f[11]))return false;
    f[3]+=f[0]*meters; f[7]+=f[4]*meters; f[11]+=f[8]*meters;
    if(!Finite(f[3])||!Finite(f[7])||!Finite(f[11]))return false;
    std::memcpy(c.bytes,f,sizeof(f));return true;
}
inline bool MakeStereoDesign(CameraSet& set,float ipd) noexcept {
    // V66.48: exactly 0 is allowed for the zero-IPD diagnostic (both passes use the center camera).
    if(!Finite(ipd)||ipd>0.10f||(ipd<0.03f&&ipd!=0.0f))return false;
    for(unsigned bank=0;bank<2;++bank) {
        if(!SaneSource(set.source[bank]))return false;
        set.preparedLeft[bank]=set.source[bank];
        set.prepared[bank]=set.source[bank];
        if(!ShiftCamera(set.preparedLeft[bank],-0.5f*ipd)||!ShiftCamera(set.prepared[bank],+0.5f*ipd))return false;
        Field(set.preparedLeft[bank],pf::kCameraEyeTag,std::uint32_t{0});
        Field(set.prepared[bank],pf::kCameraEyeTag,std::uint32_t{1});
        // Preserve each destination slot's native-owned member identity.
        Field(set.preparedLeft[bank],kOwnedMember,Field<std::uintptr_t>(set.source[bank],kOwnedMember));
        Field(set.prepared[bank],kOwnedMember,Field<std::uintptr_t>(set.savedRight[bank],kOwnedMember));
        for(std::size_t i=0;i<pf::kCameraSize;++i)if(Padding(i)) {
            set.preparedLeft[bank].bytes[i]=set.source[bank].bytes[i];
            set.prepared[bank].bytes[i]=set.savedRight[bank].bytes[i];
        }
        if(Field<std::uint32_t>(set.preparedLeft[bank],pf::kCameraEyeTag)!=0||
           Field<std::uint32_t>(set.prepared[bank],pf::kCameraEyeTag)!=1)return false;
    }
    return true;
}

inline bool MakeControl(CameraSet& set) noexcept {
    for(unsigned bank=0;bank<2;++bank) {
        if(!SaneSource(set.source[bank]))return false;
        set.prepared[bank]=set.source[bank];
        Field(set.prepared[bank],pf::kCameraEyeTag,std::uint32_t{1});
        // The native assignment owns this member. Keeping the destination's
        // existing pointer forces its identity/no-refcount-change branch. No raw
        // borrowed source pointer is installed in place of an owned member.
        Field(set.prepared[bank],kOwnedMember,Field<std::uintptr_t>(set.savedRight[bank],kOwnedMember));
        for(std::size_t i=0;i<pf::kCameraSize;++i)if(Padding(i))set.prepared[bank].bytes[i]=set.savedRight[bank].bytes[i];
        if(!SameGeometry(set.prepared[bank],set.source[bank]))return false;
    }
    return true;
}
template<class Reader>
bool ReadCamera(Reader& read,std::uintptr_t crv,unsigned bank,unsigned eye,CameraBytes& out) noexcept {
    if(bank>1||eye>1||!crv)return false;
    const auto off=pf::kCameraBanks[bank]+eye*pf::kCameraSize;
    if(crv>~std::uintptr_t{0}-off-pf::kCameraSize)return false;
    CameraBytes again{};
    return read(crv+off,&out,sizeof(out)) && read(crv+off,&again,sizeof(again)) &&
           std::memcmp(out.bytes,again.bytes,sizeof(out))==0;
}
template<class Reader>
bool CaptureRawCameras(Reader& read,std::uintptr_t crv,CameraSet& out) noexcept {
    for(unsigned bank=0;bank<2;++bank)
        if(!ReadCamera(read,crv,bank,0,out.source[bank])||!ReadCamera(read,crv,bank,1,out.savedRight[bank]))return false;
    return true;
}
template<class Reader>
bool CaptureCameras(Reader& read,std::uintptr_t crv,CameraSet& out) noexcept {
    if(!CaptureRawCameras(read,crv,out))return false;
    return MakeControl(out);
}
template<class Reader>
bool CheckBuilt(Reader& read,std::uintptr_t crv,unsigned eye,std::uint64_t& hash) noexcept {
    std::uint64_t count=0;std::uintptr_t camera=0,planes=0;unsigned char bytes[0x400]{};
    if(eye>1||!pf::Read(read,crv,0xA330,count)||count!=1||
       !pf::Read(read,crv,0x9B30,camera)||!pf::Read(read,crv,0x9B38,planes))return false;
    const auto expected=crv+pf::kCameraBanks[0]+eye*pf::kCameraSize;
    if(camera!=expected||planes!=expected+0x114||!read(crv+0x9B30,bytes,sizeof(bytes)))return false;
    hash=pf::Hash(bytes,sizeof(bytes));return true;
}
inline bool NormalComplete(const Work& w) noexcept {
    return w.builds==1&&w.buildReturns==1&&w.standard==1&&w.standardReturns==1&&
           w.gbuffer==1&&w.gbufferReturns==1&&w.opaque==1&&w.opaqueReturns==1&&
           w.nonempty>0&&w.cameraSourceValid&&w.builtPointerValid&&w.standardStateValid;
}
inline const char* RejectPersistentResources(const pf::Snapshot& s,const rm::Evidence& e,std::uintptr_t wh,unsigned phase) noexcept {
    // Post-normal-render validation: the native output attachment is transient and
    // is expected to be cleared by normal pipeline completion. Validate the
    // persistent view identity/resources without requiring a live output pointer.
    if(!s.qualified||!s.stateRead||!s.ownerMatches||!s.outputRead)return "unqualified-main-view";
    if(s.selector!=0||s.builtCount!=1||s.phase!=phase||s.scheduleEnabled||s.deferSetting||s.dispatchMode)return "native-phase-or-mode";
    if(!e.ownerRead||e.resourcePipeline!=s.pipeline||!e.rt.readable||!e.rt.matches||!e.rt.stable||e.rt.loadingMode)return "native-owner-or-thread";
    for(unsigned eye=0;eye<2;++eye) {
        const auto& t=s.velocity[eye];const auto& m=e.texture[eye];const auto& v=e.view[eye];
        if(t.status!=pf::ResourceStatus::BasicFieldsPresent||!t.stable)return "missing-or-changing-velocity-backing";
        if(!m.readable||!m.stable||m.vtable!=wh+rm::kTextureVtableRva||m.type!=1||m.depth!=1||m.mipCount!=1||
           m.sourceFormat!=rm::kVelocityFormat||m.deviceFormat!=rm::kVelocityFormat)return "velocity-shape-or-format";
        if(!v.headerRead||!v.bounded||!v.entryRead||!v.stable||!v.rtv)return "velocity-rtv-not-observed";
    }
    const auto& l=s.velocity[0];const auto& r=s.velocity[1];
    if(l.object==r.object||l.backing==r.backing||e.view[0].rtv==e.view[1].rtv)return "eye-resource-alias";
    if(l.width!=r.width||l.height!=r.height||l.width<64||l.height<64||l.width>16384||l.height>16384)return "velocity-size";
    if(!rm::NamesMatchPair(e.texture[0].name,e.texture[1].name))return "velocity-name-pair";
    return nullptr; // necessary basic predicates, NOT complete GPU/stereo readiness
}
inline const char* RejectResources(const pf::Snapshot& s,const rm::Evidence& e,std::uintptr_t wh,unsigned phase) noexcept {
    // Pre-eye1 camera mutation: by this point the second native pipeline
    // invocation must have recreated a live output attachment.
    if(!s.output)return "native-output-not-reestablished";
    return RejectPersistentResources(s,e,wh,phase);
}
inline bool SamePersistentResourceCheckpoint(const pf::Snapshot& a,const rm::Evidence& ae,
                                             const pf::Snapshot& b,const rm::Evidence& be) noexcept {
    if(a.pipeline!=b.pipeline || a.crv!=b.crv || a.frame!=b.frame)return false;
    if(a.selector!=0 || b.selector!=0 || a.builtCount!=1 || b.builtCount!=1)return false;
    if(!a.ownerMatches || !b.ownerMatches || !ae.ownerRead || !be.ownerRead ||
       ae.resourcePipeline!=a.pipeline || be.resourcePipeline!=b.pipeline)return false;
    if(a.velocity[0].object!=b.velocity[0].object || a.velocity[1].object!=b.velocity[1].object)return false;
    for(unsigned i=0;i<2;++i) {
        const auto& x=a.velocity[i];const auto& y=b.velocity[i];
        if(x.backing!=y.backing || x.views!=y.views || x.width!=y.width || x.height!=y.height)return false;
        const auto& m=ae.texture[i];const auto& n=be.texture[i];
        if(m.namePointer!=n.namePointer || m.flags!=n.flags || m.type!=n.type || m.sourceFormat!=n.sourceFormat ||
           m.deviceFormat!=n.deviceFormat || m.customId!=n.customId || std::memcmp(m.name,n.name,rm::kNameCapacity)!=0)return false;
        const auto& vx=ae.view[i];const auto& vy=be.view[i];
        if(vx.begin!=vy.begin || vx.end!=vy.end || vx.capacity!=vy.capacity || vx.rtv!=vy.rtv || vx.rtvKey!=vy.rtvKey)return false;
    }
    return ae.rt.renderer==be.rt.renderer && ae.rt.threadObject==be.rt.threadObject;
}
} // namespace kcd2vr::eye_control_model
