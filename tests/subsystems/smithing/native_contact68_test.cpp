// Actual diagnostic detour, with only memory/native-forward/recorder leaves
// substituted. Native results are not emulated or claimed headset-tested.
#include "../../../src/subsystems/smithing/smith_contact68.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <map>
#include <string>
#include <vector>
namespace c68=kcd2vr::subsystems::smithing::contact68;
namespace st=kcd2vr::smith_tool36;
static constexpr std::uintptr_t base68=0x180000000ull,instance68=0x10000,state68=0x11000,player68=0x12000,
 henry68=0x13000,model68=0x14000,holder68=0x15000,definition68=0x16000,workpiece68=0x17000,
 hammer68=0x18000,attachment68=0x19000,manager68=0x1A000,character68=0x1B000,object68=0x1C000,
 name68=0x1D000,slots68=0x1E000;
static unsigned checks68=0,reads68=0,native68=0,clock68=0,hammerReads68=0,lookups68=0;
static std::uint64_t now68=2000;
static std::uintptr_t fixtureCaller68=base68+0x859A76;
static std::map<std::uintptr_t,unsigned char> memory68;
static std::vector<std::string> lines68;
static c68::Vec3* output68=nullptr;
static c68::HammerFrame current68{};
static bool mutateLifetime68=false,foreignReturn68=false,retireDuringHammer68=false;
static std::uintptr_t mutateRead68=0;static unsigned mutateCount68=0;
template<class T>static void put68(std::uintptr_t at,const T& v){const auto* b=reinterpret_cast<const unsigned char*>(&v);for(unsigned i=0;i<sizeof(v);++i)memory68[at+i]=b[i];}
static bool fixtureRead68(std::uintptr_t at,void* out,std::size_t n) noexcept {
 ++reads68;
 if(output68&&at==reinterpret_cast<std::uintptr_t>(output68)&&n==sizeof(*output68)){std::memcpy(out,output68,n);return true;}
 if(at==mutateRead68&&++mutateCount68==2)return false;
 auto* b=static_cast<unsigned char*>(out);for(std::size_t i=0;i<n;++i){const auto p=memory68.find(at+i);if(p==memory68.end())return false;b[i]=p->second;}return true;
}
static std::uint64_t GetTickCount64(){++clock68;return now68;}
namespace kcd2vr::combat_rules178r10 {inline std::atomic<int> f5Rung{1};}
namespace kcd2vr::diagnostics::capture168 {
enum class Domain:unsigned {Interaction=0};
inline bool recording=true;
inline bool active(Domain,std::uint64_t){return recording;}
struct Gate {std::atomic<std::uint64_t> session[1]{{1}};};inline Gate gate;
}
namespace kcd2vr::activity_input163 {
enum class Kind175:unsigned {Smithing=0};
struct Witness {std::atomic<bool> ready{true};std::atomic<unsigned> transitionDepth175{0};
 std::atomic<std::uint64_t> lifetime175{7},smithEpochC1{3};std::atomic<std::uintptr_t> active[1]{{instance68}};};inline Witness witness175;
}
namespace diagnostic_trace {static void Write(const char* s){lines68.emplace_back(s);}}
#define KCD2VR_SMITH_CONTACT68_FIXTURE 1
#ifndef _MSC_VER
#define __fastcall
#endif
#include "../../../src/subsystems/smithing/native_contact68.inl"
static void ck68(bool v,const char* what){++checks68;if(!v){std::fprintf(stderr,"FAIL %u: %s\n",checks68,what);std::exit(1);}}
static c68::Vec3* __fastcall original68(void*,c68::Vec3* out){++native68;*out={10.25f,20.5f,30.f};static c68::Vec3 other{};return foreignReturn68?&other:out;}
static std::uintptr_t find68(std::uint32_t id) noexcept{++lookups68;return id==44?workpiece68:0;}
static bool hammerFrame68(c68::HammerFrame& h,std::uint64_t) noexcept{
 ++hammerReads68;h=current68;if(mutateLifetime68)++kcd2vr::activity_input163::witness175.lifetime175;
 if(retireDuringHammer68)c68::detail68::ready=false;return true;
}
static unsigned physicsQueriesD39=0;static bool physicsAvailableD39=false,changeModeDuringQueryD39=false;
static bool physicsQueryD39(const double (&hit)[3],kcd2vr::subsystems::smithing::anvil_physics_a3::Box* boxes,unsigned& n,const char*& why){
 ++physicsQueriesD39;n=0;why="FIXTURE_TRANSIENT_GEOMETRY_MISS";
 if(changeModeDuringQueryD39)kcd2vr::subsystems::smithing::anvil_a2::setMode(2);
 if(!physicsAvailableD39)return false;
 auto& b=boxes[0];b.entity=0xABCD;b.mn[0]=hit[0]-.30;b.mx[0]=hit[0]+.30;b.mn[1]=hit[1]-.15;b.mx[1]=hit[1]+.15;
 b.mn[2]=hit[2]-.82;b.mx[2]=hit[2]-.02;n=1;why="OK";return true;
}
static void reset68(){
 memory68.clear();lines68.clear();reads68=native68=clock68=hammerReads68=lookups68=0;
 now68=2000;physicsQueriesD39=0;physicsAvailableD39=changeModeDuringQueryD39=false;
 namespace av=kcd2vr::subsystems::smithing::anvil_a2;
 av::setMode(0);av::producerRing={};av::published=0;av::mailbox.publish({});
 c68::detail68::a3::lastId={};c68::detail68::a3::tried=c68::detail68::a3::found=c68::detail68::a3::missNotified=false;
 c68::detail68::a3::lastAttempt=c68::detail68::a3::modeRevision=0;c68::detail68::a3::fixtureQuery=&physicsQueryD39;
 mutateRead68=0;mutateCount68=0;mutateLifetime68=foreignReturn68=retireDuringHammer68=false;fixtureCaller68=base68+0x859A76;
 c68::detail68::original=&original68;c68::detail68::base=base68;c68::detail68::ready=true;c68::detail68::budget={};c68::exhaustedSession=0;
 kcd2vr::semi_smith46::clockControl.publish({});
 kcd2vr::combat_rules178r10::f5Rung=1;kcd2vr::diagnostics::capture168::recording=true;
 kcd2vr::diagnostics::capture168::gate.session[0]=1;
 auto& w=kcd2vr::activity_input163::witness175;w.ready=true;w.transitionDepth175=0;w.lifetime175=7;w.smithEpochC1=3;w.active[0]=instance68;
 put68(instance68,base68+st::kInstanceVft);put68(instance68+0x2C,std::uint8_t(1));put68(instance68+0xB0,state68);put68(state68,instance68);
 put68(instance68+0x18,player68);put68(player68,base68+st::kPlayerVft);put68(player68+0x38,henry68);put68(henry68,base68+0x4072C78);
 put68(player68+0x30,std::uint32_t(900));put68(player68+st::kPlayerSlots,slots68);
 put68(instance68+0x98,model68);put68(model68,base68+st::kModelVft);put68(model68+0x88,std::uint8_t(16));
 put68(model68+0x198,base68+0x3A6A1E0);put68(model68+0x1A0,c68::Vec2{.4f,.6f});put68(model68+0x2F8,holder68);
 put68(holder68,base68+st::kWorkpieceVft);put68(holder68+0x10,std::uint32_t(44));put68(holder68+0x18,definition68);
 put68(workpiece68,base68+0x4072C78);put68(workpiece68+0xC,std::uint32_t(44));
 const float matrix[12]{1,0,0,10,0,1,0,20,0,0,1,30};put68(workpiece68+0x58,matrix);put68(henry68+0x58,matrix);
 put68(definition68+0x40,c68::Vec2{.18f,.3f});put68(definition68+0x48,c68::Vec2{0,-.15f});put68(definition68+0x50,c68::Vec3{0,1.57f,1.57f});
 put68(model68+st::kHammerSlot,hammer68);put68(hammer68,base68+st::kSlotToolVft);put68(hammer68+st::kHand,int(0));
 put68(hammer68+st::kHost,std::uint32_t(900));put68(hammer68+st::kEntity,std::uint32_t(42));put68(hammer68+st::kAttached,std::uint8_t(1));
 put68(hammer68+st::kKey,std::uint64_t(0x0200000000000005ull));put68(slots68+st::kHandKeys,std::uint64_t(0x0200000000000005ull));
 put68(attachment68,base68+st::kAttachmentVft);put68(attachment68+0x28,manager68);put68(manager68+0x70,character68);
 put68(attachment68+0x10,name68);const char name[]="hand_right";put68(name68,name);put68(attachment68+0x20,object68);
 put68(object68,base68+st::kObjectVft);put68(object68+8,std::uint32_t(42));
 const auto rd=[](std::uintptr_t at,auto& v){return fixtureRead68(at,&v,sizeof(v));};
 current68={};ck68(st::read(rd,base68,instance68,player68,character68,attachment68,42,16,current68.binding),"real pure smith reader joins native hammer");
 current68.binding.lifetime=7;current68.tick=now68;current68.sample=11;current68.poseEpoch=4;current68.sourceEpoch=8;current68.modeEpoch=9;
 current68.world[3]=current68.world[7]=1;current68.world[4]=10.2f;current68.world[5]=20.4f;current68.world[6]=30.1f;current68.placed=true;
 c68::readHammer=&hammerFrame68;c68::findEntity=&find68;reads68=0;
}
static c68::Vec3 result68;
static c68::Vec3* invoke68(){output68=&result68;return c68::detail68::detour(reinterpret_cast<void*>(state68),output68);}
int main(){
 reset68();kcd2vr::combat_rules178r10::f5Rung=0;
 ck68(invoke68()==&result68&&native68==1,"off forwards original exactly once and returns same pointer");
 ck68(reads68==0&&clock68==0&&hammerReads68==0&&lookups68==0&&lines68.empty(),"off has zero native reads/callbacks/format/clock");
 reset68();kcd2vr::diagnostics::capture168::recording=false;invoke68();
 ck68(native68==1&&reads68==0&&hammerReads68==0&&lookups68==0&&lines68.empty(),"recorder off does no diagnostic reads or callbacks");
 for(int rung:{2,3}){reset68();kcd2vr::combat_rules178r10::f5Rung=rung;invoke68();
  ck68(native68==1&&lines68.size()==1&&lines68[0].find("hammerJoined=1")!=std::string::npos,"all active F5 rungs observe the same exact native StrokeHit");}
 reset68();fixtureCaller68=base68+0x859895;invoke68();
 ck68(native68==1&&reads68==0&&clock68==0&&lines68.empty(),"particle caller never counts as StrokeHit");
 reset68();c68::detail68::ready=false;invoke68();ck68(native68==1&&reads68==0&&clock68==0&&lines68.empty(),"retired hook forwards native only");
 reset68();retireDuringHammer68=true;invoke68();ck68(native68==1&&lines68.empty(),"retirement during diagnostic callback prevents later formatting");
 reset68();ck68(invoke68()==&result68&&native68==1,"recorded native original once");
 ck68(lines68.size()==1&&lines68[0].find("hammerJoined=1")!=std::string::npos,"same native event joins real placed current hammer");
 ck68(lines68[0].find("cursorNative=(0.40000,0.60000)")!=std::string::npos&&lines68[0].find("pointWorld=(10.2500,20.5000,30.0000)")!=std::string::npos,"records actual native output AND separate normalized cursor");
 ck68(lines68[0].find("contact3D=UNPROVEN")!=std::string::npos&&result68.x==10.25f&&result68.z==30.f,"no contact/quality claim or native point rewrite");
 // D57: actual native callback joins existing input snapshot metadata only.
 reset68();kcd2vr::semi_smith46::ClockControl strokeCommand{};auto& receipt=strokeCommand.diagnosticD57;
 receipt.instance=instance68;receipt.session=1;receipt.lifetime=7;receipt.id=23;
 receipt.pose=4;receipt.source=8;receipt.modeEpoch=9;receipt.mode=3;receipt.nativeHeight=true;
 receipt.startTick=1800;receipt.releaseTick=1950;receipt.sample=99;receipt.sampleTick=receipt.tick=1990;
 kcd2vr::semi_smith46::clockControl.publish(strokeCommand);invoke68();
 ck68(native68==1&&lines68.size()==1&&lines68[0].find("strokeIdD57=23")!=std::string::npos,"actual native consumer receives input stroke ID");
 ck68(lines68[0].find("strokeJoin=CORRELATED_NATIVE_POINT_ONLY")!=std::string::npos&&lines68[0].find("releaseToPointMs=50")!=std::string::npos,"same station/epochs join measured release-to-point interval");
 ck68(lines68[0].find("callbackKind=NATIVE_POINT_CALLBACK")!=std::string::npos&&lines68[0].find("acceptance=UNOBSERVED")!=std::string::npos,"full extended receipt is untruncated and never calls projection acceptance");
 const auto sourceReads=reads68;reset68();invoke68();ck68(reads68==sourceReads,"stroke correlation adds zero native memory reads");
 reset68();receipt.source=88;kcd2vr::semi_smith46::clockControl.publish(strokeCommand);invoke68();
 ck68(lines68[0].find("strokeJoin=EPOCH_CHANGED")!=std::string::npos,"real callback rejects mismatched input epoch");
 reset68();receipt.source=8;receipt.tick=receipt.sampleTick=1919;kcd2vr::semi_smith46::clockControl.publish(strokeCommand);invoke68();
 ck68(lines68[0].find("strokeJoin=INPUT_STALE")!=std::string::npos,"real callback refuses stale input");
 reset68();receipt.tick=receipt.sampleTick=1990;receipt.releaseTick=0;kcd2vr::semi_smith46::clockControl.publish(strokeCommand);invoke68();
 ck68(lines68[0].find("strokeJoin=NO_DOWN_REVERSAL")!=std::string::npos,"native/manual or pre-release point is not labelled a VR blow");
 reset68();current68.tick=now68-81;invoke68();ck68(lines68[0].find("hammerJoined=0")!=std::string::npos,"stale placement is not a current hammer");
 reset68();current68.sourceEpoch=0;invoke68();ck68(lines68[0].find("hammerJoined=0")!=std::string::npos,"missing source epoch refuses hammer join");
 reset68();current68.binding.lifetime=6;invoke68();ck68(lines68[0].find("hammerJoined=0")!=std::string::npos,"old minigame lifetime refuses hammer join");
 reset68();put68(slots68+st::kHandKeys,std::uint64_t(99));invoke68();ck68(lines68[0].find("hammerJoined=0")!=std::string::npos,"replaced native hand WUID refuses join");
 reset68();current68.binding.phase=14;invoke68();ck68(lines68[0].find("hammerJoined=1")!=std::string::npos,"placed grip may cross14to16 but live native reader independently qualifies16");
 reset68();mutateLifetime68=true;invoke68();ck68(lines68[0].find("reason=NATIVE_LIFECYCLE_CHANGED")!=std::string::npos&&lines68[0].find("hammerJoined=0")!=std::string::npos,"lifetime change during callback erases old observed identity");
 reset68();mutateRead68=model68+0x2F8;invoke68();ck68(lines68[0].find("reason=NATIVE_CONTEXT_CHANGED")!=std::string::npos,"changed workpiece holder during exact read refuses context");
 reset68();foreignReturn68=true;invoke68();ck68(lines68[0].find("reason=NATIVE_RETURN_IDENTITY_OR_POINT")!=std::string::npos&&lookups68==0,"foreign native return pointer never dereferenced as supplied output");
 reset68();for(int i=0;i<127;++i)invoke68();
 ck68(c68::exhaustedSession.load()==0,"placement readback remains available before the finite budget is full");
 invoke68();ck68(c68::exhaustedSession.load()==1,"128th admitted native event publishes the exact exhausted capture session");
 ck68(lines68.back().find("budgetExhausted=1")!=std::string::npos,"last finite native receipt explicitly marks exhausted coverage");
 for(int i=128;i<200;++i)invoke68();ck68(native68==200&&lines68.size()==128&&hammerReads68==128,"finite128 event capture; no native events suppressed");
 const auto spent=reads68;invoke68();ck68(reads68==spent&&native68==201,"exhausted capture does no more memory reads");
 kcd2vr::diagnostics::capture168::gate.session[0]=2;
 ck68(c68::exhaustedSession.load()!=kcd2vr::diagnostics::capture168::gate.session[0].load(),"new capture is not blocked by a prior exhausted session marker");
 invoke68();ck68(lines68.size()==129&&c68::detail68::budget.serial==1,"new session resets finite budget and event serial");
 for(int i=1;i<128;++i)invoke68();ck68(c68::exhaustedSession.load()==2,"new session publishes its own exhaustion independently");
 const float affine[12]{0,-2,0,10,2,0,0,20,0,0,2,30};c68::Vec3 local{};
 ck68(c68::localPoint(affine,{6,22,36},local)&&std::fabs(local.x-1)<.00001&&std::fabs(local.y-2)<.00001&&std::fabs(local.z-3)<.00001,"real world-to-workpiece affine inversion handles rotation/scale");
 float singular[12]{};ck68(!c68::localPoint(singular,{1,2,3},local),"singular transform fails closed");
 ck68(!c68::localPoint(affine,{std::numeric_limits<float>::infinity(),2,3},local),"nonfinite point fails closed");
 // D39: actual production observer, native original still once on every call.
 namespace av=kcd2vr::subsystems::smithing::anvil_a2;
 reset68();av::setMode(1);kcd2vr::diagnostics::capture168::recording=false;
 invoke68();ck68(native68==1&&physicsQueriesD39==1&&av::published.load()==1,"first exact native blow may query real geometry while F5 recording is off");
 now68+=200;invoke68();ck68(native68==2&&physicsQueriesD39==1,"transient physics miss is throttled on subsequent native blows");
 physicsAvailableD39=true;now68+=800;invoke68();ck68(native68==3&&physicsQueriesD39==2,"later actual blow retries a miss after one second");
 av::Ring ring{};ck68(av::mailbox.read(ring)&&ring.blow[2].boxValid&&ring.blow[2].phys==0xABCD,"successful retry publishes exact observed static box");
 const auto revision=av::modeRevisionD39.load();ck68(ring.blow[2].modeRevisionD39==revision,"native observation carries the current menu generation");
 now68+=1100;invoke68();ck68(physicsQueriesD39==2,"successful same-station geometry stays cached");
 fixtureCaller68=base68+0x859895;now68+=1100;invoke68();ck68(native68==5&&physicsQueriesD39==2,"foreign caller cannot trigger geometry query or retry");
 fixtureCaller68=base68+0x859A76;av::setMode(2);invoke68();ck68(physicsQueriesD39==2,"learned source does not query real physics");
 av::setMode(1);invoke68();ck68(physicsQueriesD39==3,"returning to real source revalidates its native geometry cache");
 reset68();av::setMode(1);physicsAvailableD39=changeModeDuringQueryD39=true;invoke68();
 ck68(native68==1&&physicsQueriesD39==1&&av::published.load()==0,"mode change during the native read cannot publish geometry in a new mode");
 std::printf("native_contact68_test: %u checks PASS\n",checks68);return 0;
}
