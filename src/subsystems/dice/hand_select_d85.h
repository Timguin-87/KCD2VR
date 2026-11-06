#pragma once
// D85 dice: pick the die to keep by pointing with the right hand.
// INI VrDiceHandsD85 + Experiments > Objects page 2 "Dice: point to choose".
// OFF by default (experiment). ON = while it is your dice choosing turn the native
// dice cursor jumps to the die your right hand points at; A (native dice_hold)
// keeps/unkeeps it, X/Y/B stay the game's own buttons. Results stay the game's RNG.
//
// Exact retail evidence (WHGame 0x6A350E20), every native field below is pinned:
//  - C_Dice vft+178 = 2E8E45C `cmp [rcx+498h],5; sete al` = native dice input allowed.
//  - game model C_Dice+4A0 (vft 473F178), its state at +108, human player at +20.
//    every dice state vft+20 = 9DD2D0 `mov rax,[rcx+18h]` (current player);
//    PlayerTurn 3F711E0 / Holding 473F1C8 vft+40 = 2E849C8 `cmp [rcx+18h],rdx` and
//    2E8FC4C passes model+20 to it: only these two states accept the human's input.
//  - 2E8A5E2: die list = C_Dice + (player+58h + 4Bh)*16, vector at list+F0/F8, 16 byte
//    entries (2E8B8EA). die+48 = C_DieModel (vft 473F160, ctor 2E7F76C: +28 index,
//    +40 state), 2E8FABA hold handler reads model+40 / model+28.
//  - 202AEF0 (C_Dice die hover lambda, vft 3F6EDC8, built at 2E90143): a die whose
//    state==2 is refused, otherwise 2E951BC(C_Dice,model+28,false). The d-pad/stick
//    handler 2E9007C makes the same call. That is the only cursor write used here.
//  - die+30 = entity id (2E8B9D6); entity world pos = CEntity+64/+74/+84 (vft+170 = 648030).
// Nothing here throws dice, keeps a die, or changes a result.
#include "../../core/snapshot_mailbox.h"
#include <atomic>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace kcd2vr::dice_hands_d85 {
inline std::atomic<bool> enabled{false};
enum class Status:unsigned {Off,WaitTurn,Point,NoRay,Unsupported,Refused};
inline std::atomic<unsigned> status{0};
inline const wchar_t* statusText(unsigned s) noexcept {
    switch(Status(s)){
    case Status::WaitTurn:return L"ON - waits for your dice choice";
    case Status::Point:return L"ON - point at a die, A keeps it";
    case Status::NoRay:return L"ON - no right hand ray, gamepad";
    case Status::Unsupported:return L"ON - game version unknown, gamepad";
    case Status::Refused:return L"ON - game refused cursor, gamepad";
    default:return L"ON";
    }
}
// Right hand world ray (published by the mod worker, read on the game frame end).
struct Ray {float o[3]{},d[3]{0,1,0};std::uint64_t tick=0;bool valid=false;};
inline core::SnapshotMailbox<Ray> ray;
inline bool fresh(const Ray& r,std::uint64_t now) noexcept {return r.valid&&r.tick&&now>=r.tick&&now-r.tick<=80;}

// Same transform as the existing alchemy pointer fallback in mod.cpp (palm rays):
// controller relative to the HMD centre, rotated into the rendered camera basis.
// cam = CameraRotationSample::m (right m0/m3/m6, forward m1/m4/m7, up m2/m5/m8).
inline void qrot(const float q[4],const float v[3],float out[3]) noexcept {
    const float tx=2.f*(q[1]*v[2]-q[2]*v[1]),ty=2.f*(q[2]*v[0]-q[0]*v[2]),tz=2.f*(q[0]*v[1]-q[1]*v[0]);
    out[0]=v[0]+q[3]*tx+(q[1]*tz-q[2]*ty);out[1]=v[1]+q[3]*ty+(q[2]*tx-q[0]*tz);out[2]=v[2]+q[3]*tz+(q[0]*ty-q[1]*tx);
}
inline bool buildRay(const float eyeL[3],const float eyeR[3],const float headQ[4],const float ctrlPos[3],const float ctrlQ[4],
                     const float cam[9],const float camPos[3],std::uint64_t tick,Ray& out) noexcept {
    out={};
    const float head[3]{.5f*(eyeL[0]+eyeR[0]),.5f*(eyeL[1]+eyeR[1]),.5f*(eyeL[2]+eyeR[2])};
    const float inv[4]{-headQ[0],-headQ[1],-headQ[2],headQ[3]};
    const float rel[3]{ctrlPos[0]-head[0],ctrlPos[1]-head[1],ctrlPos[2]-head[2]};
    float relHead[3]{},aim[3]{},aimHead[3]{};const float fwd[3]{0,0,-1};
    qrot(inv,rel,relHead);qrot(ctrlQ,fwd,aim);qrot(inv,aim,aimHead);
    const float rx=cam[0],ry=cam[3],rz=cam[6],fx=cam[1],fy=cam[4],fz=cam[7],ux=cam[2],uy=cam[5],uz=cam[8];
    out.o[0]=camPos[0]+rx*relHead[0]+ux*relHead[1]+fx*(-relHead[2]);
    out.o[1]=camPos[1]+ry*relHead[0]+uy*relHead[1]+fy*(-relHead[2]);
    out.o[2]=camPos[2]+rz*relHead[0]+uz*relHead[1]+fz*(-relHead[2]);
    float d[3]{rx*aimHead[0]+ux*aimHead[1]+fx*(-aimHead[2]),ry*aimHead[0]+uy*aimHead[1]+fy*(-aimHead[2]),rz*aimHead[0]+uz*aimHead[1]+fz*(-aimHead[2])};
    const float l=std::sqrt(d[0]*d[0]+d[1]*d[1]+d[2]*d[2]);
    if(!std::isfinite(l)||l<1e-4f)return false;
    for(int i=0;i<3;++i){out.d[i]=d[i]/l;if(!std::isfinite(out.o[i]))return false;}
    out.tick=tick;out.valid=tick!=0;return out.valid;
}

