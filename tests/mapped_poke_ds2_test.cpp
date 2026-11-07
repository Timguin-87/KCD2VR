#define DS1_TEST_MAIN priorDs1Checks
#include "living_touch_ds1_test.cpp"
static void pokeFixture(){lightFixture();lab::impactLeaseOk=true;}
int main(){
 if(priorDs1Checks())return 1;
 pokeFixture();publish();fp::Dispatch();
 check(impactCalls==1&&capsuleCalls==0&&impactJoint==5,"actual mapped geometry submits gentle native skeleton poke, never capsule-part-as-joint");
 check(std::fabs(impactImpulse[2]+.05f)<1e-6,"mapped poke uses bounded half-light impulse in actual AddImpact packet");
 fp::Dispatch();check(impactCalls==1,"same original poke sample never replays");
 for(int kind=0;kind<17;++kind){pokeFixture();auto e=publish();
  switch(kind){
   case 0:lab::impactLeaseOk=false;break;
   case 1:e.grip=.9f;break;case 2:e.fist=true;break;
   case 3:e.sample.tick=now-81;break;case 4:e.sample.request=999;break;
   case 5:e.sample.motion.incoming=.1;break;
   case 6:put(0xB000,base+0x3A48B30);break;
   case 7:put(0xB020,Address(0xDEAD));break;
   case 8:put(joints+5*0x30+0x20,Address(0));break;
   case 9:currentGeometry=currentProxyGeometry=0xDEAD;break;
   case 10:put(character+0x7B4,static_cast<unsigned char>(0x20));break;
   case 11:e.sample.worldCoherent=false;break;
   case 12:lab::rung=2;break;case 13:fp::g_ctx_menu=true;break;
   case 14:fp::g_last_player_entity=reinterpret_cast<void*>(entity);break;
   case 15:mutate=1;break; // root changes during the final current-character read
   case 16:mutate=2;break; // current character changes after mapping
  }
  send(e);fp::Dispatch();check(impactCalls==0&&capsuleCalls==0,"unproven current living joint/lease/grip/pose/context cannot poke");
  fp::g_last_player_entity=reinterpret_cast<void*>(0x7770);
 }
 pokeFixture();publish();impactResult=0;fp::Dispatch();fp::Dispatch();check(impactCalls==1&&fp::touch43::g_calls==0,"native poke refusal authoritative, no replay");
 pokeFixture();publish();impactResult=-1;fp::Dispatch();fp::Dispatch();check(impactCalls==1&&fp::touch43::g_latched,"native poke fault latches once");
 std::printf("mapped_poke_ds2: %u/%u assertions passed (prior suites included)\n",checks-failed,checks);return failed?1:0;
}
