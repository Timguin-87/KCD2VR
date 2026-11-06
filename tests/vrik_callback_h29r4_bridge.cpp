// Complete production frik TU plus verbatim natural callback decision/original
// call slices. Native original and callback-local inputs are explicit leaves.
#include "../src/frik_pose_probe.cpp"
#include <cstdio>
#include <vector>
static std::vector<std::string> outputLines23;
namespace kcd2vr::diagnostic_trace {void Write(const char* line) noexcept {if(line)outputLines23.emplace_back(line);}}
// This ordinary recorder fixture must never query/grant an XR34 working hand.
// Keep the required native-read leaf explicit and detect accidental consumption.
namespace kcd2vr::animation_filter {extern unsigned reloadPassengerEligibilityCallsXR34;}
using namespace kcd2vr::frik_pose_probe;
namespace diagnostics=kcd2vr::diagnostics;
namespace vd4=kcd2vr::diagnostics::vrik_h29;
namespace cap4=kcd2vr::diagnostics::capture168;
static unsigned checks4=0,failures4=0,original4=0;
static void check4(bool b,const char* p){++checks4;if(!b){++failures4;std::printf("FAIL %s\n",p);}}
static void __fastcall originalLeaf4(void*){++original4;}
static std::vector<vd4::Record> records4(){std::vector<vd4::Record> out;vd4::Record r{};while(vd4::pop(r))out.push_back(r);return out;}
static void callback4(unsigned negative,const TrackingSnapshot& source,bool original=true){
 CoherentTransaction6689 transaction;bool committed=false;std::optional<KickOutputProbe23> kickOutput23;
 std::optional<kcd2vr::diagnostics::hands28::Record> fingerOutput28;
 // This recorder fixture exercises ordinary native-body exclusion. Actual XR34
 // registration/working-hand ownership is covered by its composed native tests.
 const bool reloadWorkingHandXR34=false;
 bool nativeBody=negative==0,rigOk=negative!=4,rawOk=negative!=5,haveSnapshot=negative!=6;
 const int f12=negative==2?2:0,census=negative==3?0:3;
 void* pose=nullptr;void* owner=nullptr;void* skinChar=reinterpret_cast<void*>(0x12345000);void* skinAnim=nullptr;void* desc=nullptr;
 std::uint32_t count=0;RigMap rig{};PoseGetFn b1fn=nullptr;
 auto poseSnapshot=source;const auto validationTick=hostTickCount64;
 if(negative==7)poseSnapshot.tick=validationTick-101;
 g_skinning_core_original=original?&originalLeaf4:nullptr;
 #include <vrik_callback_decision_h29r4.inc>
 #include <vrik_callback_original_h29r4.inc>
 transaction.Restore();
 if(committed)PublishCommitted6689(transaction);
}
static unsigned poseReads23=0,poseWrites23=0;
static bool unreadablePose23=false;
static std::array<QuatT28,20> poseValues23{};
static const QuatT28* __fastcall poseGetter23(void*,int id){++poseReads23;
 if(unreadablePose23&&id==4)return reinterpret_cast<const QuatT28*>(UINTPTR_MAX-15);
 return &poseValues23.at(id);}
