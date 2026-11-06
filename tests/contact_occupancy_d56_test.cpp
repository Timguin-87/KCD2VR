// Exact generated production free-hand caller plus world query adapter. Native
// geometry is synthetic; UNKNOWN retains only the accepted restart origin.
#define KCD2VR_WORLD_ADAPTER_CALLER_ONLY_D56 1
#include "world_adapter_test.cpp"
#undef KCD2VR_WORLD_ADAPTER_CALLER_ONLY_D56
int main(){
 Init();SchedulingStateH24 state{};HandContact6695 c[2]{};V3 goals[2]{{.532f,0,0},{0,0,0}};
 SchedulingTrackingH24 t{};t.tick=1000;t.sequence=1;const bool valid[2]{true,false},unknown[2]{false,false},empty[2]{true,false};
 const unsigned yields[2]{16,0};const bool unclaimed[2]{};
 std::uint64_t previous=0;const auto ch=reinterpret_cast<void*>(character);
 {ContactPass6695 pass(true,1000,previous,1,7);g_contact_budget6695.remaining=0;V3 point{},offsets[5]{};
  ContactHand6695(c[0],ch,1,{0,0,0},goals[0],point,offsets);ContactCommit6695(c[0],ch,goals[0],1000);}
 c[0].planeCount=1;c[0].fingerContact[1]=true;c[0].lastCompleteTickH18=1000;
 {ContactPass6695 pass(true,1005,previous,2,7);const auto rays=mockOrigins55.size();
  SchedulingCallerH24(state,c,goals,t,true,valid,unknown,1,false,false,yields,unclaimed);
  Check(mockOrigins55.size()==rays&&!c[0].have&&!c[0].planeCount&&!c[0].fingerContact[1]&&!c[0].lastCompleteTickH18,
        "occupancy UNKNOWN caller issues no world query and retains no measured/finger authority");
  Check(c[0].presentationTickD53==1000&&Len(Sub(c[0].presentationWorldD53,{.532f,0,0}))<1e-6f,
        "UNKNOWN caller retains accepted wrist as a bounded later sweep origin only");}
 mockMode=4;wallPoint={.20f,0,0};wallNormal={-1,0,0};goals[0]={.53325f,0,0};t.tick=1011;t.sequence=3;
 {ContactPass6695 pass(true,1011,previous,3,7);
  SchedulingCallerH24(state,c,goals,t,true,valid,empty);
  Check(state.allowedReady[0]&&!state.handBlocked[0]&&goals[0].x>.53f,"EMPTY reacquisition cannot sweep from unrelated authored wrist and create a33cm false jump");
  Check(!mockOrigins55.empty()&&std::abs(mockOrigins55.front().x-.532f)<1e-6f,"production caller uses the preserved accepted wrist in its actual first ray");}
 const bool claimed[2]{true,false};t.tick=1015;t.sequence=4;
 {ContactPass6695 pass(true,1015,previous,4,7);SchedulingCallerH24(state,c,goals,t,true,valid,unknown,1,false,false,yields,claimed);
  Check(!c[0].presentationTickD53&&!c[0].character,"a different hand owner still retires the entire old contact including the restart origin");}
 std::printf("contact_occupancy_d56=%d/%d; native/headset=NOT_EXECUTED\n",assertions-failures,assertions);return failures?1:0;
}
