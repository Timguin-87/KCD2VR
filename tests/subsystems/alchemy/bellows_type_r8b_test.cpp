// R8b: the mounted bellows is natively selected by TYPE 18 with entity 0 (C37 selectedType=18 selectedEntity=0).
// the hand's bellows nomination must admit against that, and only for type 18.
#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
int main(){
 auto setup=[](Fixture& z){z.in.nativeFrame.offers[14].valid=false;z.in.nativeFrame.offers[18]={{18,118},{0,0,0},z.in.now,true};
  z.in.palm[1].point={0,0,0};z.in.palm[0].point={2,0,0};z.grip(1,1);z.step();z.advance();};
 {Fixture z;setup(z);auto o=z.step();check(!o.admitted&&o.nomination.decision.ref.entity==118);
  z.in.nativeSelectedTypeOnly=18;z.in.nativeTypeOnlyTick=z.in.now;o=z.step();
  check(o.admitted&&o.control.entity==118&&o.control.type==18);std::printf("PASS type-only 18 admits the hand's bellows\n");}
 {Fixture z;setup(z);z.in.nativeSelectedTypeOnly=17;z.in.nativeTypeOnlyTick=z.in.now;auto o=z.step();
  check(!o.admitted);std::printf("PASS type-only other than 18 does not admit\n");}
 {Fixture z;setup(z);z.in.nativeSelectedTypeOnly=18;z.in.nativeTypeOnlyTick=z.in.now-500;auto o=z.step();
  check(!o.admitted);std::printf("PASS stale type-only does not admit\n");}
 {Fixture z;setup(z);z.in.nativeFrame.table.refs[18]={18,0};z.in.nativeSelectedTypeOnly=18;z.in.nativeTypeOnlyTick=z.in.now;auto o=z.step();
  check(!o.admitted);std::printf("PASS no bellows member in the table: no admission\n");}
 std::printf("bellows_type_r8b %u checks ok\n",checks);return 0;}