static void __fastcall poseSetter23(void*,int id,const QuatT28* q){++poseWrites23;poseValues23.at(id)=*q;}
static void __fastcall outputOriginal23(void* desc){
 ++original4;void* out=nullptr;std::memcpy(&out,static_cast<unsigned char*>(desc)+0x20,8);
 for(unsigned j=0;j<poseValues23.size();++j){poseValues23[j].t[2]+=3;
  std::memset(static_cast<unsigned char*>(out)+j*32,int(j+1),32);}
}
static void setupOutput23(void* owner,void* ci){
 auto& s=g_pose6689;s.ready=true;s.owner=owner;s.character=ci;s.count=20;s.rig.pelvis=2;
 s.legsReady128=true;s.leg[0]={3,4,5,.4,.4,true};s.leg[1]={6,7,8,.4,.4,true};
 for(unsigned j=0;j<poseValues23.size();++j){poseValues23[j]={};poseValues23[j].q[0]=1;
  poseValues23[j].t[0]=float(j);poseValues23[j].t[1]=float(j)+.1f;poseValues23[j].t[2]=float(j)+.2f;}
}
static std::optional<KickOutputProbe23> outputCallback23(bool original=true,unsigned invalidation=0){
 std::array<unsigned char,64> descriptor{};static std::array<unsigned char,20*32> palette{};
 void* pose=poseValues23.data();void* owner=reinterpret_cast<void*>(0x12340000);
 void* skinChar=reinterpret_cast<void*>(0x12345000);void* skinAnim=reinterpret_cast<void*>(0x12346000);
 palette.fill(0);void* output=palette.data();void* desc=descriptor.data();void* ci8=static_cast<unsigned char*>(skinChar)+8;
 std::memcpy(descriptor.data(),&pose,8);std::memcpy(descriptor.data()+8,&owner,8);
 std::memcpy(descriptor.data()+0x10,&ci8,8);std::memcpy(descriptor.data()+0x20,&output,8);
 RigMap rig{};rig.pelvis=2;PoseGetFn b1fn=&poseGetter23;
 CoherentTransaction6689 transaction;bool committed=true;
 std::optional<kcd2vr::diagnostics::hands28::Record> fingerOutput28;
 const bool rawOk=true;const std::uint32_t count=20;const auto validationTick=std::uint64_t(hostTickCount64);
 #include <kick_output_begin23.inc>
 transaction.pose=pose;transaction.setter=&poseSetter23;transaction.attempted[4]=true;
 g_pose6689.original[4]=poseValues23[4];
 // Controlled solve leaf changes native-input leg transforms before actual core slice.
 for(auto& q:poseValues23)q.t[1]+=2;
 if(invalidation==1)++g_pose_epoch6689;
 if(invalidation==2)cap4::gate.select(cap4::Domain::Combat,false,hostTickCount64);
 if(invalidation==3){void* different=reinterpret_cast<void*>(0x400000);std::memcpy(descriptor.data()+0x20,&different,8);}
 g_skinning_core_original=original?&outputOriginal23:nullptr;
 if(invalidation==3)g_skinning_core_original=nullptr; // changed pointer is observed, never dereferenced by controlled native leaf
 #include <kick_output_phases23.inc>
 transaction.Restore();
 return kickOutput23;
}
static void outputChecks23(){
 hostTickCount64=30000;g_pose_epoch6689=21;g_last_player_pose=reinterpret_cast<void*>(0x12347000);
 const auto owner=reinterpret_cast<void*>(0x12340000),ci=reinterpret_cast<void*>(0x12345000);
 setupOutput23(owner,ci);kcd2vr::animation_filter::nativeKickActive175=true;
 cap4::gate.select(cap4::Domain::Combat,false,hostTickCount64);
 cap4::gate.select(cap4::Domain::Interaction,true,hostTickCount64);
 cap4::gate.select(cap4::Domain::Vrik,true,hostTickCount64);
 outputLines23.clear();poseReads23=0;const auto calls=original4;auto off=outputCallback23();
 check4(!bool(off)&&poseReads23==0&&outputLines23.empty()&&original4==calls+1,
  "F3 OFF/F5+F7 ON constructs no contained probe, adds zero joint reads/formatting and forwards original once");
 cap4::gate.select(cap4::Domain::Combat,true,hostTickCount64);setupOutput23(owner,ci);
 b168::SpecialRequest req{139,hostTickCount64-50,17,21,0x12348000,0x12349000,0x1234A000};g_special_request168.publish(req);
 outputLines23.clear();poseReads23=0;const auto prior=original4;auto on=outputCallback23();
 check4(bool(on)&&on->mapKnown&&poseReads23==21&&original4==prior+1&&on->called&&on->returned,
  "actual extracted callback samples seven derived joints in three phases with one original");
 check4(on->jointMask[0]==127&&on->jointMask[1]==127&&on->jointMask[2]==127&&
  on->joints[0][2].t[1]+2==on->joints[1][2].t[1]&&on->joints[1][2].t[2]+3==on->joints[2][2].t[2],
  "native input/controlled solve/native core changes are distinguished at real phase positions");
 check4(poseValues23[4].t[1]==on->joints[0][2].t[1]&&poseValues23[4].t[2]==on->joints[0][2].t[2],
  "actual original samples and diagnostic emission precede existing transaction restoration");
 check4(on->outputMask[0]==127&&on->outputMask[1]==127&&on->descriptorSame[0]&&on->descriptorSame[1]&&
  on->outputHash[0][2]!=on->outputHash[1][2],"same native output pointer hashes bounded raw32 bytes before/after core without decode");
 check4(outputLines23.size()==8&&outputLines23[0].find("lastR3Serial=139")!=std::string::npos&&
  outputLines23[0].find("requestAssociation=UNPROVEN")!=std::string::npos&&
  outputLines23[0].find("meshVisibility=UNOBSERVED")!=std::string::npos,
  "joined bounded records expose original request without inventing request/clip/mesh attribution or truncating");
 for(auto delta:{1u,498u}){hostTickCount64+=delta;setupOutput23(owner,ci);poseReads23=0;outputLines23.clear();auto r=outputCallback23();
  check4(!bool(r)&&poseReads23==0&&outputLines23.empty(),"same F3 session suppresses all extra reads inside 500ms");}
 ++hostTickCount64;setupOutput23(owner,ci);poseReads23=0;auto next=outputCallback23();
 check4(bool(next)&&poseReads23==21,"500ms boundary permits next finite sample");
 hostTickCount64+=500;setupOutput23(owner,ci);outputLines23.clear();auto absent=outputCallback23(false);
 check4(bool(absent)&&!absent->called&&!absent->returned&&absent->outputMask[1]==0&&absent->jointMask[2]==0,
  "missing original is unexecuted/unknown, not a fabricated post-core output hash");
 hostTickCount64+=500;setupOutput23(owner,ci);outputLines23.clear();auto changed=outputCallback23(true,3);
 check4(bool(changed)&&!changed->descriptorSame[0]&&changed->outputMask[0]==0&&changed->outputMask[1]==0,
  "changed native output pointer is unknown and not dereferenced");
 hostTickCount64+=500;setupOutput23(owner,ci);outputLines23.clear();auto epoch=outputCallback23(true,1);
 check4(bool(epoch)&&epoch->jointMask[0]==127&&epoch->jointMask[1]==0&&outputLines23.empty(),
  "body epoch change suppresses stale phase reads and joined emission, not original");
 hostTickCount64+=500;setupOutput23(owner,ci);outputLines23.clear();auto expired=outputCallback23(true,2);
 check4(bool(expired)&&expired->jointMask[1]==0&&outputLines23.empty(),"F3 revoked inside callback stops additional reads/formatting");
 hostTickCount64+=500;cap4::gate.select(cap4::Domain::Combat,true,hostTickCount64);setupOutput23(owner,ci);
 g_pose6689.character=reinterpret_cast<void*>(0x9999);poseReads23=0;outputLines23.clear();auto map=outputCallback23();
 check4(bool(map)&&!map->mapKnown&&poseReads23==0&&outputLines23.size()==1,
  "different character anatomy cannot supply guessed leg IDs; emits mapKnown0 only");
 hostTickCount64+=500;setupOutput23(owner,ci);unreadablePose23=true;poseReads23=0;outputLines23.clear();auto unreadable=outputCallback23();
 check4(unreadable&&unreadable->jointMask[0]==123&&unreadable->jointMask[1]==123&&unreadable->jointMask[2]==123,
  "nonnull getter with unreadable span remains explicitly unknown in all phases, never a known zero transform");
 unreadablePose23=false;
 hostTickCount64+=500;setupOutput23(owner,ci);g_pose6689.leg[1].ankle=20;poseReads23=0;outputLines23.clear();auto bounds=outputCallback23();
 check4(bool(bounds)&&!bounds->mapKnown&&poseReads23==0,"derived joint equal to actual skeleton count is never read or hashed");
 hostTickCount64=cap4::gate.until[unsigned(cap4::Domain::Combat)].load();setupOutput23(owner,ci);poseReads23=0;outputLines23.clear();auto finite=outputCallback23();
 check4(!bool(finite)&&poseReads23==0&&outputLines23.empty(),"finite F3 expiry stops new reads even while selected mask remains set");
 hostTickCount64+=500;cap4::gate.select(cap4::Domain::Combat,true,hostTickCount64);setupOutput23(owner,ci);kcd2vr::animation_filter::nativeKickActive175=false;
 poseReads23=0;outputLines23.clear();auto noKick=outputCallback23();
 check4(!bool(noKick)&&poseReads23==0&&outputLines23.empty(),"F3 alone does not add joint reads without actual activated kick ownership");
 cap4::gate.select(cap4::Domain::Combat,false,hostTickCount64);cap4::gate.select(cap4::Domain::Interaction,false,hostTickCount64);
 cap4::gate.select(cap4::Domain::Vrik,false,hostTickCount64);g_skinning_core_original=nullptr;g_pose6689={};
}
int main(){
 hostTickCount64=10000;g_pose_epoch6689=14;g_pose_restore_fault6689=false;
 TrackingSnapshot t{};t.sequence=51;t.tick=10000;t.input_tick=9995;t.timeNs=778899;
 t.head_good_tick=9998;t.right_good_tick=9997;t.left_good_tick=9996;t.source_epoch=kcd2vr::tracking59c6::epoch.load();
 t.allowed=t.head_valid=t.right_valid=t.left_valid=true;t.right_held=true;
 cap4::gate.select(cap4::Domain::Vrik,false,hostTickCount64);callback4(0,t);
 check4(original4==1&&records4().empty(),"F7 OFF actual extracted callback forwards original once with no receipt");
 const char* expected[]{"SKIP_NATIVE_BODY","SKIP_NATIVE_REFERENCE","SKIP_NATIVE_WEAPON_MODE","SKIP_VRIK_OFF","SKIP_RIG","SKIP_RAW_POSE_API","SKIP_NO_TRACKING_SNAPSHOT","SKIP_TRACKING_AGE"};
 for(unsigned n=0;n<8;++n){hostTickCount64+=300;t.tick=hostTickCount64;t.sequence++;
  // Actual existing recorder-independent native-reference selector, reset later.
  diagnostics::lab::Set(diagnostics::lab::Key::F3,n==1?1u:0u);
  cap4::gate.select(cap4::Domain::Vrik,true,hostTickCount64);records4();const auto before=original4;
  callback4(n,t);auto out=records4();check4(out.size()==1,"one exact natural skip cycle, no stale pose dump");const auto& r=out.at(0);
  check4(original4==before+1&&r.nativeCalled&&r.nativeReturned&&!r.started&&!r.committed&&!r.published&&!r.poseData,
   "each refusal still forwards original exactly once and never invents a writer");
  check4(std::strcmp(r.stage,expected[n])==0&&r.sample==t.sequence&&r.character==0x12345000&&r.epoch==14,
   "exact natural pretransaction failure and live actor/body/source join retained");
  check4(r.sourceTick==(n==7?t.tick-101:t.tick)&&r.inputTick==t.input_tick&&r.timeNs==t.timeNs&&r.goodRight==t.right_good_tick&&(r.tracking&16u),
   "pretransaction failure keeps original source/good/input/held provenance");
 }
 diagnostics::lab::Set(diagnostics::lab::Key::F3,0);hostTickCount64+=300;t.tick=hostTickCount64;
 cap4::gate.select(cap4::Domain::Vrik,true,hostTickCount64);records4();callback4(0,t,false);auto out=records4();
 check4(out.size()==1&&!out[0].nativeCalled&&!out[0].nativeReturned,"absent native original is unknown/unexecuted, not a fabricated return");
 cap4::gate.select(cap4::Domain::Vrik,false,hostTickCount64);g_skinning_core_original=nullptr;
 check4(kcd2vr::animation_filter::reloadPassengerEligibilityCallsXR34==0,"ordinary F7 refusal/recorder paths never consume XR34 native eligibility");
 outputChecks23();
 std::printf("VRIK_CALLBACK_H29R4 %u/%u actual-TU/extracted-callback checks; native/headset=NOT_EXECUTED\n",checks4-failures4,checks4);return failures4?1:0;
}
