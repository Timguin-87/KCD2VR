#define KCD2VR_MORTAR69_NO_MAIN
#include "mortar_input69_test.cpp"
static void pair70(Fixture& q){
 q.in.controllerMortar70=true;q.in.nativeFrame.offers[14].valid=false;
 auto bounds=[](float x){kcd2vr::subsystems::alchemy::geometry68::Bounds b{};for(int j=0;j<3;++j){b.local[j]=-.01f;b.local[j+3]=.01f;}b.world[0]=b.world[5]=b.world[10]=1;b.world[3]=x;return b;};
 q.in.nativeFrame.offers[2]={{2,102},{0,0,0},q.in.now,true};q.in.nativeFrame.offers[2].boundsValid=true;q.in.nativeFrame.offers[2].bounds=bounds(0);
 q.in.nativeFrame.offers[6]={{6,106},{.52,0,0},q.in.now,true};q.in.nativeFrame.offers[6].boundsValid=true;q.in.nativeFrame.offers[6].bounds=bounds(.52f);
 q.in.palm[1].point={0,0,0};q.in.palm[0].point={2,0,0};q.grip(1,1);q.step();q.advance();q.selected(2);check(q.step().admitted&&q.state.mortarHome70.valid);
 q.grip(0,1);q.tool(0,3);q.tool(1,2);for(int h=0;h<2;++h){auto& t=q.in.tool[h];t.binding={1,2,888,std::uintptr_t(800+h),t.entity,h,t.type};t.binding.object=900+h;t.binding.lifetime=3;}
 q.step();q.advance(81);q.step();
}
int main(){
 Fixture q;pair70(q);q.advance();q.in.palm[1].point={.5,0,0};
 q.in.nativeFrame.offers[6].point={.52,0,0};q.selected(6);q.step();q.advance();q.grip(1,0);auto o=q.step();
 check(o.use&&!o.passenger&&!o.semiControl.manualUse&&o.control.held[0]&&o.control.held[1]);
 q.advance();q.in.action.parent=777;q.in.action.placeMortar69=true;
 for(auto& t:q.in.tool){t.parent=777;t.binding.action=777;}
 q.in.work={1,777,888,3,q.in.now,66,s::MortarPlace70,0,3,true,false};
 q.grip(1,1);q.f.hand[1].rotation[0]=std::sin(.3f);q.f.hand[1].rotation[3]=std::cos(.3f);
 o=q.step();check(!o.passenger&&!o.semiControl.manualUse&&o.semiControl.totalSeconds==0);
 q.advance(20);o=q.step();check(o.semiControl.totalSeconds>.019&&o.gesture.status==kcd2vr::subsystems::alchemy::gesture67::Status::Pouring);
 auto before=o.semiControl.totalSeconds;o=q.step();check(o.semiControl.totalSeconds==before); // no duplicate sample credit
 q.advance(20);q.f.hand[1].rotation[0]=0;q.f.hand[1].rotation[3]=1;o=q.step();check(o.semiControl.totalSeconds==before); // upright pauses
 q.advance(20);q.in.work.phase=1;o=q.step();check(o.semiControl.totalSeconds==before); // real Use happened; cannot pour twice
 q.advance(20);q.in.palm[1].point={0,0,0};o=q.step();check(o.semiControl.totalSeconds>before);
 q.advance(20);q.in.work.phase=2;o=q.step();check(o.semiControl.manualUse); // actual Detach -> native cleanup
 q.advance();q.in.action.parent=0;q.in.work={};q.in.nativeFrame.table.handTypesKnown68=true;q.in.nativeFrame.table.handType68={21,21};
 o=q.step();check(o.mortarPlaced69&&!o.use);
 // Lost/mismatched native binding after Use retires physical ownership once,
 // without letting a fresh home pose advance a tool we no longer own.
 for(int fault=0;fault<2;++fault){Fixture lost;pair70(lost);lost.advance();lost.in.palm[1].point={.5,0,0};lost.selected(6);lost.step();lost.advance();lost.grip(1,0);lost.step();
  lost.advance();lost.grip(1,1);lost.in.action.parent=777;lost.in.action.placeMortar69=true;
  for(auto& t:lost.in.tool){t.parent=777;t.binding.action=777;}
  lost.in.work={1,777,888,3,lost.in.now,66,s::MortarPlace70,1,3,true,false};lost.in.palm[1].point={0,0,0};lost.step();lost.advance(20);
  if(fault==0)lost.in.tool[0].joined=false;else lost.in.tool[1].binding.object=12345;
  auto refused=lost.step();check(!refused.control.controllerMortar70&&!refused.control.held[0]&&!refused.control.held[1]);
  check((!refused.control.enabled||refused.semiControl.manualUse)&&!refused.use); // revoked input or explicit native-only cleanup
 }
 // D72: the release that committed this exact native Place owns its pour
 // through the Use event. It must not require a second grip press; recipient,
 // pair, native parent, orientation and once-per-source credit still apply.
 {Fixture released;pair70(released);released.advance();released.in.palm[1].point={.5,0,0};
  released.selected(6);released.step();released.advance();released.grip(1,0);released.step();
  released.advance();released.in.action.parent=777;released.in.action.placeMortar69=true;
  for(auto& t:released.in.tool){t.parent=777;t.binding.action=777;}
  released.in.work={1,777,888,3,released.in.now,66,s::MortarPlace70,0,3,true,false};
  released.f.hand[1].rotation[0]=std::sin(.3f);released.f.hand[1].rotation[3]=std::cos(.3f);
  released.step();released.advance(20);auto flowing=released.step();
  check(flowing.semiControl.totalSeconds>.019&&flowing.gesture.status==kcd2vr::subsystems::alchemy::gesture67::Status::Pouring);
  const auto earned=flowing.semiControl.totalSeconds;
  check(released.step().semiControl.totalSeconds==earned);
  released.advance(20);released.in.palm[1].point={2,0,0};
  check(released.step().semiControl.totalSeconds==earned); // leaving exact destination pauses
  released.advance(20);released.in.palm[1].point={.5,0,0};released.in.tool[1].binding.object++;
  check(!released.step().control.controllerMortar70); // replacement cannot borrow release intent
 }
 // The saved resting target follows the current native pot frame, not an old world pose.
 kcd2vr::subsystems::alchemy::pour70::Home home;
 auto pot=q.in.nativeFrame.offers[6].bounds,source=pot;source.world[3]=0;
 check(home.capture(106,102,pot,source));pot.world[3]+=1;
 check(home.atRest70(106,102,pot,{1,0,0})&&!home.atRest70(106,102,pot,{0,0,0})&&!home.atRest70(999,102,pot,{1,0,0}));
 pot.world[3]=.52f;pot.world[0]=pot.world[5]=0;pot.world[1]=-1;pot.world[4]=1;
 check(home.atRest70(106,102,pot,{.52,-.52,0}));
 // Recenter/source changes cannot carry earned animation work into another pose.
 kcd2vr::subsystems::alchemy::pour70::Controller c;
 q.in.work={1,777,888,3,q.in.now,66,s::MortarPlace70,0,3,true,false};c.step(q.in.work,q.f,true);q.advance(20);check(c.step(q.in.work,q.f,true)>.019);
 q.advance();++q.f.sourceEpoch;check(c.step(q.in.work,q.f,true)==0);
 // An81ms same-operation gap cannot reset the cumulative counter beneath
 // the native lease's already-consumed seconds and stall resumed valid work.
 c={};q.advance(20);c.step(q.in.work,q.f,true);
 for(int i=0;i<60;++i){q.advance(20);c.step(q.in.work,q.f,true);}const auto total=c.seconds;check(total>1.1);
 q.advance(81);check(c.step(q.in.work,q.f,true)==total);q.advance(20);check(c.step(q.in.work,q.f,true)>total+.019);
 std::printf("controller mortar70: %u checks PASS\n",checks);return 0;
}
