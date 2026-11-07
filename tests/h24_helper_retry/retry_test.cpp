#include "vrik/physical_weapon.h"
#include <cstdio>
namespace ph=kcd2vr::physical;namespace pm=kcd2vr::pose_core;
#include "actual_item.inc"
static unsigned calls=0;static bool ready=false,slotReady=false;static ph::Pose helper{},slotHelper{};
static bool PhysHelper34(void*,const char* name,ph::Pose& out,bool local){++calls;if(local)return false;if(std::strcmp(name,"slt_0")==0){if(!slotReady)return false;out=slotHelper;return true;}if(!ready)return false;out=helper;return true;}
#include "actual_retry.inc"
static int n=0,bad=0;static void ck(bool ok,const char* s){++n;bad+=!ok;printf("%s %s\n",ok?"PASS":"FAIL",s);}
int main(){
 PhysEquipmentItem6698 x{};x.entity=reinterpret_cast<void*>(1);x.profile.classId=4;x.profile.generation=1;
 ck(!PhysReadGuardFaceH24(x)&&calls==2&&!x.profile.guardFaceReadyH24,"real initial item resolve may precede helper readiness");
 for(int i=0;i<100;++i)PhysRetryGuardFaceH24(x,10);
 ck(calls==2&&x.guardFaceRetryCountH24==0,"stowed metadata refreshes never poll missing helper");
 x.held=true;PhysRetryGuardFaceH24(x,10);
 ck(calls==4&&x.guardFaceRetryCountH24==1&&!x.profile.guardFaceReadyH24,"draw retries actual missing helper once through production policy");
 ready=true;helper.q={0,0,.7071067811865475,.7071067811865475};PhysRetryGuardFaceH24(x,10);
 ck(calls==5&&x.profile.guardFaceReadyH24&&x.profile.guardFaceNormalH24.y>.999,"loading helper recovers into actual profile with measured orientation");
 for(int i=0;i<100;++i)PhysRetryGuardFaceH24(x,10);
 ck(calls==5&&x.guardFaceRetryCountH24==2,"resolved profile stops all retry work");
 ready=false;PhysEquipmentItem6698 missing{};missing.entity=x.entity;missing.profile.classId=4;missing.held=true;
 const auto before=calls;for(int i=0;i<1000;++i)PhysRetryGuardFaceH24(missing,20);
 ck(calls-before==8&&missing.guardFaceRetryCountH24==4&&!missing.profile.guardFaceReadyH24,"genuinely absent helper is bounded to four held attempts per request");
 PhysRetryGuardFaceH24(missing,21);ck(calls-before==10&&missing.guardFaceRetryCountH24==1,"new request receives a bounded fresh retry budget");
 missing.held=false;PhysRetryGuardFaceH24(missing,21);missing.held=true;PhysRetryGuardFaceH24(missing,21);
 ck(calls-before==12&&missing.guardFaceRetryCountH24==1,"stow and redraw rearm loading retry without replacing a working profile");
 auto other=missing;other.profile.classId=3;const auto b=calls;PhysRetryGuardFaceH24(other,30);other.profile.classId=4;other.entity=nullptr;PhysRetryGuardFaceH24(other,30);
 ck(calls==b,"other weapon classes and absent entities never call helper");
 // Only the proven helper-local normal is used in the fallback.
 ready=false;slotReady=true;slotHelper.q={0,0,.7071067811865475,.7071067811865475};
 PhysEquipmentItem6698 jm{};jm.entity=x.entity;jm.profile.classId=4;
 const auto beforeFallback=calls;
 ck(PhysReadGuardFaceH24(jm)&&calls-beforeFallback==2&&jm.profile.guardFaceReadyH24&&jm.profile.guardFaceNormalH24.x>.999&&std::abs(jm.profile.guardFaceNormalH24.y)<1.e-6,
    "missing sharpener uses actual slt_0 local minusY basis, not helperX or wrist pose");
 ready=true;const auto beforePreferred=calls;PhysReadGuardFaceH24(jm);
 ck(calls-beforePreferred==1&&jm.profile.guardFaceNormalH24.y>.999,"available sharpening helper remains first authority without unnecessary slot query");
 printf("%dchecks %dfailures; metadata refresh scheduling is production-owned\n",n,bad);return bad?1:0;
}