// One native choosing turn, read on the game thread.
constexpr unsigned kMaxDice=8;
struct Die {int index=-1;unsigned char state=0;bool pos=false;float p[3]{};};
struct Turn {std::uintptr_t instance=0,player=0;std::uint64_t lifetime=0;int cursor=-1;unsigned count=0;Die dice[kMaxDice]{};};
constexpr float kMaxAngleDeg=3.5f,kKeepAngleDeg=4.5f,kKeepBonusDeg=1.f,kMaxRangeM=1.6f;
inline float angleDeg(const Ray& r,const float p[3],float& range) noexcept {
    const float v[3]{p[0]-r.o[0],p[1]-r.o[1],p[2]-r.o[2]};
    range=std::sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
    if(!std::isfinite(range)||range<.05f)return 180.f;
    float c=(v[0]*r.d[0]+v[1]*r.d[1]+v[2]*r.d[2])/range;c=c>1.f?1.f:(c<-1.f?-1.f:c);
    return std::acos(c)*57.2957795f;
}
// Nearest selectable die inside the pointing cone. The die chosen last stays
// chosen up to 4.5 degrees so neighbouring dice do not flicker (6 cm apart at
// 0.7 m are 4.9 degrees; a die is about 1.3 degrees).
inline int pick(const Ray& r,const Turn& t,int previous) noexcept {
    int best=-1;float bestAngle=kMaxAngleDeg;
    for(unsigned i=0;i<t.count&&i<kMaxDice;++i){
        const auto& d=t.dice[i];if(!d.pos||d.index!=int(i)||d.state==2)continue;
        float range=0;const float a=angleDeg(r,d.p,range);
        if(range>kMaxRangeM)continue;
        const float limit=int(i)==previous?kKeepAngleDeg:kMaxAngleDeg;
        const float score=int(i)==previous?a-kKeepBonusDeg:a;
        if(a<=limit&&score<bestAngle){best=int(i);bestAngle=score;}
    }
    return best;
}
struct Owner {std::uintptr_t instance=0,player=0;std::uint64_t lifetime=0;int target=-1;};
struct Decision {int set=-1;Status status=Status::Off;};
// Edge only: the cursor is moved when the pointed die CHANGES. The stick/d-pad
// keep working while the hand keeps pointing at the same die.
inline Decision step(Owner& o,bool on,bool turnKnown,const Turn& t,const Ray& r,std::uint64_t now) noexcept {
    Decision d{};
    if(!on){o={};d.status=Status::Off;return d;}
    if(!turnKnown){o.target=-1;d.status=Status::WaitTurn;return d;}
    if(t.instance!=o.instance||t.lifetime!=o.lifetime||t.player!=o.player){o={};o.instance=t.instance;o.lifetime=t.lifetime;o.player=t.player;}
    if(!fresh(r,now)){o.target=-1;d.status=Status::NoRay;return d;}
    d.status=Status::Point;
    const int target=pick(r,t,o.target);
    if(target<0){o.target=-1;return d;}
    if(target==o.target)return d;
    o.target=target;
    if(t.cursor!=target)d.set=target;
    return d;
}

