// Literal production BuildStagedPickup596 and StagedPropTick596. The retained
// fixture substitutes native memory/physics leaves, never branch decisions.
#define KCD2VR_PICKUP_BUILD_FIXTURE_ONLY_D81
#include "pickup_admission_d78_test.cpp"
#include <cstdlib>
static unsigned checksR96=0;
static void needR96(bool value,const char* label){++checksR96;if(!value){std::fprintf(stderr,"FAIL R96 %u %s\n",checksR96,label);std::exit(1);}}
static StagedProp596 acquireR96(int side=0){
 frame.mapGripM39[1-side]=0;frame.mapGripM39[side]=1;
 frame.allowed[side]=frame.propRequested5916c1[side]={};
 frame.allowed[1-side].p=frame.propRequested5916c1[1-side].p={1,0,0};
 StagedProp596 p{};needR96(BuildStagedPickup596(nativeController.data(),11,true,&p),"actual fresh native build accepted");
 p.instantD72=true;p.instantTokenD72=101;g_prop596=p;
 kcd2vr::mutual_admission_t25::pickupClaimsD72[side]=101;return p;
}
static void freshR96(std::uint64_t tick){now=tracking.tick=frame.stamp.tick=tick;g_stage_tick596=tick;++frame.stamp.sample;}
int main(){
 PropFixtureMainD78();
 for(int side=0;side<2;++side){
  prepare();frame.stamp.tick=1001;readClockR96=1001;auto p=acquireR96(side);readClockR96=0;
  needR96(p.sourceTickR96==1001&&p.hold.last==1001,"newer read is validated against post-read clock");
  needR96(committedReadsR96==1,"build validates one copied pose rather than borrowing replacement");
  StagedPropTick596();needR96(publishCalls==1&&velocityCalls==0,"first identical source publishes observed body without duplicate force");
  const auto source=g_prop596.sourceTickR96,last=g_prop596.hold.last;
  now=1020;committedAvailableR96=false;StagedPropTick596();
  needR96(g_prop596.id&&g_prop596.sourceTickR96==source&&g_prop596.hold.last==last,"temporary read gap keeps exact owner without renewal");
  needR96(velocityCalls==0&&publishCalls==1&&stowCalls==0&&kcd2vr::mutual_admission_t25::pickupClaimsD72[side]==101,"gap produces no native action or semantic completion");
  committedAvailableR96=true;freshR96(1040);frame.propRequested5916c1[side].p.x=.03;StagedPropTick596();
  needR96(g_prop596.id&&velocityCalls==1&&stowCalls==0&&dynamics.linear.x>0,"same occurrence resumes actual servo after read recovery");
  committedAvailableR96=false;now=1060;StagedPropTick596();committedAvailableR96=true;
  freshR96(1080);frame.propRequested5916c1[side].p.x=.04;StagedPropTick596();
  needR96(g_prop596.id&&velocityCalls==2,"second observation gap recovers without fixture/owner reset");
  freshR96(1100);frame.mapGripM39[side]=0;const auto velocity=dynamics.linear.x;StagedPropTick596();
  needR96(!g_prop596.id&&velocityCalls==2&&dynamics.linear.x==velocity&&!kcd2vr::mutual_admission_t25::pickupClaimsD72[side],"actual release preserves achieved momentum and relinquishes exact hand");
 }
 prepare();frame.stamp.tick=1001;StagedProp596 p{};
 needR96(!BuildStagedPickup596(nativeController.data(),11,true,&p),"genuine future source still denied");
 prepare();frame.stamp.tick=919;needR96(!BuildStagedPickup596(nativeController.data(),11,true,&p),"genuine stale source still denied");
 prepare();acquireR96();committedAvailableR96=false;now=1081;StagedPropTick596();
 needR96(!g_prop596.id&&velocityCalls==1&&stowCalls==0,"unavailable evidence cannot extend original 80ms age");
 prepare();acquireR96();g_ctx_menu=true;now=1001;StagedPropTick596();
 needR96(!g_prop596.id&&stowCalls==0,"positive context takeover ends the occurrence immediately");
 namespace life=kcd2vr::inventory_lifecycle_t8;
 prepare();acquireR96();life::Publish(true,true);StagedPropTick596();
 needR96(!g_prop596.id&&velocityCalls==0&&stowCalls==0,"positive load discards old private world claim without old native mutation");
 life::Publish(true,false);freshR96(1040);acquireR96();freshR96(1060);frame.propRequested5916c1[0].p.x=.03;StagedPropTick596();
 needR96(g_prop596.id&&velocityCalls==1,"new save occurrence performs actual native drive");
 // A possibly partially applied native call is never retried. Recovery only
 // opens new admission after observed load completion, then requalifies body.
 prepare();acquireR96();actionOK=false;freshR96(1020);frame.propRequested5916c1[0].p.x=.03;StagedPropTick596();
 needR96(!g_prop596.id&&g_prop_fault5916c1.load()&&velocityCalls==1,"failed native call quarantines this load");
 actionOK=true;needR96(!BuildStagedPickup596(nativeController.data(),11,true,&p)&&velocityCalls==1,"clean-looking old body cannot replay the failed request");
 life::Publish(false,false);life::RequestRecovery();life::Recheck();
 needR96(g_prop_fault5916c1.load(),"unknown/menu/recheck is not positive load recovery");
 life::Publish(true,true);needR96(!BuildStagedPickup596(nativeController.data(),11,true,&p),"loading is not completed-load admission");
 life::Publish(true,false);freshR96(1040);acquireR96();freshR96(1060);frame.propRequested5916c1[0].p.x=.06;StagedPropTick596();
 needR96(g_prop596.id&&velocityCalls==2&&!g_prop_fault5916c1.load(),"post-load fresh native action succeeds without process reset");
 actionOK=false;freshR96(1080);frame.propRequested5916c1[0].p.x=.08;StagedPropTick596();
 needR96(!g_prop596.id&&g_prop_fault5916c1.load()&&velocityCalls==3,"later fault scopes to its own load");
 actionOK=true;life::Publish(true,true);life::Publish(true,false);freshR96(1100);acquireR96();freshR96(1120);frame.propRequested5916c1[0].p.x=.1;StagedPropTick596();
 needR96(g_prop596.id&&velocityCalls==4&&!g_prop_fault5916c1.load(),"second load recovery performs real servo with no production-state reset");
 std::printf("R96 prop actual build/consumer: %u checks PASS; native memory/physics leaves substituted\n",checksR96);
}
