// Production ProcessHit gate. Retail caller/mode/live-frame resolution and
// native Process execution are explicit fixture leaves, not emulated game code.
#include <cstdint>
#include <string>
#include <vector>
static void* processCaller176=nullptr;
#define __builtin_return_address(n) processCaller176
#include "../src/combat_diag_probe.cpp"
#undef __builtin_return_address
namespace a176=kcd2vr::contact176;
namespace cap176=kcd2vr::diagnostics::capture168;
static bool combat176=true,liveOk176=true,nativeResult176=true;
static a176::Live live176{};
static void* playerEntity176=nullptr;
static unsigned calls176=0,checks176=0,failures176=0;
static std::vector<std::string> lines176;
namespace kcd2vr::diagnostic_trace {void Write(const char* text) noexcept {lines176.emplace_back(text);}}
namespace kcd2vr::frik_pose_probe {
// This passive native probe fixture has no physical fist owner.
int FistContactAdmissionH12(const void*,const float*,int,const void*) noexcept {return -1;}

bool CombatExperiment175Enabled() noexcept {return combat176;}
bool PhysicalCombat177Enabled() noexcept {return false;} // R55C2 host stub (GNU suite): this fixture exercises the rung-1 ticket gate, not the rung-2 denial
bool CombatIntegrated178r22c2Enabled() noexcept {return false;}
int F4ContactGateMode178r26() noexcept {return 1;} // R26: this fixture exercises the ticket gate
bool GeometryAutoLock178r29() noexcept {return false;} // R29 stub
bool GeometryOwnerValid178r30(std::uint64_t,std::uint64_t,std::uint64_t,std::uint64_t) noexcept {return true;} // R30 stub
bool GeometryAdmittedRelease51(const kcd2vr::contact176::Ticket&,const void*) noexcept {return false;} // separate clock fixture verifies actual admitted ownership
bool GeometryNativeAnswer19c4r2(const void*,const void*,int,kcd2vr::answer59c4r2::Lineage&) noexcept {return false;}
bool GuardOwnedByUs59c7e(const void*) noexcept {return false;}
int GeometryOwnerStage49() noexcept {return 0;} // R49 stub
int WeaponControlMode() noexcept {return combat176?7:6;}
bool PhysicalWeaponInputOwned6698() noexcept {return false;}
bool ReadF4ContactLive176(kcd2vr::contact176::Live& out) noexcept {out=live176;return liveOk176;}
void* LastPlayerEntity() noexcept {return playerEntity176;}
void NotifyNativeWeaponImpact136R2(float,float,float,bool,bool,bool,std::uint32_t) noexcept {}
}
using namespace kcd2vr::combat_diag_probe;
namespace native176=nhp134;
template<class T> static void put176(void* p,std::size_t off,T value){std::memcpy(static_cast<unsigned char*>(p)+off,&value,sizeof(value));}
static void ck176(bool value,const char* why){++checks176;if(!value){++failures176;std::printf("FAIL %s\n",why);}}
struct Actor176 {
    std::array<unsigned char,0x300> actor{};
    std::array<unsigned char,0xD20> state{};
    std::array<unsigned char,0x400> player{};
    std::array<unsigned char,0x80> entity{};
    void initialize(bool henry,unsigned id){
        put176(actor.data(),0,native176::g_base+(henry?native176::kCombatPlayerVft:native176::kCombatActorVft));
        put176(actor.data(),0x210,state.data());put176(state.data(),0xCE0,actor.data());
        put176(state.data(),native176::kAttackType+8,0);
        put176(entity.data(),0,native176::g_base+native176::kEntityVft);put176(entity.data(),0xC,id);
        if(henry){
            put176(actor.data(),0x1F8,player.data());put176(player.data(),0,native176::g_base+native176::kPlayerVft);
            put176(player.data(),0x278,actor.data());put176(player.data(),0x38,entity.data());
        }else put176(actor.data(),0x1F8,entity.data());
    }
};
static void* seenRpg176=nullptr;static void* seenHit176=nullptr;static const void* seenRequest176=nullptr;static void* seenDetails176=nullptr;
static bool __fastcall Original176(void* rpg,void* hit,const void* request,void* details){
    ++calls176;seenRpg176=rpg;seenHit176=hit;seenRequest176=request;seenDetails176=details;return nativeResult176;
}
int main(){
    native176::g_base=0x180000000ull;native176::g_process_original=Original176;
    Actor176 henry,enemy;std::array<unsigned char,0x300> collisions{};
    std::array<unsigned char,0x80> rpg{},request{},hit{},details{};
    std::array<unsigned char,32> samples{};
    unsigned serial=0;a176::Payload payload{};
    const auto reset=[&](){
        hostTickCount64+=2000;combat176=liveOk176=nativeResult176=true;native176::g_f4_contract176=true;
        native176::g_vr_weapon_log136=true;cap176::gate.mask=0;lines176.clear();
        henry.initialize(true,100);enemy.initialize(false,200);playerEntity176=henry.entity.data();
        put176(henry.actor.data(),0x218,collisions.data());put176(rpg.data(),8,henry.actor.data());
        put176(request.data(),0x10,enemy.actor.data());put176(request.data(),0x18,collisions.data()+0x170);
        put176(request.data(),0x20,samples.data());put176(request.data(),0x28,samples.data()+samples.size());
        for(unsigned i=0;i<samples.size();++i)samples[i]=static_cast<unsigned char>(i*7+3);
        hit.fill(0xA3);details.fill(0xBC);
        live176={};live176.now=hostTickCount64;
        live176.identity={reinterpret_cast<std::uintptr_t>(henry.player.data()),reinterpret_cast<std::uintptr_t>(henry.entity.data()),
            reinterpret_cast<std::uintptr_t>(henry.actor.data()),reinterpret_cast<std::uintptr_t>(collisions.data()),1,2,3,4,100,hostTickCount64-5};
        live176.ticket={live176.identity.player,live176.identity.entity,++serial,hostTickCount64-20,99,1,2,3,4};
        live176.ticket.policy=a176::policy.load();
        payload={};payload.identity=live176.identity;payload.ticket=live176.ticket;
        payload.wrapper=0x120;payload.action=0x340;payload.hand=0;payload.bone=8;payload.dt=.012f;payload.redirected=payload.moving=true;
        processCaller176=reinterpret_cast<void*>(native176::g_base+0xC52534);
    };
    const auto invoke=[&](bool forward,const char* why,bool scope=true){
        const auto before=calls176;const auto requestBefore=request,hitBefore=hit,detailsBefore=details;const auto samplesBefore=samples;
        bool result=false;
        if(scope){a176::SweepScope own(payload);result=native176::ProcessHitDetour(rpg.data(),hit.data(),request.data(),details.data());}
        else result=native176::ProcessHitDetour(rpg.data(),hit.data(),request.data(),details.data());
        ck176(calls176==before+unsigned(forward)&&result==(forward&&nativeResult176),why);
        ck176(request==requestBefore&&hit==hitBefore&&details==detailsBefore&&samples==samplesBefore,"gate never constructs/edits request, samples, energy or result bytes");
        if(forward)ck176(seenRpg176==rpg.data()&&seenHit176==hit.data()&&seenRequest176==request.data()&&seenDetails176==details.data(),"native receives original argument pointers exactly once");
    };
    reset();const auto counts=native176::g_counts[0].load();invoke(true,"quiet exact tracked primary contact reaches native Process");
    ck176(lines176.empty()&&native176::g_counts[0].load()==counts,"recorder OFF changes no diagnostic output/counters");
    invoke(false,"the same release cannot submit a second ordinary primary contact");
    reset();nativeResult176=false;invoke(true,"native false remains false and is never fabricated as accepted");
    invoke(false,"native rejection does not give the same physical release an automatic retry");
    reset();invoke(false,"ordinary canned sweep without synchronous VR authority is rejected",false);
    reset();combat176=false;invoke(true,"F12/other modes retain native Process",false);
    reset();native176::g_f4_contract176=false;invoke(true,"unarmed/unqualified gate cannot mutate native Process",false);
    reset();processCaller176=reinterpret_cast<void*>(native176::g_base+0xD538D2);invoke(true,"second native Process producer stays native",false);
    reset();processCaller176=reinterpret_cast<void*>(native176::g_base+0xC61543);invoke(true,"unowned caller is never intercepted",false);
    reset();put176(rpg.data(),8,enemy.actor.data());put176(request.data(),0x10,henry.actor.data());invoke(true,"NPC incoming hit stays native",false);
    for(int type:{2,3,4,5,6,7,8,9,-1}){
        reset();put176(henry.state.data(),native176::kAttackType+8,type);invoke(true,"native special/kick/master or unknown type stays outside ordinary gate",false);
    }
    reset();put176(henry.state.data(),native176::kAttackType+8,1);invoke(true,"qualified ordinary thrust is admitted");
    reset();liveOk176=false;invoke(false,"lost current live physical owner fails closed");
    reset();++live176.identity.epoch;invoke(false,"changed pose epoch fails closed");
    reset();++live176.identity.sample;invoke(false,"borrowed older pose sample cannot authorize a hit");
    reset();live176.identity.collisions=0;invoke(false,"changed collisions owner fails closed");
    reset();payload.identity.tick=live176.identity.tick=hostTickCount64-151;invoke(false,"expired completed pose cannot authorize hit (150 ms, V66.178R6)");
    reset();payload.ticket.tick=live176.ticket.tick=hostTickCount64-121;invoke(true,"V66.178R6: a 121 ms old release still owns its animation-bound native sweep");
    reset();payload.ticket.tick=live176.ticket.tick=hostTickCount64-1501;invoke(false,"release past the 1.5 s safety cap cannot authorize hit");
    reset();payload.ticket.tick=live176.ticket.tick=hostTickCount64+1;invoke(false,"future release cannot authorize hit");
    reset();payload.redirected=false;invoke(false,"unmodified native predictor cannot borrow VR authority");
    reset();payload.moving=false;invoke(false,"resting weapon cannot submit ordinary damage");
    reset();payload.hand=1;invoke(false,"unqualified offhand sweep cannot borrow primary contact authority");
    reset();put176(request.data(),0x18,collisions.data()+0x98);invoke(false,"different native details buffer cannot borrow main result authority");
    reset();put176(request.data(),0x10,static_cast<void*>(nullptr));invoke(false,"unknown target cannot authorize contact");
    reset();put176(request.data(),0x10,henry.actor.data());invoke(false,"self hit cannot authorize contact");
    reset();put176(enemy.state.data(),0xCE0,static_cast<void*>(nullptr));invoke(false,"broken native target backlink fails closed");
    reset();{
        a176::SweepScope outer(payload);a176::SweepScope nested;
        const auto before=calls176;ck176(!native176::ProcessHitDetour(rpg.data(),hit.data(),request.data(),details.data())&&calls176==before,"nested unrelated sweep masks outer authority");
    }
    ck176(a176::current==nullptr,"no borrowed native scope survives return");
    reset();cap176::gate.select(cap176::Domain::Combat,true,hostTickCount64);invoke(false,"recorded rejection has the same gameplay result",false);
    bool found=false;for(const auto& line:lines176)found|=line.find("F4_CONTACT_ADMISSION decision=NO_SYNCHRONOUS_SWEEP nativeCalled=0")!=std::string::npos;
    ck176(found,"F3 explains exact admission refusal without manufacturing a Process result");
    cap176::gate.select(cap176::Domain::Combat,false,hostTickCount64);lines176.clear();invoke(true,"turning recorder off still admits fresh qualified contact");
    ck176(lines176.empty(),"gameplay never enables hidden combat recording");
    std::printf("contact_gate176=%u/%u production_process=YES native_execution=NONE coverage=ORDINARY_HENRY_ONLY\n",checks176-failures176,checks176);
    return failures176?1:0;
}