// Pins and the native reader, templated on an explicit memory leaf so the host
// test runs this exact code against a fixture address space.
//   M::read(addr,T&) bounded read; M::same(addr,bytes,n) code compare;
//   M::entity(id) native registry lookup (0 = absent).
struct Pin {std::uintptr_t rva;const unsigned char* bytes;std::size_t size;};
inline constexpr unsigned char kAllowed[]{0x83,0xB9,0x98,0x04,0x00,0x00,0x05,0x0F,0x94,0xC0,0xC3};
inline constexpr unsigned char kStatePlayer[]{0x48,0x8B,0x41,0x18,0xC3};
inline constexpr unsigned char kHumanTurn[]{0x48,0x39,0x51,0x18,0x0F,0x94,0xC0,0xC3};
inline constexpr unsigned char kDieList[]{0x48,0x8B,0x88,0x08,0x01,0x00,0x00,0x48,0x8B,0x01,0xFF,0x50,0x20,0x48,0x63,0x40,0x58,0x48,0x83,0xC0,0x4B,0x48,0x03,0xC0,0x48,0x8B,0x04,0xC3};
inline constexpr unsigned char kDieVector[]{0x4C,0x8B,0x80,0xF8,0x00,0x00,0x00,0x4C,0x2B,0x80,0xF0,0x00,0x00,0x00,0x49,0xC1,0xF8,0x04};
inline constexpr unsigned char kHoldModel[]{0x48,0x8B,0x46,0x48,0x48,0x8B,0xCF,0x8A,0x58,0x40};
inline constexpr unsigned char kHoldIndex[]{0x8B,0x52,0x28};
inline constexpr unsigned char kHover[]{0x48,0x83,0xEC,0x28,0x80,0x7A,0x40,0x02,0x74,0x16,0x8B,0x52,0x28,0x45,0x33,0xC0,0x48,0x8B,0x49,0x08,0xE8,0xB3,0xA2,0xE6,0x00};
inline constexpr unsigned char kDieEntity[]{0x8B,0x51,0x30,0x48,0x8B,0x0D};
inline constexpr unsigned char kWorldPos[]{0x8B,0x41,0x64,0x89,0x02,0x8B,0x41,0x74,0x89,0x42,0x04,0x8B,0x81,0x84,0x00,0x00,0x00};
inline constexpr unsigned char kEntityId[]{0x8B,0x41,0x0C,0xC3};
inline constexpr unsigned char kSetter[]{0x48,0x89,0x5C,0x24,0x08,0x48,0x89,0x74,0x24,0x10,0x48,0x89,0x7C,0x24,0x18,0x55,0x41,0x54,0x41,0x55,0x41,0x56,0x41,0x57};
inline constexpr unsigned char kHumanQuery[]{0x49,0x8B,0x96,0xA0,0x04,0x00,0x00,0x48,0x8B,0x8A,0x08,0x01,0x00,0x00,0x48,0x8B,0x52,0x20,0x48,0x8B,0x01,0xFF,0x50,0x40};
inline constexpr unsigned char kStickSet[]{0x48,0x8B,0x52,0x48,0x45,0x33,0xC0,0x49,0x8B,0xCE,0x8B,0x52,0x28,0xE8,0x3B,0x51,0x00,0x00};
inline constexpr Pin kPins[]{
    {0x2E8E45C,kAllowed,sizeof(kAllowed)},{0x9DD2D0,kStatePlayer,sizeof(kStatePlayer)},{0x2E849C8,kHumanTurn,sizeof(kHumanTurn)},
    {0x2E8A5E2,kDieList,sizeof(kDieList)},{0x2E8B8EA,kDieVector,sizeof(kDieVector)},{0x2E8FABA,kHoldModel,sizeof(kHoldModel)},
    {0x2E8FAD7,kHoldIndex,sizeof(kHoldIndex)},{0x202AEF0,kHover,sizeof(kHover)},{0x2E8B9D6,kDieEntity,sizeof(kDieEntity)},
    {0x648030,kWorldPos,sizeof(kWorldPos)},{0x1A76130,kEntityId,sizeof(kEntityId)},{0x2E951BC,kSetter,sizeof(kSetter)},
    {0x2E8FC4C,kHumanQuery,sizeof(kHumanQuery)},{0x2E9006F,kStickSet,sizeof(kStickSet)}};
// vft slot -> function, all RVAs.
struct Slot {std::uintptr_t at,fn;};
inline constexpr Slot kSlots[]{
    {0x3F72098+0x178,0x2E8E45C}, // C_Dice: input allowed
    {0x3F711E0+0x20,0x9DD2D0},{0x3F711E0+0x40,0x2E849C8}, // PlayerTurn
    {0x473F1C8+0x20,0x9DD2D0},{0x473F1C8+0x40,0x2E849C8}, // Holding
    {0x4072C78+0x8,0x1A76130},{0x4072C78+0x170,0x648030}};  // CEntity id / world pos
constexpr std::uintptr_t kGameModelVft=0x473F178,kDieModelVft=0x473F160,kEntityVft=0x4072C78;
constexpr std::uintptr_t kPlayerTurnVft=0x3F711E0,kHoldingVft=0x473F1C8;

