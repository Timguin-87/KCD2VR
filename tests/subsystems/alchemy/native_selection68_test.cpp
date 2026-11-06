#if defined(KCD2VR_NOMINEE_LOS_JOINED)
// Entire current interaction TU and the real input/near/bridge owners. These
// host tests control only copied-frame, native-query and platform leaves.
#define KCD2VR_INPUT67_NO_MAIN
#include "input67_test.cpp"
#include "../../../src/interaction_alignment.cpp"
#include <vector>
#include <thread>
namespace ia=kcd2vr::interaction_alignment;
namespace b=kcd2vr::subsystems::alchemy::bridge67;
namespace ai=kcd2vr::activity_input163;
static unsigned assertions68=0,failures68=0;
static void expect68(bool v,const char* text){++assertions68;if(!v){++failures68;std::printf("FAIL %u %s\n",assertions68,text);}}
static std::vector<std::string> lines68;
namespace kcd2vr::diagnostic_trace {void Write(const char* text) noexcept {lines68.emplace_back(text);}}
namespace kcd2vr {
bool CameraHook::GetLatestFrustumRotationSample(CameraRotationSample&){return false;}
bool CameraHook::GetLatestVisualForwardWorld(float&,float&,float&,std::uint64_t){return false;}
bool XrManager::CopyActiveRenderFrameState(RenderFrameState&){return false;}
namespace retail_build112 {static bool fixtureReady68=true;bool Ready(std::uintptr_t) noexcept{return fixtureReady68;}bool Verify(std::uintptr_t) noexcept{return fixtureReady68;}}
namespace frik_pose_probe {
static workstation_sample46::Frame fixtureFrame68{};
static void(*frameRead68)()=nullptr;
bool WorkstationFrame46(workstation_sample46::Frame& f) noexcept {if(frameRead68)frameRead68();f=fixtureFrame68;return true;}
int ExactHandItemState(int) noexcept{return -1;}
unsigned PhysicalHandReservations114() noexcept{return 3;}
bool ReadInteractionPalm59c9(int,float&,float&,float&,float&,float&,float&,std::uint64_t&,std::uint64_t*,std::uintptr_t*) noexcept{return false;}
}}
struct Packet68 {float origin[3]{},delta[3]{};int types=0,flags=0;std::uintptr_t skip[2]{};unsigned char count=0;char mode=0;std::uintptr_t returnPC=0;};
struct Call68 {Packet68* packet;const float* origin;const float* delta;const void* row;std::uintptr_t interactor;unsigned key,types,flags;unsigned char count;std::uintptr_t* skip;char mode;};
static_assert(offsetof(Call68,interactor)==32&&offsetof(Call68,key)==40&&offsetof(Call68,types)==44&&offsetof(Call68,flags)==48&&offsetof(Call68,count)==52&&offsetof(Call68,skip)==56&&offsetof(Call68,mode)==64);
extern "C" void NomineeHarness68(Call68*);
extern "C" unsigned char NomineeSeam68[],NomineeReturn68[];
extern "C" {void* RayDispatchTarget68=reinterpret_cast<void*>(&ia::RayBuildDetour);std::uintptr_t QueryReturn68=0;}
static void(*afterCopy68)()=nullptr;
static unsigned nativeCalls68=0;
extern "C" __declspec(noinline) std::uintptr_t PacketLeaf68(Packet68* out,const float* origin,const float* delta,int types,int flags,std::uintptr_t* skip,unsigned char count,char mode){
 ++nativeCalls68;std::memcpy(out->origin,origin,12);std::memcpy(out->delta,delta,12);out->types=types;out->flags=flags;out->count=count;out->mode=mode;out->returnPC=QueryReturn68;
 for(unsigned i=0;i<2;++i)out->skip[i]=skip[i];if(afterCopy68)afterCopy68();return reinterpret_cast<std::uintptr_t>(out);
}
static std::array<unsigned char,0xB20> player68{};
static std::array<unsigned char,0x100> interactor68{},henry68{};
static std::array<unsigned char,0x40> policy68{},policies68{},row68{};
static std::uintptr_t address68(auto& x){return reinterpret_cast<std::uintptr_t>(x.data());}
template<class T>static void put68(auto& dst,std::size_t off,T v){std::memcpy(dst.data()+off,&v,sizeof(v));}
static constexpr float nativeOrigin68[3]{0,0,1.6f};
static float nativeDelta68[3]{};
static std::uintptr_t skips68[2]{0x11,0x22};
static Fixture fixture68;
static a::Output output68{};
static void publish68(){
 using namespace kcd2vr;
 fixture68.in.now=GetTickCount64();fixture68.f.tick=fixture68.in.now;fixture68.f.timeNs=fixture68.in.now*1000000;
 auto& t=fixture68.in.nativeFrame.table;t.tick=fixture68.in.now;
 for(auto& o:fixture68.in.nativeFrame.offers)if(o.valid)o.tick=t.tick;
 for(auto& p:fixture68.in.palm)p.tick=t.tick;
 for(auto& p:fixture68.in.world)p.tick=t.tick;
 fixture68.in.action.tick=t.tick;
 output68=fixture68.step();
 frik_pose_probe::fixtureFrame68=fixture68.f;
 b::table.publish(t);for(unsigned i=0;i<n::Capacity;++i)b::offers[i].publish({t.instance,t.player,t.lifetime,fixture68.in.nativeFrame.offers[i]});
 b::nomination.publish(output68.nomination);b::control.publish(output68.control);
}
static void resetJoined68(int side=1,int type=18,bool proximity=true){
 // R116 OFF here: these cases pin the R115 palm-origin packet; the R116 box-surface origin
 // (head_select_r116::losOrigin) is covered by tests/alchemy_view_select_r116_test.cpp.
 kcd2vr::subsystems::alchemy::head_select_r116::enabled.store(false);
 using namespace kcd2vr;fixture68=Fixture{};player68={};interactor68={};henry68={};policy68={};policies68={};row68={};
 ia::g_wh_base=0x180000000ull;ia::g_ray_original=reinterpret_cast<ia::RayQueryBuildFn>(&PacketLeaf68);ia::g_lookray_lo=ia::g_wh_base+0x82C7D8;ia::g_lookray_hi=ia::g_wh_base+0x82CCD8;ia::g_enabled=true;ia::g_selection_ready68=true;
#if KCD2VR_TEST_LOS_PRESENT
 ia::g_nominee_los_ready68=true;
#endif
 b::enabled=true;minigame_hands45::modeEpoch67=6;tracking59c6::epoch=5;
 auto& t=fixture68.in.nativeFrame.table;t.instance=0x12345678;t.player=address68(player68);t.playerEntity=address68(henry68);t.lifetime=7;
 fixture68.in.context={t.instance,t.player,t.lifetime,4,5,6,t.lifetime};fixture68.in.action.instance=t.instance;fixture68.in.action.lifetime=t.lifetime;
 ai::witness175.ready=true;ai::witness175.transitionDepth175=0;ai::witness175.lifetime175=t.lifetime;
 for(auto& active:ai::witness175.active)active=0;ai::witness175.active[unsigned(ai::Kind175::Alchemy)]=t.instance;
 put68(player68,0,ia::g_wh_base+0x3AA4B98);put68(player68,0x38,t.playerEntity);put68(player68,0xB08,address68(interactor68));
 put68(interactor68,0,ia::g_wh_base+0x3A6A0C8);put68(interactor68,8,t.player);put68(interactor68,0xD8,address68(policies68));put68(interactor68,0xE0,std::uintptr_t(3));
 put68(policies68,0,1u);put68(policies68,8,address68(policy68));put68(policies68,16,2u);put68(policies68,32,4u);
 put68(policy68,0,ia::g_wh_base+0x3A69D90);put68(policy68,8,t.player);put68(henry68,0,ia::g_wh_base+0x4072C78);
 for(auto& o:fixture68.in.nativeFrame.offers)o={};
 auto& offer=fixture68.in.nativeFrame.offers[type];offer.ref=t.refs[type];offer.valid=offer.boundsValid=true;
 const float endpoint[3]{.6f,0,.9f};for(unsigned j=0;j<3;++j){offer.bounds.local[j]=-.02f;offer.bounds.local[j+3]=.02f;offer.bounds.world[4*j+j]=1;offer.bounds.world[4*j+3]=endpoint[j];}
 offer.point={endpoint[0],endpoint[1],endpoint[2]};
 fixture68.in.proximityCraft1=proximity;
 for(int h=0;h<2;++h){fixture68.in.palm[h].point={h==side?.55:2,0,.92};fixture68.in.world[h].point[0]=float(fixture68.in.palm[h].point.x);fixture68.in.world[h].point[2]=.92f;fixture68.grip(h,h==side?1.f:0.f);}
 put68(row68,0,t.refs[type].entity);for(unsigned j=0;j<3;++j){put68(row68,0x18+4*j,endpoint[j]);nativeDelta68[j]=endpoint[j]-nativeOrigin68[j];}
 afterCopy68=nullptr;frik_pose_probe::frameRead68=nullptr;publish68();
 expect68(!output68.use&&!output68.admitted,"actual near input nominates without fabricated native action");
 b::Nomination actual{};expect68(b::selectionOwner68(t,actual,fixture68.in.now)==side&&actual.decision.ref.entity==t.refs[type].entity,"actual current bridge nominee and side join");
}
static Packet68 invoke68(unsigned key=1,std::uintptr_t interactor=0){
 Packet68 p{};Call68 call{&p,nativeOrigin68,nativeDelta68,row68.data(),interactor?interactor:address68(interactor68),key,0x11f,0x40f,2,skips68,2};NomineeHarness68(&call);return p;
}
static bool palm68(const Packet68& p){const auto& q=output68.nomination.decision.palm.point;return p.origin[0]==float(q.x)&&p.origin[1]==float(q.y)&&p.origin[2]==float(q.z);}
static bool native68(const Packet68& p){return std::memcmp(p.origin,nativeOrigin68,12)==0&&std::memcmp(p.delta,nativeDelta68,12)==0;}
static void packetMetadata68(const Packet68& p){expect68(p.types==0x11f&&p.flags==0x40f&&p.count==2&&p.mode==2&&p.skip[0]==skips68[0]&&p.skip[1]==skips68[1],"all native query metadata unchanged");expect68(p.returnPC==reinterpret_cast<std::uintptr_t>(NomineeReturn68),"original packet call return PC unchanged");}
static bool visibility68(const Packet68& p,unsigned collisionID){
 // Model of native82A7A8 plus an exact geometry68 ray, not engine execution.
 kcd2vr::subsystems::alchemy::geometry68::Bounds box{};box.local[0]=.2f;box.local[1]=-.1f;box.local[2]=1;box.local[3]=.25f;box.local[4]=.1f;box.local[5]=1.5f;for(int i=0;i<3;++i)box.world[4*i+i]=1;
 const double len=std::sqrt(double(p.delta[0])*p.delta[0]+double(p.delta[1])*p.delta[1]+double(p.delta[2])*p.delta[2]);
 const auto hit=kcd2vr::subsystems::alchemy::geometry68::ray(box,{p.origin[0],p.origin[1],p.origin[2]},{p.delta[0]/len,p.delta[1]/len,p.delta[2]/len},len);
 unsigned id=0;std::memcpy(&id,row68.data(),4);return !hit.valid||id==collisionID||len<=hit.distance+.01;
}
#if KCD2VR_TEST_LOS_PRESENT
static void seedPins68(std::uintptr_t base,const auto& pins){for(const auto& pin:pins){std::vector<unsigned char> bytes;for(const char* q=pin.hex;*q;q+=2){const auto digit=[](char c){return c<='9'?c-'0':c-'a'+10;};bytes.push_back(static_cast<unsigned char>((digit(q[0])<<4)|digit(q[1])));}std::memcpy(reinterpret_cast<void*>(base+pin.rva),bytes.data(),bytes.size());}}
static int installMode68(const char* mode){
 using namespace DetourModKit::hook;
 auto* image=static_cast<unsigned char*>(VirtualAlloc(nullptr,0x4200000,MEM_RESERVE|MEM_COMMIT,PAGE_EXECUTE_READWRITE));expect68(image!=nullptr,"own-process image fixture allocation");if(!image)return 3;
 const auto base=reinterpret_cast<std::uintptr_t>(image);ia::g_wh_base=base;
 seedPins68(base,ia::kNativeSelectionPins68);seedPins68(base,ia::kNativeNomineeLosPins68);
#if KCD2VR_TEST_PACKET_PREPIN
 seedPins68(base,ia::kNativeNomineePacketPins68);
#endif
 for(const auto& pin:ia::kNativeSelectionPointers68){auto value=base+pin[1];std::memcpy(image+pin[0],&value,8);}
 ia::g_split_ready57c1=true;
 std::optional<Hook> foreign;
 const bool pinFail=std::strcmp(mode,"pin_fail")==0,foreignFail=std::strcmp(mode,"foreign_fail")==0,retailFail=std::strcmp(mode,"retail_fail")==0,disableFail=std::strcmp(mode,"disable_fail")==0;
#if KCD2VR_TEST_PACKET_PREPIN
 ia::PrepareNativeNomineeLos68(base);
#endif
 // Reproduce the actual production ordering: packet builder is already owned
 // and enabled when the optional descriptor seam is installed.
 Options rayOptions{};auto ray=inline_at(InlineRequest{"existing_packet_builder",Target{DetourModKit::Address{base+0x530324}},rayOptions},&ia::RayBuildDetour);
 expect68(bool(ray),"real preceding packet-builder inline hook");if(!ray)return 3;
 ia::g_ray_hook=std::move(*ray);ia::g_ray_original=ia::g_ray_hook->original<ia::RayQueryBuildFn>();
 expect68(bool(ia::g_ray_hook->enable()),"preceding packet-builder hook enabled");
 if(pinFail)image[0x7F590B]^=1;
 if(foreignFail){Options opts{};opts.fail_if_already_hooked=true;auto f=mid_at(MidRequest{"foreign_disabled_los_owner",Target{DetourModKit::Address{base+0x7F5B05}},opts},[](MidContext&){});expect68(bool(f),"real disabled foreign mid owner");if(f)foreign=std::move(*f);}
 kcd2vr::retail_build112::fixtureReady68=!retailFail;
 const bool installed=ia::InstallNativeSelection68(base);
 if(retailFail){expect68(!installed&&!ia::g_selector_hook68&&!ia::g_nominee_los_hook68,"wrong retail leaf refuses all new hooks");}
 else{
 expect68(installed&&ia::NativeSelectionReady68(),"original three-hooks remain ready independently");
 expect68(ia::g_selector_hook68&&ia::g_bounds_hook68&&ia::g_facing_hook68&&ia::g_selector_hook68->is_enabled()&&ia::g_bounds_hook68->is_enabled()&&ia::g_facing_hook68->is_enabled(),"all three actual original hook handles stay enabled");
 const bool optionalReady=!pinFail&&!foreignFail;
 expect68(ia::g_nominee_los_ready68.load()==optionalReady,"optional readiness independently reflects real pin/factory result");
 expect68(ia::g_nominee_los_attempted68,"optional installation attempt retained");
 auto count=lines68.size();expect68(ia::InstallNativeSelection68(base)&&lines68.size()==count,"ready retry does not reallocate/log optional failure");
 std::array<unsigned char,5> installedBytes{};if(disableFail){std::memcpy(installedBytes.data(),image+0x7F5B05,5);image[0x7F5B05]^=1;FlushInstructionCache(GetCurrentProcess(),image+0x7F5B05,5);}
 ia::StopNativeSelection68();expect68(!ia::g_nominee_los_ready68&&!ia::NativeSelectionReady68(),"stop invalidates all authority before native disarm");
 expect68(ia::g_selector_hook68&&ia::g_bounds_hook68&&ia::g_facing_hook68&&(!optionalReady||ia::g_nominee_los_hook68),"teardown retains hook/trampoline lifetime");
 if(disableFail){expect68(ia::g_nominee_los_hook68->is_enabled(),"foreign patch prevents disable without losing handle");std::memcpy(image+0x7F5B05,installedBytes.data(),5);FlushInstructionCache(GetCurrentProcess(),image+0x7F5B05,5);ia::StopNativeSelection68();expect68(!ia::g_nominee_los_hook68->is_enabled(),"retained failed-disarm handle can be retried");}
 expect68(!ia::InstallNativeSelection68(base),"retired selector cannot rearm after stop");
 }
 if(ia::g_ray_hook)(void)ia::g_ray_hook->disable();
 std::printf("NOMINEE_LOS_INSTALL %s %u/%u checks\n",mode,assertions68-failures68,assertions68);return failures68?2:0;
}
#endif
int main(int argc,char** argv){
 using namespace DetourModKit::hook;
#if KCD2VR_TEST_LOS_PRESENT
 if(argc>1)return installMode68(argv[1]);
#endif
 std::array<unsigned char,10> seam{};std::memcpy(seam.data(),NomineeSeam68,10);
 expect68(std::memcmp(seam.data(),"\x44\x89\x74\x24\x20\xe8",6)==0,"harness matches exact five-byte native flags store then original E8");
#if KCD2VR_TEST_LOS_PRESENT
 Options opts{};opts.fail_if_already_hooked=true;
 auto result=mid_at(MidRequest{"joined_native_nominee_los",Target{DetourModKit::Address{reinterpret_cast<std::uintptr_t>(NomineeSeam68)}},opts},ia::NativeNomineeLos68);
 expect68(bool(result),"actual same-version DMK mid creation");if(!result)return 3;
 ia::g_nominee_los_hook68=std::move(*result);expect68(bool(ia::g_nominee_los_hook68->enable()),"actual DMK seam enabled");
 expect68(std::memcmp(NomineeSeam68+5,seam.data()+5,5)==0,"real mid steals only flags store: original E8 intact");
#endif
 resetJoined68();auto oldRayCalls=ia::g_interaction_calls.load();auto p=invoke68();expect68(ia::g_interaction_calls.load()==oldRayCalls,"actual ray wrapper preserves descriptor caller classification");expect68(palm68(p),"same actual nominee palm reaches native packet leaf");packetMetadata68(p);
 float endpoint[3]{};std::memcpy(endpoint,row68.data()+0x18,12);bool endpointSame=true;for(int i=0;i<3;++i)endpointSame=endpointSame&&std::abs(double(p.origin[i])+p.delta[i]-endpoint[i])<1e-7;
 expect68(endpointSame,"native row endpoint retained with hand-to-same-endpoint delta");
 expect68(visibility68(p,999),"native visibility model sees clear nominee hand segment");
 Packet68 old{};std::memcpy(old.origin,nativeOrigin68,12);std::memcpy(old.delta,nativeDelta68,12);expect68(!visibility68(old,999)&&visibility68(old,118),"foreign obstruction and exact target-self collision retain native policy distinction");
 const auto originalRow=row68;const auto originalPlayer=player68;(void)invoke68();
 expect68(originalRow==row68&&originalPlayer==player68,"native row and actor pose are not mutated");
 for(int fault=0;fault<15;++fault){resetJoined68();switch(fault){
 case 0:ia::g_enabled=false;break;case 1:b::enabled=false;break;case 2:ai::witness175.active[0]=0;break;case 3:++ai::witness175.lifetime175;break;case 4:ai::witness175.transitionDepth175=1;break;
 case 5:++fixture68.in.nativeFrame.table.instance;b::table.publish(fixture68.in.nativeFrame.table);break;
 case 6:put68(row68,0,119u);break;case 7:put68(player68,0x38,std::uintptr_t(123));break;case 8:put68(player68,0xB08,std::uintptr_t(123));break;
 case 9:kcd2vr::tracking59c6::epoch=99;break;case 10:++kcd2vr::minigame_hands45::modeEpoch67;break;
 case 11:kcd2vr::frik_pose_probe::fixtureFrame68.hand[1].valid=false;break;
 case 12:{auto t=fixture68.in.nativeFrame.table;t.tick=fixture68.in.now-81;b::table.publish(t);break;}
 case 13:put68(row68,0x18,std::numeric_limits<float>::quiet_NaN());break;
 case 14:{auto nominee=output68.nomination;nominee.decision.source=n::Source::Gaze;b::nomination.publish(nominee);break;}
 }auto bad=invoke68();expect68(native68(bad),"unqualified/currentness mismatch retains exact native inputs");packetMetadata68(bad);}
 resetJoined68();expect68(native68(invoke68(2)),"other policy keeps native input");expect68(native68(invoke68(1,address68(player68))),"other interactor keeps native input");
 resetJoined68(0,4);expect68(palm68(invoke68()),"right-hand ordinary table nominee uses same native input correction");
 resetJoined68(1,18,false);expect68(palm68(invoke68()),"default near left bellows nominates before A/held ownership and receives its palm packet");
 resetJoined68(0,4,false);expect68(palm68(invoke68()),"default near right ordinary item retains native-selection authority and receives its palm packet");
 resetJoined68();auto unchanged=output68;expect68(!unchanged.use&&!unchanged.admitted,"visibility input change alone never admits native Use");
 fixture68.advance();fixture68.in.nativeSelectedKnown=true;const auto& acceptedTable=fixture68.in.nativeFrame.table;
 fixture68.in.nativeSelected={acceptedTable.instance,acceptedTable.lifetime,fixture68.in.now,acceptedTable.refs[18].entity,18,false};
 auto accepted=fixture68.step();expect68(accepted.use&&accepted.admitted,"actual input still needs matching final native selection/hint");
 fixture68.advance(81);fixture68.in.nativeSelected.tick=fixture68.in.now;expect68(!fixture68.step().use,"original once-only contextual pulse expiry retained");

 resetJoined68();kcd2vr::frik_pose_probe::frameRead68=[](){kcd2vr::frik_pose_probe::frameRead68=nullptr;++ai::witness175.lifetime175;};expect68(native68(invoke68()),"native lifetime changing during copied-frame read keeps native input");
 resetJoined68();static Packet68 nested{};afterCopy68=[](){afterCopy68=nullptr;resetJoined68(0,4);fixture68.in.palm[0].point.x=.56;publish68();nested=invoke68();};auto outer=invoke68();expect68(outer.origin[0]==.55f&&nested.origin[0]==.56f&&outer.delta[0]!=nested.delta[0],"nested native query after copy owns an independent packet");packetMetadata68(outer);
 resetJoined68();kcd2vr::frik_pose_probe::frameRead68=[](){kcd2vr::frik_pose_probe::frameRead68=nullptr;const auto inner=invoke68(2);expect68(native68(inner),"reentrant unrelated query cannot borrow ancestor nomination");};expect68(palm68(invoke68()),"outer copied input survives nested query before publication");
 resetJoined68();kcd2vr::frik_pose_probe::frameRead68=[](){kcd2vr::frik_pose_probe::frameRead68=nullptr;
  fixture68.in.palm[1].point.x=.56;publish68();const auto inner=invoke68();expect68(inner.origin[0]==.56f,"nested qualified query copies its own real nominee input");
  fixture68.in.palm[1].point.x=.55;publish68();};const auto outerBeforeCopy=invoke68();expect68(outerBeforeCopy.origin[0]==.55f,"outer input publication follows nested qualified packet copy");

#if KCD2VR_TEST_LOS_PRESENT
 ia::StopNativeSelection68();expect68(!ia::g_nominee_los_ready68&&ia::g_nominee_los_hook68,"stop revokes authority and retains mid handle");expect68(native68(invoke68()),"stopped callback/native seam leaves original inputs");ia::StopNativeSelection68();expect68(std::memcmp(NomineeSeam68,seam.data(),10)==0,"native flags store and call restored after disable");
#endif
 std::printf("NOMINEE_LOS_JOINED %u/%u checks; native packet leaf calls=%u; controlled engine query, no headset proof\n",assertions68-failures68,assertions68,nativeCalls68);return failures68?2:0;
}
#else
// Executes the actual production selector/bounds/facing wrappers. Only retail
// leaves and copied bridge endpoints are doubled; the selector flow is not a
// reimplementation. No game process is opened.
#define NOMINMAX
#include <windows.h>
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <array>
#include <optional>
#include <utility>
#include "../../../src/subsystems/alchemy/native_selection68.h"
using Table68=kcd2vr::subsystems::alchemy::bridge67::NativeTable;
using Ref68=kcd2vr::subsystems::alchemy::near67::Ref;
using Bounds68=kcd2vr::subsystems::alchemy::geometry68::Bounds;
static std::uint64_t now68=1000;
static std::uintptr_t ret68=0;
static const std::uintptr_t g_wh_base=0x180000000ull;
static std::atomic<bool> g_enabled{true};
static std::atomic<bool> g_split_ready57c1{true};
static std::uint64_t Tick68(){return now68;}
static Table68 current68{};
static bool tableOn68=true,nearOn68=true,mutateBounds68=false;
static std::uint32_t nearId68=100;
static unsigned offers68=0,selectorCalls68=0,boundsCalls68=0,facingCalls68=0;
static unsigned checks68=0;
static void ck(bool value,const char* text){++checks68;if(!value){std::fprintf(stderr,"FAIL %u %s\n",checks68,text);std::exit(2);}}
static bool pins68=true,retail68=true,module68=true,disableFail68=false;
static unsigned create68=0,pinCalls68=0,enable68=0,disable68=0,failCreate68=0,failEnable68=0;
static bool MatchEntry(std::uintptr_t,const unsigned char*,std::size_t){return pins68;}
namespace retail_build112 {static bool Ready(std::uintptr_t){return retail68;}}
static BOOL FixtureModule68(DWORD flags,LPCWSTR,HMODULE* out){++pinCalls68;ck((flags&GET_MODULE_HANDLE_EX_FLAG_PIN)!=0,"callback module is pinned");*out=module68?reinterpret_cast<HMODULE>(1):nullptr;return module68;}
static std::uintptr_t __fastcall OriginalSelector68(std::uintptr_t,std::uintptr_t,std::uintptr_t,std::uintptr_t,unsigned);
static std::uintptr_t __fastcall OriginalBounds68(std::uintptr_t,std::uintptr_t);
static char __fastcall OriginalFacing68(std::uintptr_t,std::uintptr_t,std::uintptr_t);
namespace DetourModKit {
struct Address{std::uintptr_t value;};
namespace hook {
struct Options{bool fail_if_already_hooked=false;};struct Target{Address address;};
struct InlineRequest{const char* name;Target target;Options options;};
struct Hook{
    unsigned id=0;bool active=false;
    template<class T>T original(){return id==1?reinterpret_cast<T>(&OriginalSelector68):id==2?reinterpret_cast<T>(&OriginalBounds68):reinterpret_cast<T>(&OriginalFacing68);}
    bool enable(){++enable68;ck(pinCalls68!=0,"module pin precedes hook enable");if(failEnable68==id)return false;active=true;return true;}
    bool is_enabled()const{return active;}
    bool disable(){++disable68;if(disableFail68)return false;active=false;return true;}
};
template<class F>std::optional<Hook> inline_at(InlineRequest request,F){
    ck(request.options.fail_if_already_hooked,"foreign hook collision refuses");++create68;
    if(create68==failCreate68)return std::nullopt;return Hook{create68,false};
}
}}
namespace kcd2vr::subsystems::alchemy::bridge67 {
static bool FixtureTable68(NativeTable& t,std::uint64_t now){if(!tableOn68||now<current68.tick||now-current68.tick>80)return false;t=current68;return true;}
static void FixtureBounds68(const NativeTable& t,near67::Ref r,const geometry68::Bounds& b,std::uint64_t){
    ck(t.instance==current68.instance&&r.entity==100&&geometry68::valid(b),"exact observed native table/bounds");++offers68;
}
static bool FixtureNear68(const NativeTable& t,std::uint32_t& id,std::uint64_t now){
    NativeTable current{};
    if(!nearOn68||!FixtureTable68(current,now)||t.instance!=current.instance||t.lifetime!=current.lifetime)return false;id=nearId68;return true;
}
}
#define _ReturnAddress() reinterpret_cast<void*>(ret68)
#define GetTickCount64 Tick68
#define selectionTable68 FixtureTable68
#define selectionBounds68 FixtureBounds68
#define selectionNear68 FixtureNear68
#define GetModuleHandleExW FixtureModule68
#include "../../../src/interaction_native_selection68.inl"
#undef GetModuleHandleExW
#undef selectionNear68
#undef selectionBounds68
#undef selectionTable68
#undef GetTickCount64
#undef _ReturnAddress

