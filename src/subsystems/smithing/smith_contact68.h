#pragma once
#include "../../vrik/smith_tool36.h"
#include <atomic>
#include <cmath>
#include <cstdint>

namespace kcd2vr::subsystems::smithing::contact68 {
// READ-ONLY. A native StrokeHit has two DIFFERENT position inputs: C51060's
// projected world point and model+198's normalized Vec2. Neither is permission
// to make a hit, change heat/quality, or turn a hovering hammer into contact.
struct Vec2 {float x=0,y=0;};
struct Vec3 {float x=0,y=0,z=0;};
struct HammerFrame {
    smith_tool36::Binding binding{};
    std::uint64_t tick=0,sample=0,poseEpoch=0,sourceEpoch=0,modeEpoch=0;
    float world[8]{}; // qxyzw, WORLD xyz, scale; ACTUAL successful placement
    bool placed=false;
};
// Root publishes/reads an existing completed/placed packet, never polls native
// state here. Its adapter must reject a changed current pose/mode epoch.
inline bool (*readHammer)(HammerFrame&,std::uint64_t) noexcept=nullptr;
// Exact existing retail entity lookup, used only for one recorded native hit.
// Its adapter must qualify current CEntity class + native entity ID.
inline std::uintptr_t (*findEntity)(std::uint32_t) noexcept=nullptr;
// Copied diagnostic metadata only: the producer may stop its extra placement
// readback once this exact finite capture is full. A new capture has a distinct
// session and is not blocked by the previous session's exhausted marker.
inline std::atomic<std::uint64_t> exhaustedSession{0};
inline bool install(std::uintptr_t) noexcept;
inline bool uninstall() noexcept;

struct Event {
    std::uintptr_t state=0,instance=0,player=0,henry=0,model=0,holder=0,definition=0,workpiece=0;
    std::uint64_t lifetime=0;std::uint32_t workpieceId=0;
    int phase=-1;Vec2 cursor{},mapSize{},mapOffset{};Vec3 mapRotation{},point{};
    float workpieceWorld[12]{};
    HammerFrame hammer{};bool hammerJoined=false;
};
inline bool finite(Vec2 v) noexcept {return std::isfinite(v.x)&&std::isfinite(v.y);}
inline bool finite(Vec3 v) noexcept {return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z);}
template<unsigned N> inline bool finite(const float (&v)[N]) noexcept {
    for(const auto x:v)if(!std::isfinite(x)||std::fabs(x)>1000000.f)return false;return true;
}
struct Budget {
    std::uint64_t session=0,serial=0;unsigned used=0;
    bool admit(bool f5One,bool recording,std::uint64_t capture,std::uintptr_t caller,std::uintptr_t base) noexcept {
        if(!f5One||!recording||!capture||!base||caller!=base+0x859A76)return false;
        if(session!=capture){session=capture;used=0;serial=0;}
        if(used>=128)return false;++used;++serial;return true;
    }
};
template<class Read,class Find> bool context(Read&& rd,Find&& find,std::uintptr_t b,
    std::uintptr_t state,std::uintptr_t instance,std::uint64_t lifetime,Vec3 point,Event& out,const char*& reason) noexcept {
    out={};reason="NATIVE_CONTEXT_UNQUALIFIED";
    if(!b||!state||!instance||!lifetime||!finite(point))return false;
    Event e{};e.state=state;e.instance=instance;e.lifetime=lifetime;e.point=point;
    std::uintptr_t v=0,back=0;unsigned char active=0,phase=255;
    if(!rd(instance,v)||v!=b+smith_tool36::kInstanceVft||!rd(instance+0x2C,active)||active!=1||
       !rd(state,back)||back!=instance||!rd(instance+0xB0,back)||back!=state||
       !rd(instance+0x18,e.player)||!e.player||!rd(e.player,v)||v!=b+smith_tool36::kPlayerVft||
       !rd(e.player+0x38,e.henry)||!e.henry||!rd(e.henry,v)||v!=b+0x4072C78||
       !rd(instance+0x98,e.model)||!e.model||!rd(e.model,v)||v!=b+smith_tool36::kModelVft||
       !rd(e.model+0x88,phase)||phase>20||!rd(e.model+0x198,v)||v!=b+0x3A6A1E0||
       !rd(e.model+0x1A0,e.cursor)||!finite(e.cursor)||
       !rd(e.model+0x2F8,e.holder)||!e.holder||!rd(e.holder,v)||v!=b+smith_tool36::kWorkpieceVft||
       !rd(e.holder+0x10,e.workpieceId)||!e.workpieceId||!rd(e.holder+0x18,e.definition)||!e.definition)
        return false;
    e.phase=phase;e.workpiece=find(e.workpieceId);
    std::uint32_t id=0;
    if(!e.workpiece||!rd(e.workpiece,v)||v!=b+0x4072C78||!rd(e.workpiece+0xC,id)||id!=e.workpieceId||
       !rd(e.workpiece+0x58,e.workpieceWorld)||!finite(e.workpieceWorld)||
       !rd(e.definition+0x40,e.mapSize)||!finite(e.mapSize)||
       !rd(e.definition+0x48,e.mapOffset)||!finite(e.mapOffset)||
       !rd(e.definition+0x50,e.mapRotation)||!finite(e.mapRotation)){
        reason="WORKPIECE_FRAME_UNQUALIFIED";return false;
    }
    unsigned char again=255;
    if(!rd(state,back)||back!=instance||!rd(instance+0xB0,back)||back!=state||
       !rd(instance+0x18,back)||back!=e.player||!rd(instance+0x98,back)||back!=e.model||
       !rd(e.model+0x88,again)||again!=phase||!rd(e.model+0x2F8,back)||back!=e.holder||
       !rd(e.holder+0x18,back)||back!=e.definition||!rd(e.holder+0x10,id)||id!=e.workpieceId||
       !rd(instance+0x2C,active)||active!=1){reason="NATIVE_CONTEXT_CHANGED";return false;}
    out=e;reason="NATIVE_EVENT_OBSERVED";return true;
}
template<class Read> bool joinHammer(Read&& rd,std::uintptr_t b,std::uint64_t now,const Event& e,
                                   const HammerFrame& h) noexcept {
    if(!h.placed||!h.tick||now<h.tick||now-h.tick>80||!h.sample||!h.poseEpoch||!h.sourceEpoch||!h.modeEpoch||
       !finite(h.world)||h.world[7]<=0||h.world[7]>8||!h.binding||h.binding.tool!=int(smith_tool36::Tool::Hammer)||
       h.binding.side!=0||h.binding.instance!=e.instance||h.binding.player!=e.player||h.binding.lifetime!=e.lifetime)return false;
    double norm=0;for(int i=0;i<4;++i)norm+=double(h.world[i])*h.world[i];if(norm<.25||norm>4)return false;
    // Phase is independently qualified NOW. Previously placed geometry may
    // cross14->16; that does not borrow an old native phase capability.
    smith_tool36::Binding current{};
    if(!smith_tool36::read(rd,b,e.instance,e.player,h.binding.character,h.binding.attachment,h.binding.entity,e.phase,current))return false;
    current.lifetime=e.lifetime;return smith_tool36::sameGripIdentity67(current,h.binding);
}
// Pure affine inversion for diagnostics only; this is NOT the game's UV map
// inverse (MapRotation/offset/flip still need a proven native mapping join).
inline bool localPoint(const float (&m)[12],Vec3 p,Vec3& out) noexcept {
    out={};if(!finite(m)||!finite(p))return false;
    const double a=m[0],bb=m[1],c=m[2],d=m[4],e=m[5],f=m[6],g=m[8],h=m[9],i=m[10];
    const double det=a*(e*i-f*h)-bb*(d*i-f*g)+c*(d*h-e*g);
    if(!std::isfinite(det)||std::fabs(det)<1.e-9)return false;
    const double x=p.x-m[3],y=p.y-m[7],z=p.z-m[11];
    out={float(((e*i-f*h)*x+(c*h-bb*i)*y+(bb*f-c*e)*z)/det),
         float(((f*g-d*i)*x+(a*i-c*g)*y+(c*d-a*f)*z)/det),
         float(((d*h-e*g)*x+(bb*g-a*h)*y+(a*e-bb*d)*z)/det)};
    return finite(out);
}
}
