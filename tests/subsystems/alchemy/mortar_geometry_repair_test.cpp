#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
#include <cmath>
static unsigned failuresRepair=0,checksRepair=0;
static void expectRepair(bool ok,const char* message){++checksRepair;if(!ok){++failuresRepair;std::fprintf(stderr,"FAIL: %s\n",message);}}
static void boxesRepair(Fixture& q){
 for(int slot:{2,3}){auto& o=q.in.nativeFrame.offers[slot];o={{slot,unsigned(100+slot)},{},q.in.now,true};o.boundsValid=true;
  auto& b=o.bounds;b.world[0]=b.world[5]=b.world[10]=1;
  if(slot==2){b.local[0]=b.local[1]=-.08f;b.local[2]=-.04f;b.local[3]=b.local[4]=.08f;b.local[5]=.04f;}
  else {b.local[0]=b.local[1]=-.02f;b.local[3]=b.local[4]=.02f;b.local[5]=.20f;b.world[11]=.03f;}}
}
static void nativePairRepair(Fixture& q){
 q.in.proximityCraft1=true;q.admit(2,1);q.grip(0,1);q.tool(0,3);q.tool(1,2);
 for(int h=0;h<2;++h){auto& t=q.in.tool[h];t.parent=999;
  t.binding={1,2,888,std::uintptr_t(800+h),t.entity,h,t.type};t.binding.object=900+h;t.binding.lifetime=3;}
 q.in.action.parent=999;q.in.work={1,999,555,3,q.in.now,44,s::Mortar,3,3,true,false};
 q.f.hand[0].position[1]=.25f;q.in.world[0].point[0]=0;q.in.world[0].point[2]=.25f;
 q.in.palm[0].point={0,0,.25};boxesRepair(q);q.step();
 expectRepair(q.state.mortarPair69,"current paired native attachments remain joined while nomination hides carried tools");
}
int main(){
 for(int fault=0;fault<4;++fault){
  Fixture q;q.in.proximityCraft1=true;q.admit(2,1);q.advance(16);q.grip(0,fault==1?.45f:.8f);q.step();
  if(fault==2){q.advance(16);q.grip(0,.2f);q.grip(1,.2f);q.step();}
  if(fault==3){q.advance(16);q.in.palm[0].valid=q.f.hand[0].valid=false;q.step();q.in.palm[0].valid=q.f.hand[0].valid=true;}
  q.advance(16);q.grip(0,.45f);q.grip(1,.45f);q.tool(0,3);q.tool(1,2);
  for(int h=0;h<2;++h){auto& t=q.in.tool[h];t.parent=999;
   t.binding={1,2,888,std::uintptr_t(800+h),t.entity,h,t.type};t.binding.object=900+h;t.binding.lifetime=3;}
  q.in.action.parent=999;q.in.work={1,999,555,3,q.in.now,44,s::Mortar,3,3,true,false};boxesRepair(q);
  auto o=q.step();expectRepair(o.control.held[0]==(fault==0)&&o.control.held[1]==(fault==0),
   "late mortar pair Attach retains both actual grip edges; unacquired, released or tracking-lost grip cannot borrow them");
  q.advance(16);q.f.hand[0].position[0]=.01f;q.in.nativeFrame.offers[3].bounds.world[3]=.01f;o=q.step();
  expectRepair((o.gesture.totalSeconds>0)==(fault==0),"same retained .45 mortar grips can grind without resqueeze; invalid retention grants no credit");
 }
 {Fixture q;nativePairRepair(q);q.advance(16);q.f.hand[0].position[0]=.01f;
  q.in.palm[0].point.x=q.in.world[0].point[0]=.01f;q.in.nativeFrame.offers[3].bounds.world[3]=.01f;
  auto o=q.step();expectRepair(o.gesture.totalSeconds>0&&o.gesture.status==g::Status::Grinding,
    "pestle contacting the mortar grinds with controller wrists 25cm apart");
  const auto credit=o.gesture.totalSeconds;o=q.step();expectRepair(o.gesture.totalSeconds==credit,"second-eye duplicate earns no grinding credit");
  q.advance(16);q.f.hand[0].position[0]=.02f;q.in.nativeFrame.offers[3].bounds.world[3]=1.f;
  o=q.step();expectRepair(o.gesture.totalSeconds==credit&&o.gesture.status==g::Status::WorkStopped,
    "a distant actual pestle cannot grind merely because its wrist moves");
 }
 {Fixture q;nativePairRepair(q);q.advance(16);q.f.hand[0].position[0]=q.f.hand[1].position[0]=.02f;
  q.in.nativeFrame.offers[2].bounds.world[3]=q.in.nativeFrame.offers[3].bounds.world[3]=.02f;
  const auto o=q.step();expectRepair(o.gesture.totalSeconds==0,"moving both tools together does not grind");}
 for(int fault=0;fault<4;++fault){Fixture q;nativePairRepair(q);q.advance(16);q.f.hand[0].position[0]=.02f;
  switch(fault){case 0:q.in.nativeFrame.offers[3].boundsValid=false;break;
   case 1:q.in.nativeFrame.offers[3].tick=q.in.now-81;break;
   case 2:q.in.nativeFrame.offers[3].ref.entity++;break;
   case 3:q.in.nativeFrame.offers[2].bounds.world[0]=0;break;}
  const auto o=q.step();expectRepair(o.gesture.totalSeconds==0&&o.gesture.status==g::Status::GeometryRequired,
   "missing, stale, foreign or singular tool bounds cannot borrow current grinding authority");}
 std::printf("mortar_geometry_repair: %u checks, %u failures\n",checksRepair,failuresRepair);return failuresRepair?1:0;
}
