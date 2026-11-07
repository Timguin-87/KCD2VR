// Exact function bodies are extracted on every run from the production files.
#include "subsystems/throw_drop/native_ammo_service_t22.h"
#include "vrik/weapon_release_t1.h"
#include "vrik/physical_weapon.h"
#include "core/initial_draw_completion_h36r3.h"
#include <cstdio>
#include <atomic>
namespace st2=kcd2vr::supply_t2;namespace svc22=kcd2vr::ammo_service_t22;
namespace adm23=kcd2vr::throw_admission_t23;namespace ta20=kcd2vr::transient_ammo_t20;
namespace paid20=kcd2vr::paid_ammo_t20;namespace pl21=kcd2vr::private_launcher_t21;
namespace ac18=kcd2vr::ammo_carry_t18;namespace ng13=kcd2vr::grasp13;
namespace wrt1=kcd2vr::weapon_release_t1;namespace ph=kcd2vr::physical;
static int g_ammo_current22[2]{-1,-1};static std::atomic<unsigned>g_ammo_cleanup_pending_d48{0};
static const st2::Input*g_ammo_input22=nullptr;
static kcd2vr::core::SnapshotMailbox<svc22::Status>g_ammo_status22[2];
static std::atomic<bool>g_supply_cleanup_t2{false};static bool readiness=false,claimBlocked=false;
static bool AmmoServiceReadinessWantedT22(){return readiness;}
static bool AmmoServiceClaimPublishT22(int,const ac18::Claim&){return !claimBlocked;}
static void AmmoNoteT18(const char*,int,std::uint64_t,std::uint64_t,const char*){}
struct PhysEquipment6698{};
struct LeftEquipmentObligationH36 {bool active=false,entered=false,uncertain=false;bool selectionEnteredD48=false,selectionReturnedD48=false;kcd2vr::initial_draw_h36r3::Watch initialDrawH36r3{};};
static LeftEquipmentObligationH36 obligation{};static bool exact=true;static unsigned refreshes=0;
static LeftEquipmentObligationH36*FindLeftEquipmentH36(const PhysEquipment6698&,const ph::Command&){return exact&&obligation.active?&obligation:nullptr;}
static void PhysLeftEquipmentRefreshH36(const PhysEquipment6698&,std::uint64_t){++refreshes;}
static std::uint64_t GetTickCount64(){return 1000;}
#include "holster_lifecycle_actual_d48.inc"
static unsigned checks=0,failed=0;
static void ck(bool value,const char* text){++checks;if(!value){++failed;std::printf("FAIL %s\n",text);}}
int main(){
    st2::enabled=false;wrt1::SetMode(0);ac18::hands=0;ac18::flights=0;
    ck(!AmmoServiceOwnsT22()&&!SupplyWantedT2(),"OFF empty service is not kept alive");
    AmmoServiceSetCurrentD48(1,4);
    ck(g_ammo_current22[1]==4&&g_ammo_cleanup_pending_d48.load()==2,"current native record atomically publishes outstanding work");
    ck(AmmoServiceOwnsT22()&&SupplyWantedT2(),"OFF cleanup still schedules exact token work after physical hand release");
    AmmoServiceRecordT22 left{};left.side=1;left.serial=123;left.handReleasedK32=true;left.stage=svc22::Stage::Retiring;
    AmmoServiceSetCurrentD48(0,2);
    ck(AmmoServiceReleaseClaimT22(left)&&g_ammo_current22[1]<0&&g_ammo_cleanup_pending_d48.load()==1,"left exact retirement preserves independent right cleanup");
    AmmoServiceRecordT22 right{};right.side=0;right.serial=124;right.stage=svc22::Stage::Retiring;
    ac18::hands=1;claimBlocked=true;
    ck(!AmmoServiceReleaseClaimT22(right)&&g_ammo_current22[0]==2&&g_ammo_cleanup_pending_d48.load()==1&&ac18::hands.load()==1,
        "failed hand-claim clear retains native cleanup scheduling and admission");
    claimBlocked=false;
    ck(AmmoServiceReleaseClaimT22(right)&&!AmmoServiceOwnsT22()&&!SupplyWantedT2()&&!g_ammo_cleanup_pending_d48.load(),
        "successful final exact retirement ends residual scheduling");
    st2::enabled=true;ck(AmmoServiceSelectedT22()&&AmmoServiceOwnsT22(),"normal finite chest preference still selects service");st2::enabled=false;
    ac18::flights=1;ck(AmmoServiceOwnsT22()&&SupplyWantedT2(),"flight owner still schedules service independently");ac18::flights=0;
    PhysEquipment6698 equipment{};ph::Command command{};command.kind=ph::CommandKind::Draw;
    obligation={true,false,false};refreshes=0;
    ck(PhysCancelUnenteredEquipmentH36(equipment,command)&&!obligation.active&&refreshes==1,"release retires exact never-entered native obligation");
    obligation={true,true,false};refreshes=0;
    ck(!PhysCancelUnenteredEquipmentH36(equipment,command)&&obligation.active&&obligation.entered&&!refreshes,
        "release cannot erase entered native debt");
    obligation.uncertain=true;
    ck(!PhysCancelUnenteredEquipmentH36(equipment,command)&&obligation.uncertain,"uncertain entered native action remains quarantined");
    exact=false;
    ck(PhysCancelUnenteredEquipmentH36(equipment,command)&&obligation.active&&obligation.entered&&obligation.uncertain,
        "unrelated rejected request cannot retire older native debt");
    std::printf("D48 extracted production lifecycle %u/%u; native leaves SIMULATED\n",checks-failed,checks);
    return failed?1:0;
}
