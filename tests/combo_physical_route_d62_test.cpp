// The real authored-table policy and real NativeInputClass117 callback run.
// Native memory, candidate ownership and caller addresses are explicit leaves;
// no RPG result, selector success, charge, hit or animation is simulated.
#include "combat_backups_d51_host.h"
#include "subsystems/masterstrike_combos/combo_zone_assist_cz.h"
#include "subsystems/masterstrike_combos/physical_finisher_cb1.h"
#include "vrik/physical_weapon.h"
#include <cstdio>
#include <string>
namespace cz=kcd2vr::combo_zone_assist_cz;
namespace cb=kcd2vr::finisher_cb1;
namespace ph=kcd2vr::physical;
namespace fixture {
constexpr std::uintptr_t base=0x10000000,actor=0x10000,player=0x20000,state=0x30000,self=0x40000;
struct NativeOwner104 {void* player=nullptr;void* actor=nullptr;void* state=nullptr;std::uint64_t epoch=0;};
struct NativeInputScope117 {NativeOwner104 owner{};ph::CommandKind stroke=ph::CommandKind::None;
    bool prepare=false,visited=false,fistPunchR19=false;int passed=-1;};
struct FistOwner101 {bool empty=false;void* player=nullptr;std::uint64_t epoch=0,request=0;};
struct FistNative {std::uint64_t request=0;} g_fist_native101;
std::atomic<std::uintptr_t> g_input_contract117{base};
std::atomic<bool> g_thrust_as_swing55{true};
NativeInputScope117* g_input_scope117=nullptr;
namespace retail_build112 {bool Ready(std::uintptr_t p) noexcept {return p==base;}}
std::uintptr_t WhBase() noexcept {return base;}
template<class T>bool Read(const void* p,T& out) noexcept {return hostD51::read(reinterpret_cast<std::uintptr_t>(p),&out,sizeof(out));}
bool NativeResolve104(void* p,NativeOwner104& out) noexcept {
    if(p!=reinterpret_cast<void*>(player))return false;
    out={p,reinterpret_cast<void*>(actor),reinterpret_cast<void*>(state),9};return true;
}
bool FistReadOwner101(FistOwner101&) noexcept {return false;}
#include "input_class_d62_production.inc"
}
unsigned checks=0,failures=0;
void check(bool ok,const char* why){++checks;if(!ok){++failures;std::printf("FAIL %s\n",why);}}
int main(){
    using namespace fixture;
    hostD51::put(self+0x18,actor);hostD51::put(base+0x5321014,18);hostD51::put(base+0x5321010,19);hostD51::put(base+0x5321018,20);
    NativeInputScope117 scope{};NativeResolve104(reinterpret_cast<void*>(player),scope.owner);
    scope.stroke=ph::CommandKind::Thrust;g_input_scope117=&scope;
    unsigned corrected=0;
    for(const auto& r:cz::kRecipes){
        std::uint8_t guid[16]{};cz::guidBytes(r.technique,guid);const auto* physical=cb::findRow(guid);
        const auto step=unsigned(r.count-1);const auto& s=r.steps[step];
        const std::string full=std::string(r.group)+"_"+r.id;
        const bool expected=full=="Blunt_C15"||full=="BluntShield_C15"||full=="Halberd_C05";
        cz::NativeCandidate observed{};std::memcpy(observed.technique,guid,16);observed.cursor=step;observed.required=r.count;
        const auto observedPick=cz::choose(true,true,&observed,1,0,cz::Kind::Stab,true);
        if(!expected){check(bool(observedPick.recipe)==(cz::fits(s,cz::Kind::Stab,true)==cz::Result::Assisted),
                          "unrelated final step retains the original transport/type policy");continue;}
        ++corrected;
        check(!s.charged&&s.kind==cz::StepKind::Slash&&physical&&physical->motion==kcd2vr::bodycombo59c7g::Motion::Thrust,
              "exception preserves native uncharged Slash row and joined descriptive thrust");
        cz::NativeCandidate candidate{};std::memcpy(candidate.technique,guid,16);candidate.cursor=step;candidate.required=r.count;
        const auto pick=cz::choose(true,true,&candidate,1,0,cz::Kind::Stab,true);
        check(pick.recipe==&r&&pick.zone==s.zone&&(pick.result==cz::Result::Assisted||pick.result==cz::Result::SameZone),
              "native candidate final-step selection announces the original authored zone");
        for(bool prepare:{true,false}){
            scope.prepare=prepare;scope.visited=false;scope.passed=-1;g_thrust_as_swing55=true;
            const auto caller=base+(prepare?0x27C08E6:0x27C0916);
            const int value=NativeInputClass117(reinterpret_cast<void*>(self),18,caller,prepare);
            check(value==18&&scope.visited&&scope.passed==18,"actual current input-class callback transports physical thrust as native HEAVY at both edges");
            scope.visited=false;scope.passed=-1;g_thrust_as_swing55=false;
            check(NativeInputClass117(reinterpret_cast<void*>(self),18,caller,prepare)==19&&scope.visited,
                  "actual existing LIGHT transport is unchanged when thrust-as-swing is OFF");
        }
        check(!cz::choose(true,true,&candidate,1,0,cz::Kind::Stab,false).recipe,"no exception borrows the wrong native LIGHT transport");
        check(!cz::choose(false,true,&candidate,1,0,cz::Kind::Stab,true).recipe&&
              !cz::choose(true,false,&candidate,1,0,cz::Kind::Stab,true).recipe,"disabled assistance or closed native slot stays unmodified");
        check(cb::judge(physical,cb::Kind::Thrust,false,0)==cb::Verdict::WrongRegion,
              "routing does not waive the separate physical finisher region gate");
    }
    check(corrected==3,"all three impossible-assistance rows are repaired, no broad type rewrite");
    for(const auto& r:cz::kRecipes)for(unsigned step=1;step<r.count;++step)
        if(step+1<r.count||r.steps[step].charged){
            cz::NativeCandidate candidate{};cz::guidBytes(r.technique,candidate.technique);candidate.cursor=step;candidate.required=r.count;
            const auto pick=cz::choose(true,true,&candidate,1,0,cz::Kind::Stab,true);
            check(bool(pick.recipe)==(cz::fits(r.steps[step],cz::Kind::Stab,true)==cz::Result::Assisted),
                  "prefix and native charged-step rules are unchanged");
        }
    g_thrust_as_swing55=true;scope.prepare=true;scope.visited=false;
    check(NativeInputClass117(reinterpret_cast<void*>(self),18,base+0x1234,true)==18&&!scope.visited,
          "unrelated native caller is not claimed by the physical transport");
    g_input_scope117=nullptr;
    check(NativeInputClass117(reinterpret_cast<void*>(self),18,base+0x27C08E6,true)==18,
          "foreign input remains native without a physical scope");
    std::printf("D62 authored combo route + actual native input-class callback: %u/%u; native game NOT_RUN\n",checks-failures,checks);
    return failures?1:0;
}
