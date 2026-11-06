// Production lookup and State::step; only mailbox publications are synthetic.
#define main retained_gesture_transfer_main
#include "gesture_transfer_d59_test.cpp"
#undef main
#include <array>

namespace selection_native=kcd2vr::alchemy_input19c4;

// This include is extracted verbatim from semi_workstations46.h by the runner.
// It keeps the boundary that originally mixed two different snapshots covered.
static void productionAdapter(Fixture& q){
 namespace alchemy_input19c4=kcd2vr::alchemy_input19c4;
 auto& in=q.in;const auto now=in.now;
 struct {std::uintptr_t alchemy;std::uint64_t lifetime;} c{in.context.instance,in.context.lifetime};
#include D59R1_SELECTION_ADAPTER
}
static void stableAndInterleaved(){
 using namespace selection_native;
 for(int fault=0;fault<13;++fault){
  Selection first{1,3,1110,106,6},second=first;Observation hint{1,3,1109,106,6,false};
  bool hintOk=true,firstOk=true,secondOk=true;
  switch(fault){
   case 1:second={1,3,1120,102,2};break; // different recipient supplied new timestamp
   case 2:second.entity=0;second.type=21;break;
   case 3:++second.type;break;
   case 4:++second.lifetime;break;
   case 5:++second.instance;break;
   case 6:++second.tick;break; // even same target must be one completed observation
   case 7:hint.type=21;break; // unknown native hint, independent of action input
   case 8:hintOk=false;break;
   case 9:hint.tick=1000;break;
   case 10:first.tick=1121;break;
   case 11:firstOk=false;break;
   case 12:secondOk=false;break;
  }
  Observation out{9,9,9,9,9,true};Selection stamp{9,9,9,9,9};unsigned reads=0;
  const bool got=lookupSelectedJoinedD59(1,3,1120,out,stamp,
   [&](Selection& value){value=reads++?second:first;return reads==1?firstOk:secondOk;},
   [&](std::uint32_t entity,Observation& value){check(entity==106);value=hint;return hintOk;});
  check(got==(fault==0));
  if(got){check(stamp.tick==1110&&out.tick==1109&&stamp.entity==out.entity&&stamp.type==out.type);}
  else check(!stamp.tick&&!stamp.entity&&!out.tick&&!out.entity);
 }
 // D63: +304 is the native action input, not the hover entity type. Idle
 // 21 and an earlier action type must not erase a coherent current recipient.
 for(unsigned actionType:{21u,19u,2u}){
  Selection current{1,3,1110,106,actionType};Observation out{};Selection stamp{};
  check(lookupSelectedJoinedD59(1,3,1120,out,stamp,
   [&](Selection& value){value=current;return true;},
   [&](std::uint32_t,Observation& value){value={1,3,1110,106,6,false};return true;}));
  check(out.type==6&&out.entity==106&&stamp.type==actionType&&stamp.tick==1110);
 }
}
static void actualMailboxAndTransfer(){
 namespace native=selection_native;
 Fixture q;herbD59(q);q.grip(0,0);q.step();q.advance();nativeIdleD59(q);
 const auto intent=q.state.ordinaryIntentTickD59;
 native::clear();
 native::observed[6].publish({1,3,q.in.now,106,6,false}); // newer hint is not a new selection
 native::selected.publish({1,3,intent-1,106,6});
 productionAdapter(q);
 check(q.in.nativeSelectedKnown&&q.in.nativeSelected.tick==q.in.now);
 check(q.in.nativeSelectionTick==intent-1&&!q.step().use);

 // The original broken sequence borrowed the mortar's timestamp for this pot.
 // The joined helper detects the same allowed publication DURING its query.
 native::Observation out{};native::Selection stamp{};unsigned calls=0;
 const bool got=native::lookupSelectedJoinedD59(1,3,q.in.now,out,stamp,
  [&](native::Selection& value){++calls;return native::selected.read(value);},
  [&](std::uint32_t entity,native::Observation& value){
   const bool ok=native::lookup(1,3,entity,q.in.now,value);
   native::selected.publish({1,3,q.in.now,102,2});return ok;
  });
 check(!got&&calls==2&&!out.entity&&!stamp.tick);
 q.in.nativeSelectedKnown=got;q.in.nativeSelected=out;q.in.nativeSelectionTick=stamp.tick;
 check(!q.step().use&&q.state.ordinaryReleasePending);

 // Actual adapter sees the different currently selected recipient. A hint for
 // that object cannot retarget an already retained physical release.
 native::observed[2].publish({1,3,q.in.now,102,2,false});
 productionAdapter(q);check(q.in.nativeSelectedKnown&&q.in.nativeSelected.entity==102);
 check(q.in.nativeSelectionTick==q.in.now&&!q.step().use);

 // A later genuine selection of the original pot permits one normal request.
 q.advance();native::observed[6].publish({1,3,q.in.now,106,6,false});
 native::selected.publish({1,3,q.in.now,106,6});productionAdapter(q);
 auto result=q.step();check(result.use&&result.transferStateD59==4&&result.ordinaryReturnRequested);
 result=q.step();check(!result.ordinaryReturnRequested);
 q.advance(81);result=q.step();check(!result.use&&!result.ordinaryReturnRequested);

 // Failed query clears outputs from a preceding successful lookup.
 native::selected.publish({1,3,q.in.now,0,21});productionAdapter(q);
 check(!q.in.nativeSelectedKnown&&!q.in.nativeSelected.entity);
 native::clear();
 std::printf("D59R1 selection: release=%llu retained old selection waits; interleaved recipient refused; genuine post-release recipient emits once\n",
  (unsigned long long)intent);
}
static void typeOnlyStamp(){
 using namespace selection_native;
 for(int fault=0;fault<8;++fault){
  Selection first{1,3,1000,0,18},second=first,out{9,9,9,9,9};unsigned reads=0;
  switch(fault){case 1:++second.instance;break;case 2:++second.lifetime;break;
   case 3:++second.tick;break;case 4:++second.type;break;case 5:second.entity=118;break;
   case 6:first.tick=920;break;case 7:first.tick=1011;break;}
  const bool got=selectedTypeOnlyJoinedD59(1,3,1010,out,
   [&](Selection& value){value=reads++?second:first;return true;});
  check(got==(fault==0));
  if(got)check(out.tick==1000&&out.type==18&&!out.entity);
  else check(!out.tick&&out.type==21&&!out.instance);
 }
 // Real mailbox publication between the two reads: identical type/tick is
 // insufficient when a different station lifetime has become current.
 selected.publish({1,3,1000,0,18});Selection stamp{};unsigned reads=0;
 check(!selectedTypeOnlyJoinedD59(1,3,1010,stamp,[&](Selection& value){
  const bool got=selected.read(value);if(++reads==1)selected.publish({1,4,1000,0,18});return got;}));
 check(!stamp.tick&&reads==2);

 // Actual adapter retains the native timestamp and the current table-member
 // fallback still admits mounted bellows through production State::step.
 Fixture q;q.in.nativeFrame.offers[14].valid=false;
 q.in.nativeFrame.offers[18]={{18,118},{0,0,0},q.in.now,true};
 q.grip(1,1);q.step();q.advance();selected.publish({1,3,q.in.now-2,0,18});productionAdapter(q);
 check(!q.in.nativeSelectedKnown&&q.in.nativeSelectedTypeOnly==18&&q.in.nativeTypeOnlyTick==q.in.now-2);
 const auto result=q.step();check(result.admitted&&result.use&&q.state.admittedRef.slot==18);
 selected.publish({1,4,q.in.now-2,0,18});productionAdapter(q);
 check(q.in.nativeSelectedTypeOnly==21&&!q.in.nativeTypeOnlyTick);
 clear();
}
int main(){stableAndInterleaved();actualMailboxAndTransfer();typeOnlyStamp();
 std::printf("D59R1 coherent alchemy selection: %u checks PASS\n",checks);}