static std::array<unsigned char,0xB20> player68{};
static std::array<unsigned char,0x100> interactor68{};
static std::array<unsigned char,0xB0> henryEntity68{},tool68{},other68{};
static std::array<unsigned char,0x28> klass68{},policy68{};
static std::array<unsigned char,48> policies68{};
template<class T>static void put(void* where,std::size_t offset,T value){std::memcpy(static_cast<unsigned char*>(where)+offset,&value,sizeof(value));}
template<class T>static std::uintptr_t addr(T& v){return reinterpret_cast<std::uintptr_t>(v.data());}
static float local68[6]{-.1f,-.2f,-.3f,.1f,.2f,.3f};
static char facingResult68=0;
static void(*body68)()=nullptr;
static std::uintptr_t seen68[4]{};static unsigned seenKey68=0;
static std::uintptr_t __fastcall OriginalSelector68(std::uintptr_t a,std::uintptr_t b,std::uintptr_t c,std::uintptr_t d,unsigned key){
    ++selectorCalls68;seen68[0]=a;seen68[1]=b;seen68[2]=c;seen68[3]=d;seenKey68=key;if(body68)body68();return 0x87654321ABCDEF01ull;
}
static std::uintptr_t __fastcall OriginalBounds68(std::uintptr_t,std::uintptr_t out){
    ++boundsCalls68;if(out>4096)std::memcpy(reinterpret_cast<void*>(out),local68,sizeof(local68));
    if(mutateBounds68)++current68.lifetime;return 0xABCDEF1020304050ull;
}
static char __fastcall OriginalFacing68(std::uintptr_t,std::uintptr_t,std::uintptr_t){++facingCalls68;return facingResult68;}
static std::uintptr_t callSelector68(unsigned key=1){ret68=g_wh_base+0x7F5727;return SelectorDetour68(addr(interactor68),0x2222,0x3333,0x4444,key);}
static std::uintptr_t callBounds68(std::uintptr_t entity=0,std::uintptr_t caller=0){
    float bounds[6]{};ret68=caller?caller:g_wh_base+0x63FF4A;return BoundsDetour68(entity?entity:addr(tool68),reinterpret_cast<std::uintptr_t>(bounds));
}
static char callFacing68(std::uintptr_t owner=0,std::uintptr_t caller=0){
    ret68=caller?caller:g_wh_base+0x64032E;return FacingDetour68(owner?owner:addr(interactor68),0x5555,0x6666);
}
static void reset68(){
    player68={};interactor68={};henryEntity68={};tool68={};other68={};klass68={};policy68={};policies68={};
    put(player68.data(),0,g_wh_base+0x3AA4B98);put(player68.data(),0x38,addr(henryEntity68));put(player68.data(),0xB08,addr(interactor68));
    put(interactor68.data(),0,g_wh_base+0x3A6A0C8);put(interactor68.data(),8,addr(player68));
    put(interactor68.data(),0xD8,addr(policies68));put(interactor68.data(),0xE0,std::uintptr_t(3));
    put(policies68.data(),0,1u);put(policies68.data(),8,addr(policy68));put(policies68.data(),16,2u);put(policies68.data(),32,4u);
    put(policy68.data(),0,g_wh_base+0x3A69D90);put(policy68.data(),8,addr(player68));
    put(henryEntity68.data(),0,g_wh_base+0x4072C78);
    put(klass68.data(),0,g_wh_base+0x3B282A0);put(klass68.data(),0x10,reinterpret_cast<std::uintptr_t>("AlchemyItem"));
    for(auto* obj:{tool68.data(),other68.data()}){
        put(obj,0,g_wh_base+0x4072C78);put(obj,0x20,addr(klass68));
        const float matrix[12]{1,0,0,10,0,1,0,20,0,0,1,30};std::memcpy(obj+0x58,matrix,sizeof(matrix));
    }
    put(tool68.data(),0xC,100u);put(other68.data(),0xC,101u);
    current68={};current68.instance=0x123456;current68.player=addr(player68);current68.playerEntity=addr(henryEntity68);
    current68.lifetime=7;current68.sequence=9;current68.tick=now68=1000;current68.refs[0]={0,100};current68.refs[1]={1,101};
    tableOn68=nearOn68=true;nearId68=100;facingResult68=0;mutateBounds68=false;body68=nullptr;
    selectorCalls68=boundsCalls68=facingCalls68=offers68=0;g_enabled=true;g_selection_ready68=true;g_selection_scope68={};g_selection_depth68=0;
    g_selector_original68=OriginalSelector68;g_bounds_original68=OriginalBounds68;g_facing_original68=OriginalFacing68;
}
static void installationReset68(){
    reset68();g_selection_ready68=false;g_selection_retired68=false;g_selector_hook68.reset();g_bounds_hook68.reset();g_facing_hook68.reset();
    pins68=retail68=module68=true;disableFail68=false;create68=pinCalls68=enable68=disable68=failCreate68=failEnable68=0;g_split_ready57c1=true;
}
static void positive68(){
    ck(g_selection_scope68.active&&g_selection_depth68==1,"scope proven inside exact selector");
    ck(callBounds68()==0xABCDEF1020304050ull,"bounds raw return forwarded");
    ck(offers68==1&&g_selection_scope68.candidate.ref.entity==100,"one native enumerated offer");
    ck(callFacing68()==1,"near candidate survives native head-facing false");
    ck(FilterNativeCandidate68(addr(interactor68),100,1,now68)==1,"nominated native true preserved");
    ck(FilterNativeCandidate68(addr(interactor68),101,1,now68)==0,"other registered candidate refused");
    ck(FilterNativeCandidate68(addr(interactor68),100,0,now68)==0,"native exclusion false never promoted");
    ck(FilterNativeCandidate68(addr(interactor68),999,1,now68)==1,"unrelated non-table unchanged");
}
static void negative68(){callBounds68();ck(!offers68,"scope refused before capture");ck(callFacing68()==0,"native facing false unchanged outside qualified scope");}
static void nestedBody68(){callBounds68();ck(!g_selection_scope68.active,"nested context masked");ck(callFacing68()==0,"nested cannot borrow ancestor candidate");}
static void outerNested68(){
    callBounds68();body68=nestedBody68;(void)callSelector68();body68=outerNested68;
    ck(g_selection_scope68.active&&g_selection_scope68.candidate.ref.entity==100,"outer exact candidate restored");ck(callFacing68()==1,"outer restored context remains qualified");
}
static void throwBody68(){RaiseException(0xE0446868,0,0,nullptr);}
static bool exception68(){bool caught=false;__try{(void)callSelector68();}__except(EXCEPTION_EXECUTE_HANDLER){caught=true;}return caught;}
int main(){
    reset68();body68=positive68;ck(callSelector68()==0x87654321ABCDEF01ull,"selector native return bits");
    ck(selectorCalls68==1&&boundsCalls68==1&&facingCalls68==1,"each original called exactly once");
    ck(seen68[0]==addr(interactor68)&&seen68[1]==0x2222&&seen68[2]==0x3333&&seen68[3]==0x4444&&seenKey68==1,"all native arguments untouched");
    ck(!g_selection_scope68.active&&g_selection_depth68==0,"scope cannot leak after selector");
    for(unsigned key:{2u,4u,8u}){reset68();body68=negative68;(void)callSelector68(key);ck(selectorCalls68==1,"other policy original once");}
    reset68();g_selection_ready68=false;body68=negative68;(void)callSelector68();
    reset68();g_enabled=false;body68=negative68;(void)callSelector68();
    reset68();tableOn68=false;body68=negative68;(void)callSelector68();
    reset68();now68=1081;body68=negative68;(void)callSelector68();
    reset68();put(policy68.data(),8,std::uintptr_t(1));body68=negative68;(void)callSelector68();
    reset68();put(policy68.data(),0,g_wh_base+0x3A69E48);body68=negative68;(void)callSelector68();
    reset68();put(player68.data(),0xB08,std::uintptr_t(1));body68=negative68;(void)callSelector68();
#if !defined(SELECTION68_ASAN)
    reset68();put(interactor68.data(),0xD8,std::uintptr_t(1));body68=negative68;(void)callSelector68();
#endif
    reset68();body68=[](){(void)callBounds68(0,g_wh_base+0x63FF4B);ck(!offers68,"wrong bounds caller unchanged");ck(callFacing68()==0,"wrong bounds caller cannot nominate");};(void)callSelector68();
    reset68();body68=[](){put(klass68.data(),0x10,reinterpret_cast<std::uintptr_t>("NPC"));callBounds68();ck(!offers68,"wrong class cannot offer");};(void)callSelector68();
    reset68();body68=[](){put(tool68.data(),0xC,999u);callBounds68();ck(!offers68,"unregistered entity cannot offer");};(void)callSelector68();
    reset68();body68=[](){mutateBounds68=true;callBounds68();ck(!offers68,"native callback context mutation refuses");};(void)callSelector68();
    reset68();body68=[](){callBounds68();put(tool68.data(),0xC,101u);ck(callFacing68()==0,"changed candidate identity refuses near override");};(void)callSelector68();
    reset68();body68=[](){callBounds68();nearOn68=false;ck(callFacing68()==0,"retired source nomination refuses");ck(FilterNativeCandidate68(addr(interactor68),101,1,now68)==1,"retired nomination restores original candidate filter");};(void)callSelector68();
    reset68();body68=[](){callBounds68();nearId68=101;ck(callFacing68()==0,"other same-table nomination cannot borrow candidate");};(void)callSelector68();
    reset68();body68=[](){callBounds68();ck(callFacing68(addr(player68))==0,"foreign interactor refuses");ck(callFacing68(0,g_wh_base+0x64032F)==0,"foreign facing caller refuses");facingResult68=7;ck(callFacing68()==7,"native true result preserved byte-exact");};(void)callSelector68();
    reset68();body68=[](){callBounds68();(void)callBounds68(addr(henryEntity68));ck(!g_selection_scope68.candidate.valid,"failed new candidate clears old witness");ck(callFacing68()==0,"failed new candidate cannot inherit old near");};(void)callSelector68();
    reset68();body68=outerNested68;(void)callSelector68();ck(selectorCalls68==2&&offers68==1,"nested selector original once but no second offer");
    reset68();body68=throwBody68;ck(exception68(),"original exception preserved");ck(!g_selection_scope68.active&&g_selection_depth68==0,"SEH unwind retires selector TLS");
    installationReset68();ck(InstallNativeSelection68(g_wh_base)&&NativeSelectionReady68(),"complete pinned install ready");
    ck(create68==3&&enable68==3,"all three original-once hooks installed");
    ck(InstallNativeSelection68(g_wh_base)&&create68==3,"ready install idempotent");
    StopNativeSelection68();ck(!NativeSelectionReady68()&&g_selection_retired68,"stop revokes before disabling");
    ck(g_selector_hook68&&g_bounds_hook68&&g_facing_hook68&&g_selector_original68&&g_bounds_original68&&g_facing_original68,"teardown retains all trampolines and originals");
    ck(!InstallNativeSelection68(g_wh_base),"retired installation cannot silently rearm");body68=negative68;(void)callSelector68();
    installationReset68();pins68=false;ck(!InstallNativeSelection68(g_wh_base)&&!create68&&!pinCalls68,"bad byte pin no hook/no module mutation");
    installationReset68();retail68=false;ck(!InstallNativeSelection68(g_wh_base)&&!create68,"wrong retail fingerprint no hook");
    installationReset68();g_split_ready57c1=false;ck(!InstallNativeSelection68(g_wh_base)&&!create68,"old selector unavailable no hook");
    installationReset68();module68=false;ck(!InstallNativeSelection68(g_wh_base)&&!create68,"cannot pin callback module no hook");
    for(unsigned step:{1u,2u,3u}){installationReset68();failCreate68=step;ck(!InstallNativeSelection68(g_wh_base)&&!NativeSelectionReady68(),"partial create refuses authority");if(step>1)ck(g_selection_retired68&&g_selector_hook68&&g_selector_original68,"partial create retains callback lifetime");}
    for(unsigned step:{1u,2u,3u}){installationReset68();failEnable68=step;disableFail68=true;
        ck(!InstallNativeSelection68(g_wh_base)&&!NativeSelectionReady68()&&g_selection_retired68,"partial enable + failed disable remains retired");
        ck(g_selector_hook68&&g_bounds_hook68&&g_facing_hook68&&g_selector_original68&&g_bounds_original68&&g_facing_original68,"failed disable retains executable callback originals");
        body68=negative68;(void)callSelector68();}
    std::printf("native_selection68 PASS %u checks\n",checks68);return 0;
}

#endif
