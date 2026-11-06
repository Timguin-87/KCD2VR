// The production SubmitHit detour and live-pair checks run unchanged. Only
// native execution, the mode query, and the caller-address leaf are replaced.
#include <cstdint>
#include <string>
#include <vector>
#ifdef _MSC_VER
#include <intrin.h>
static void*& caller176=hostReturnAddressValue;
#else
static void* caller176=nullptr;
#endif
#define __builtin_return_address(n) caller176
#include "../src/combat_diag_probe.cpp"
#undef __builtin_return_address

// This incoming-Submit fixture never owns a released ordinary action or a
// corpse physics body. Keep unrelated native providers explicitly unavailable;
// otherwise their newer declarations leave the actual Windows TU unlinked.
namespace kcd2vr::frik_pose_probe {
bool ReadNativeComboSlotF1(const void*,subsystems::masterstrike_combos::NativeSlotF1&) noexcept {return false;}
bool GeometryCorpseBodyH7(std::uint32_t,std::uintptr_t,std::uintptr_t,int,bool) noexcept {return false;}
bool GeometryCorpseContactFaultH7() noexcept {return true;}
}

static bool f4176=true,nativeReturn176=true,wrongOutputIds176=false;
static void* henry176=nullptr;
static unsigned nativeCalls176=0,checks176=0,failures176=0;
static std::vector<std::string> messages176;
enum class Mutation176 { None,Clear,Repress,Owner,Opponent,Mode,Recorder };
static Mutation176 mutation176=Mutation176::None;
namespace kcd2vr::diagnostic_trace { void Write(const char* s) noexcept {messages176.emplace_back(s);} }
namespace kcd2vr::frik_pose_probe {
// This passive native probe fixture has no physical fist owner.
int FistContactAdmissionH12(const void*,const float*,int,const void*) noexcept {return -1;}

// The diagnostic-only native namespace leaf has its own production fixture.
void ObserveHitPartNamespace62(const kcd2vr::part_namespace62::Ticket&) noexcept {}

bool CombatExperiment175Enabled() noexcept {return f4176;}
bool PhysicalWeaponInputOwned6698() noexcept {return false;}
bool ReadF4ContactLive176(kcd2vr::contact176::Live&) noexcept {return false;}
void* LastPlayerEntity() noexcept {return henry176;}
void NotifyNativeWeaponImpact136R2(float,float,float,bool,bool,bool,std::uint32_t) noexcept {}
}
// Unused service boundaries still need explicit fail-closed definitions for
// MSVC's linker. The tested SubmitHit implementation/native leaf stays above.
namespace kcd2vr::retail_build112 {bool Verify(std::uintptr_t) noexcept{return false;} bool Ready(std::uintptr_t) noexcept{return false;}}
namespace kcd2vr::frik_pose_probe {
bool PhysicalCombat177Enabled() noexcept{return false;}
bool CombatIntegrated178r22c2Enabled() noexcept{return false;}
int F4ContactGateMode178r26() noexcept{return 1;}
bool GeometryAutoLock178r29() noexcept{return false;}
bool GeometryOwnerValid178r30(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t) noexcept{return false;}
bool GeometryAdmittedRelease51(const kcd2vr::contact176::Ticket&,const void*) noexcept{return false;}
bool GeometryNativeAnswer19c4r2(const void*,const void*,int,kcd2vr::answer59c4r2::Lineage&) noexcept {return false;}
bool GuardOwnedByUs59c7e(const void*) noexcept {return false;}
int GeometryOwnerStage49() noexcept{return 0;}
int FocusedTestMode167() noexcept{return 0;}
int WeaponControlMode() noexcept{return 0;}
void* WeaponControlEntity(int) noexcept{return nullptr;}
void ObserveCollisionAnimationCharacter(void*,std::uint32_t,std::uint64_t) noexcept{}
}
namespace kcd2vr::animation_filter {
void MarkContactForPlayback(std::uint64_t) noexcept{}
PlaybackStamp DiagnosticPlaybackStamp() noexcept{return {};}
}
using namespace kcd2vr::combat_diag_probe;
namespace n176=nhp134;
namespace p176=kcd2vr::diagnostics::parry176;
namespace c176=kcd2vr::diagnostics::capture168;
template<class T> static void put176(void* p,std::size_t off,T v){std::memcpy(static_cast<unsigned char*>(p)+off,&v,sizeof(v));}
static void check176(bool b,const char* m){++checks176;if(!b){++failures176;std::printf("FAIL %s\n",m);}}
struct Actor176 {
    std::array<unsigned char,0x300> actor{};
    std::array<unsigned char,0xD20> state{};
    std::array<unsigned char,0x400> player{};
    std::array<unsigned char,0x80> entity{};
    void initialize(bool isHenry,unsigned id){
        put176(actor.data(),0,n176::g_base+(isHenry?n176::kCombatPlayerVft:n176::kCombatActorVft));
        put176(actor.data(),0x210,state.data());put176(state.data(),0xCE0,actor.data());
        put176(entity.data(),0,n176::g_base+n176::kEntityVft);put176(entity.data(),0xC,id);
        if(isHenry){
            put176(actor.data(),0x1F8,player.data());put176(player.data(),0,n176::g_base+n176::kPlayerVft);
            put176(player.data(),0x278,actor.data());put176(player.data(),0x38,entity.data());
        }else put176(actor.data(),0x1F8,entity.data());
    }
};
struct Arguments176 {void* rpg;void* hit;const float* strength;bool* post;const void* request;bool secondary;int type;void* details;};
static Arguments176 forwarded176{};
static bool __fastcall Native176(void* rpg,void* hit,const float* strength,bool* post,const void* request,bool secondary,int type,void* details){
    ++nativeCalls176;forwarded176={rpg,hit,strength,post,request,secondary,type,details};
    const void* a=n176::Rd<const void*>(rpg,8,nullptr);const void* t=n176::Rd<const void*>(request,0x10,nullptr);
    const void *ae=nullptr,*as=nullptr,*te=nullptr,*ts=nullptr;
    n176::CombatActor(a,ae,as,false);n176::CombatActor(t,te,ts,false);
    if(nativeReturn176){
        put176(hit,0,wrongOutputIds176?999u:n176::EntityId(ae));put176(hit,0x28,n176::EntityId(te));
        put176(hit,0x30,.25f);put176(hit,0x2C,1.5f);
        put176(details,1,std::uint8_t(1));put176(details,3,std::uint8_t(1));put176(details,7,std::uint8_t(0));
    }
    if(mutation176==Mutation176::Clear)p176::Clear();
    if(mutation176==Mutation176::Repress){p176::Edge e{};p176::Read(e);++e.epoch;p176::Publish(e,p176::publication.load());}
    if(mutation176==Mutation176::Owner)put176(const_cast<void*>(ts),0xCE0,static_cast<void*>(nullptr));
    if(mutation176==Mutation176::Opponent)put176(const_cast<void*>(ts),0xCF8,static_cast<void*>(nullptr));
    if(mutation176==Mutation176::Mode)f4176=false;
    if(mutation176==Mutation176::Recorder)c176::gate.select(c176::Domain::Combat,false,hostTickCount64);
    return nativeReturn176;
}
static std::vector<std::string> outcomes176(){
    std::vector<std::string> result;for(const auto& m:messages176)if(m.find("V66.176 F4_PARRY_RESULT")!=std::string::npos)result.push_back(m);
    return result;
}
int main(){
    n176::g_base=0x180000000ull;n176::g_submit_original=Native176;n176::g_vr_weapon_log136=true;
    Actor176 h,e;h.initialize(true,100);e.initialize(false,200);henry176=h.entity.data();
    std::array<unsigned char,0x80> rpg{},request{},hit{},details{};
    float strength=.9f;bool post=false;p176::Edge edge{};
    const auto reset=[&](){
        hostTickCount64+=2000;f4176=nativeReturn176=true;wrongOutputIds176=false;mutation176=Mutation176::None;
        h.initialize(true,100);e.initialize(false,200);henry176=h.entity.data();
        put176(h.state.data(),0xCF8,e.actor.data());put176(e.state.data(),0xCF8,h.actor.data());
        put176(rpg.data(),8,e.actor.data());put176(request.data(),0x10,h.actor.data());
        hit.fill(0);details.fill(0);messages176.clear();p176::Clear();
        p176::results55c2=0;p176::blocks55c2=0;p176::perfect55c2=0;
        c176::gate.select(c176::Domain::Combat,true,hostTickCount64);caller176=reinterpret_cast<void*>(n176::g_base+0xD536BC);
        edge={};edge.tick=hostTickCount64-50;edge.epoch=8;
        edge.player=reinterpret_cast<std::uintptr_t>(h.player.data());edge.actor=reinterpret_cast<std::uintptr_t>(h.actor.data());
        edge.state=reinterpret_cast<std::uintptr_t>(h.state.data());edge.entity=reinterpret_cast<std::uintptr_t>(h.entity.data());
        edge.opponentActor=reinterpret_cast<std::uintptr_t>(e.actor.data());edge.opponentState=reinterpret_cast<std::uintptr_t>(e.state.data());
        edge.opponentEntity=reinterpret_cast<std::uintptr_t>(e.entity.data());edge.henryId=100;edge.opponentId=200;
        edge.cause=p176::Cause::Motion;edge.manualHeld=true;edge.pressReturned=edge.releaseReturned=true;edge.incomingSector=2;
    };
    const auto invoke=[&](unsigned expected,const char* label){
        p176::Publish(edge,p176::publication.load());const auto before=nativeCalls176;
        const bool result=n176::SubmitHitDetour(rpg.data(),hit.data(),&strength,&post,request.data(),true,1,details.data());
        check176(nativeCalls176==before+1&&result==nativeReturn176,"native called once and boolean unchanged");
        check176(forwarded176.rpg==rpg.data()&&forwarded176.hit==hit.data()&&forwarded176.strength==&strength&&
            forwarded176.post==&post&&forwarded176.request==request.data()&&forwarded176.secondary&&
            forwarded176.type==1&&forwarded176.details==details.data(),"every native argument forwarded unchanged");
        check176(outcomes176().size()==expected,label);
    };
    reset();invoke(1,"incoming native perfect block associates with completed held-LT motion edge");
    auto text=outcomes176().back();
    check176(text.find("cause=MOTION")!=std::string::npos&&text.find("manualHeld=1 releaseReturned=1 pressReturned=1")!=std::string::npos&&
        text.find("nativeResultFields=VALID blocked=1 perfectBlock=1 masterStrike=0")!=std::string::npos&&
        text.find("association=FRESH_SAME_PAIR_TEMPORAL_NOT_CAUSAL")!=std::string::npos,"record distinguishes input delivery, native result and temporal association");
    reset();edge.cause=p176::Cause::Initial;edge.manualHeld=edge.releaseReturned=false;invoke(1,"timed initial block edge can associate without fabricated release");
    reset();edge.cause=p176::Cause::Manual;edge.releaseReturned=false;invoke(1,"ordinary native LT press can associate");
    reset();nativeReturn176=false;details.fill(1);invoke(1,"native rejection is observable but cannot borrow old success fields");
    check176(outcomes176().back().find("nativeSubmitReturned=0 nativeResultFields=UNAVAILABLE blocked=-1 perfectBlock=-1")!=std::string::npos,"uninitialized native result fields stay unavailable");
    reset();wrongOutputIds176=true;invoke(1,"mismatching output IDs retain input association but cannot claim native result fields");
    check176(outcomes176().back().find("nativeResultFields=UNAVAILABLE")!=std::string::npos,"wrong hit output never certifies Henry parry");
    reset();put176(rpg.data(),8,h.actor.data());put176(request.data(),0x10,e.actor.data());invoke(0,"NPC perfect-block against Henry is excluded");
    reset();f4176=false;invoke(0,"F12 cannot be relabeled as F4 parry");
    reset();c176::gate.select(c176::Domain::Combat,false,hostTickCount64);invoke(0,"F3 OFF has no parry formatting");
    check176(messages176.empty(),"all diagnostic output stays quiet when recorder OFF");
    check176(p176::results55c2==1&&p176::blocks55c2==1&&p176::perfect55c2==1,"F3 OFF still counts independently verified incoming native PB result");
    reset();edge.cause=p176::Cause::NativeWindow;edge.manualHeld=false;invoke(1,"native slot input association retains verified incoming result");
    check176(outcomes176().back().find("cause=NATIVE_WINDOW")!=std::string::npos&&p176::perfect55c2==1,"native result is distinct from timed input delivery");
    for(int invalid=0;invalid<4;++invalid){reset();c176::gate.select(c176::Domain::Combat,false,hostTickCount64);
        if(invalid==0)nativeReturn176=false;if(invalid==1)wrongOutputIds176=true;
        if(invalid==2){put176(rpg.data(),8,h.actor.data());put176(request.data(),0x10,e.actor.data());}
        if(invalid==3)mutation176=Mutation176::Opponent;
        invoke(0,"F3 OFF invalid or outgoing receipt never formats");
        check176(p176::results55c2==0&&p176::perfect55c2==0,"F3 OFF invalid or outgoing native receipt cannot claim Henry perfect block");}

    reset();caller176=reinterpret_cast<void*>(n176::g_base+0xC6248F);invoke(0,"synthetic followup is not incoming collision evidence");
    reset();edge.tick=hostTickCount64+1;invoke(0,"future edge cannot associate");
    reset();edge.tick=hostTickCount64-1501;invoke(0,"stale edge cannot associate");
    reset();edge.releaseReturned=false;invoke(0,"incomplete motion release cannot certify a fresh edge");
    reset();edge.pressReturned=false;invoke(0,"undelivered press cannot associate");
    reset();edge.henryId=101;invoke(0,"reused Henry pointer with wrong entity ID cannot associate");
    reset();edge.opponentId=201;invoke(0,"reused opponent pointer with wrong entity ID cannot associate");
    reset();edge.epoch=0;invoke(0,"unknown pose epoch cannot associate");
    reset();edge.opponentState=reinterpret_cast<std::uintptr_t>(h.state.data());invoke(0,"wrong opponent state cannot associate");
    reset();put176(e.state.data(),0xCF8,static_cast<void*>(nullptr));invoke(0,"nonreciprocal native opponent cannot associate");
    for(const auto mutation:{Mutation176::Clear,Mutation176::Repress,Mutation176::Owner,Mutation176::Opponent,Mutation176::Mode,Mutation176::Recorder}){
        reset();mutation176=mutation;invoke(0,"mode/owner/pair/recorder or newer edge change during original revokes association");
    }
    std::printf("parry_outcome176=%u/%u production_submit=YES native_leaves=REPLACED headset=NOT_EXECUTED\n",checks176-failures176,checks176);
    return failures176?1:0;
}