template<class M> bool pins(M& m,std::uintptr_t b) noexcept {
    for(const auto& p:kPins)if(!m.same(b+p.rva,p.bytes,p.size))return false;
    for(const auto& s:kSlots){std::uintptr_t fn=0;if(!m.read(b+s.at,fn)||fn!=b+s.fn)return false;}
    return true;
}
// Fills t only for the human's current choosing turn; false otherwise.
template<class M> bool readTurn(M& m,std::uintptr_t b,std::uintptr_t a,Turn& t) noexcept {
    t={};t.instance=a;
    int phase=0;std::uintptr_t gm=0,gmVft=0,state=0,stateVft=0,current=0,human=0;
    if(!m.read(a+0x498,phase)||phase!=5||!m.read(a+0x4A0,gm)||!gm||!m.read(gm,gmVft)||gmVft!=b+kGameModelVft||
       !m.read(gm+0x108,state)||!state||!m.read(state,stateVft)||(stateVft!=b+kPlayerTurnVft&&stateVft!=b+kHoldingVft)||
       !m.read(state+0x18,current)||!m.read(gm+0x20,human)||!current||current!=human)return false;
    int slot=-1;std::uintptr_t list=0,first=0,last=0;
    if(!m.read(current+0x58,slot)||slot<0||slot>1||!m.read(a+0x4B0+16*std::uintptr_t(slot),list)||!list||
       !m.read(list+0xF0,first)||!m.read(list+0xF8,last)||!first||last<first||(last-first)%16||
       (last-first)/16==0||(last-first)/16>kMaxDice||!m.read(a+0x4F0,t.cursor))return false;
    t.player=current;t.count=unsigned((last-first)/16);
    for(unsigned i=0;i<t.count;++i){
        auto& d=t.dice[i];std::uintptr_t die=0,model=0,vft=0;std::uint32_t id=0;
        if(!m.read(first+16*i,die)||!die||!m.read(die+0x48,model)||!model||!m.read(model,vft)||vft!=b+kDieModelVft||
           !m.read(model+0x28,d.index)||!m.read(model+0x40,d.state))return false;
        if(d.index!=int(i))return false; // native cursor index == list position, else not this layout
        if(!m.read(die+0x30,id)||!id)continue;
        const std::uintptr_t e=m.entity(id);std::uintptr_t ev=0;std::uint32_t idAgain=0;
        if(!e||!m.read(e,ev)||ev!=b+kEntityVft||!m.read(e+0xC,idAgain)||idAgain!=id||
           !m.read(e+0x64,d.p[0])||!m.read(e+0x74,d.p[1])||!m.read(e+0x84,d.p[2]))continue;
        d.pos=std::isfinite(d.p[0])&&std::isfinite(d.p[1])&&std::isfinite(d.p[2]);
    }
    std::uintptr_t stateAgain=0,lastAgain=0;int phaseAgain=0;
    return m.read(gm+0x108,stateAgain)&&stateAgain==state&&m.read(list+0xF8,lastAgain)&&lastAgain==last&&
        m.read(a+0x498,phaseAgain)&&phaseAgain==5;
}
// One completed native frame. Leaves: m (memory/entity), set(index) = native cursor
// setter + readback, note(text) = one KCD2VR.log line. Returns the menu status.
struct Witness {std::uintptr_t instance=0,base=0;std::uint64_t lifetime=0,visit=0;bool ready=false,quiet=false,dice=false,retail=false;};
struct Frame {Owner owner{};std::uintptr_t checked=0;bool good=false;std::uint64_t refusedVisit=0;};
template<class M,class Set,class Note>
Status frameEnd(Frame& f,bool on,const Witness& w,M& m,const Ray& r,std::uint64_t now,Set&& set,Note&& note) noexcept {
    if(!on||!w.instance||!w.ready){f.owner={};return on?Status::WaitTurn:Status::Off;}
    if(f.checked!=w.base){f.checked=w.base;f.good=w.base&&w.retail&&pins(m,w.base);}
    if(!f.good){note("D85 DICE_HANDS gamepad: dice code of this game version not recognised");return Status::Unsupported;}
    Turn t{};
    const bool turn=w.quiet&&w.dice&&readTurn(m,w.base,w.instance,t);
    t.lifetime=w.lifetime;
    const auto d=step(f.owner,true,turn,t,r,now);
    if(d.status==Status::NoRay)note("D85 DICE_HANDS gamepad: no right hand ray (tracking or camera)");
    if(d.set>=0&&!set(d.set)){f.refusedVisit=w.visit;note("D85 DICE_HANDS gamepad: native dice cursor did not take the pointed die");}
    // A refusal stays visible for the rest of this dice visit; the stick still works.
    return f.refusedVisit&&f.refusedVisit==w.visit?Status::Refused:d.status;
}
}
