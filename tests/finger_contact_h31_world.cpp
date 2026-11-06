// Actual production ContactHand extraction, same native leaves as world_adapter.
#define main retained_world_adapter_main_h31
#include "world_adapter_test.cpp"
#undef main
extern "C" bool FingerWorldMeasureH31(void* state,unsigned stateBytes,const void* geometry,unsigned geometryBytes,
    unsigned long long tick,unsigned long long sample,float scale,int mode,int budget,int* calls) {
 static std::uint64_t previous=0;
 if(!sample){Init();previous=0;return true;}
 if(stateBytes!=sizeof(HandContact6695)||geometryBytes!=sizeof(FingerContactGeometry103))return false;
 HandContact6695 staged{};FingerContactGeometry103 input{};std::memcpy(&staged,state,sizeof(staged));std::memcpy(&input,geometry,sizeof(input));
 const float transform[]{0,0,0,1,0,0,0,scale};Store(character+0xA08,transform);
 mockMode=mode;wallPoint={.12f*scale,0,0};wallNormal={-1,0,0};
 V3 palm[6]{};for(int f=0;f<5;++f){const auto p=kcd2vr::pose_core::finger_clearance103(input.curve[f],input.input[f],
   {input.orientation.x,input.orientation.y,input.orientation.z,input.orientation.w},{},{{0,0,-2},{0,0,1}},0);
  palm[f+1]={float(p.tip.x),float(p.tip.y),float(p.tip.z)};}
 // Same actual root/palm point as the fixture's five identical root x offsets.
 palm[0]={.05f,0,.0025f};
 V3 allowed{};const auto before=mockCalls;bool ok=false;
 {ContactPass6695 pass(true,tick,previous,sample);if(budget>=0)g_contact_budget6695.remaining=unsigned(budget);
  ok=ContactHand6695(staged,reinterpret_cast<void*>(character),1,{-.10f,0,0},{0,0,0},allowed,palm,&input);}
 std::memcpy(state,&staged,sizeof(staged));if(calls)*calls=mockCalls-before;return ok;
}
