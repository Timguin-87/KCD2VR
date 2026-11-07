#define KCD2VR_WORLD_ADAPTER_CALLER_ONLY_D56 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_CALLER_ONLY_D56
static void seed(HandContact6695& c,int side){
 c.character=reinterpret_cast<void*>(character);c.epoch=1;c.have=c.rawFeatureReady55=c.blocked=true;
 c.featureCount=c.planeCount=7;c.lastCompleteTickH18=990;c.worldAllowed={0,side?.2f:-.2f,0};c.releaseRawWorldD45=c.worldAllowed;
 for(int n=0;n<7;++n){c.offset[n]=n<6?schedulingOffsetsH24[n]:V3{.1f,.12f,0};c.rawFeatureWorld55[n]=Add(c.worldAllowed,c.offset[n]);
  c.planes[n].feature=n;c.planes[n].native172.mask=npc172::StaticTerrain;c.planes[n].normal={-1,0,0};c.planes[n].point={.2f,0,0};}
}
int main(){
 for(bool on:{false,true}){Init();kcd2vr::held_motion_d67::setContactFairness(on);mockBodies172=true;mockMode=0;
  schedulingGeometryH24={};schedulingGeometryH24.orientation={0,0,0,1};
  for(int n=0;n<6;++n)schedulingOffsetsH24[n]={.1f,.02f*n,0};
  for(int n=1;n<5;++n){auto& c=schedulingGeometryH24.curve[n];c.count=3;c.offset[0]={.1,.12,0};schedulingGeometryH24.input[n]=1;}
  HandContact6695 contacts[2]{};seed(contacts[0],0);seed(contacts[1],1);
  SchedulingStateH24 state{};SchedulingTrackingH24 tracking{};const bool valid[2]{true,true},free[2]{true,true};
  std::uint64_t previous=0;unsigned complete[2]{},calls=0;
  for(int pass=0;pass<2;++pass){tracking.tick=1000+11*pass;tracking.sequence=1+pass;V3 goals[2]{{.01f,-.2f,0},{.01f,.2f,0}};
   {ContactPass6695 work(true,tracking.tick,previous,tracking.sequence);SchedulingReadyH24();SchedulingCallerH24(state,contacts,goals,tracking,true,valid,free);
    Check(g_contact_budget6695.used<=18&&g_contact_budget6695.spent<=1000,"caller retains shared18ray/1ms cap");calls+=g_contact_budget6695.used;}
   for(int hand=0;hand<2;++hand)complete[hand]+=contacts[hand].fresh169r1;
   if(pass==0)Check(on?(contacts[0].fresh169r1&&!contacts[1].fresh169r1):(!contacts[0].fresh169r1&&!contacts[1].fresh169r1),
      "actual current caller repairs both-hands-incomplete recovery starvation");
  }
  Check(on?(complete[0]>0&&complete[1]>0):(!complete[0]&&!complete[1]),"oldest completion grants both hands progress overtwo samples");
  std::printf("FAIRNESS on%d completed%u,%u queries%u\n",on,complete[0],complete[1],calls);
 }
 std::printf("D67 contact fairness %d/%d\n",assertions-failures,assertions);return failures?1:0;
}
