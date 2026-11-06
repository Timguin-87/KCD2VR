from pathlib import Path
import subprocess,json,hashlib,sys
from m41_test_support import options,compile_and_run
args=options('presenter');ROOT=args.source_root;BASE=ROOT;OUT=args.out
control=args.control_no_model_gate

def function(source,name):
 a=source.index(name);b=source.index('{',a);d=1;e=b+1
 while d:d+=(source[e]=='{')-(source[e]=='}');e+=1
 return source[a:e]
presenter=(ROOT/'src/subsystems/vrmap/runtime/held_map_m1.cpp').read_text()
feed=(ROOT/'src/subsystems/vrmap/runtime/held_map_feed_t2.cpp').read_text()
actual_model=(function(presenter,'bool FilenameMatchesM43(')+'\n'+function(presenter,'bool StaticModelReadyRawD72(')).replace('StaticModelReadyRawD72','StaticModelReadyM41')
actual_frame=function(presenter,'void NativeFrameTick() noexcept')
actual_shutdown=function(presenter,'void Shutdown() noexcept')
if control:
 assert '!StaticModelReadyM41(entity,expected)'in actual_frame
 actual_frame=actual_frame.replace('!StaticModelReadyM41(entity,expected)','false',1)
actual_ack=function(feed,'bool StaticModelM41(')
prefix=r'''
#define NOMINMAX
#define GetTickCount64 WinGetTickCount64
#include <windows.h>
#undef GetTickCount64
#include <atomic>
#include <cassert>
#include <cstdio>
#include <cstring>
#include "subsystems/vrmap/runtime/held_map_m1.h"
#include "held_map_pose_m1.h"
#include "subsystems/vrmap/paper_grip_d64.h"
namespace paper_grip_d64=kcd2vr::paper_grip_d64;
namespace map_geometry_d64=kcd2vr::map_geometry_d64;
#include "subsystems/vrmap/static_paper_m41.h"
#include "subsystems/vrmap/native_paper_m43.h"
namespace native_paper_m43=kcd2vr::native_paper_m43;
namespace diagnostics {namespace perf_d61 {struct Scope{template<class T>Scope(T){}};inline constexpr int MapTick=0,MapPose=1;}}
using namespace kcd2vr::held_map_m1;
static std::uint64_t now=1000;static std::uint64_t GetTickCount64(){return now;}
static unsigned checks=0,failures=0;
static void Check(bool b,const char* why){++checks;if(!b){++failures;std::printf("FAIL %s\n",why);}}
namespace diagnostic_trace {static unsigned calls=0;static void Write(const char*){++calls;}}
static std::uintptr_t base=0x100000;
alignas(16) static unsigned char actorEntity[64]{},helperEntity[64]{};
alignas(16) static unsigned char staticObject[64]{},secondStaticObject[64]{};
static bool paperVisible=false;
static bool helper=true,hidden=true,hideFault=false,placeFault=false,tracked=true,slotAvailable=true,slotChanged=false;
static unsigned slotCalls=0,placeCalls=0,rightReads=0,requestCalls=0,missingCalls=0;
static const char* filename="Objects/kcd2vr/staticmap40/trosecko.cgf";
static float placedPose[8]{};static bool loseAfterModel=false,stallPlace=false,bodyAfterModel=false,changedBody=false,occupyAfterPlace=false;static unsigned hideCalls=0;
static bool replaceInModel=false,replaceInPlace=false,replaceVmInModel=false;static unsigned foreignPlace=0,foreignHide=0;
static bool Copy(std::uintptr_t from,void* to,std::size_t n){if(!from)return false;std::memcpy(to,reinterpret_cast<void*>(from),n);return true;}
static void* StaticSlotM41(void*){++slotCalls;return !slotAvailable?nullptr:(slotChanged&&slotCalls%2==0?secondStaticObject:staticObject);}
static const char* StaticFilenameM41(void*);
namespace kcd2vr::held_map_feed_t2 {
struct ReadinessAck {std::uint64_t activation=~0ull,epoch=0,vm=0,serial=0;std::uint32_t actor=0,helper=0;char artwork[64]{};bool native=false;};
static kcd2vr::core::SnapshotMailbox<ReadinessAck> readinessAck;
static std::atomic<bool> ready{true};
static std::atomic<std::uint64_t> readinessVm{1},missingSerial{0};
static void RequestReadiness(std::uint32_t,std::uint64_t,std::uint64_t,std::uint64_t,std::uint32_t=0){++requestCalls;}
static bool ReportHelperMissing(std::uint32_t,std::uint64_t,std::uint64_t){++missingCalls;return true;}
'''
middle=r'''
}
namespace held_map_feed_t2=kcd2vr::held_map_feed_t2;
static const char* StaticFilenameM41(void*){
 if(replaceInModel){const unsigned foreign=2042;std::memcpy(helperEntity+0xC,&foreign,4);}
 if(replaceVmInModel)held_map_feed_t2::readinessVm=2;
 if(loseAfterModel)tracked=false;if(bodyAfterModel)changedBody=true;return filename;
}
namespace kcd2vr::frik_pose_probe {
struct HeldMapPairM39 {};
static bool ReadHeldMapSupportLeftD64(float(&)[8],float&,const HeldMapPairM39&){return false;}
static bool ReadHeldMapRightPoseM41(float(&hand)[8],unsigned* refusal,std::uintptr_t* source,std::uint64_t* epoch,std::uint64_t* tick,HeldMapPairM39* pair=nullptr){
 ++rightReads;if(!tracked){*refusal=7;return false;}
 float p[8]{0,0,0,1,40,0,0,1};std::memcpy(hand,p,sizeof(p));
 *source=reinterpret_cast<std::uintptr_t>(actorEntity);*epoch=changedBody?8:7;*tick=now;*refusal=0;return true;
}
struct HeldMapSnapshotD78R1 {
 float right[8]{},left[8]{},leftGrip=0;bool haveLeft=false;
 std::uintptr_t sourceEntity=0;std::uint64_t sourceEpoch=0,sourceTick=0;HeldMapPairM39 pair{};
};
static bool ReadHeldMapSnapshotD78R1(HeldMapSnapshotD78R1& out,bool,unsigned* refusal){
 out={};return ReadHeldMapRightPoseM41(out.right,refusal,&out.sourceEntity,&out.sourceEpoch,&out.sourceTick,&out.pair);
}
}
namespace frik_pose_probe=kcd2vr::frik_pose_probe;
namespace held_map_pose_m1=kcd2vr::held_map_pose_m1;
static std::atomic<bool> ready{true},shown{false};
static std::atomic<std::uint64_t> placed{0},showN{0},hideN{0},missingN{0},staleN{0};
static std::atomic<unsigned> poseRefusal19c3{0},lastHideReasonM4{0};
static std::atomic_flag busy=ATOMIC_FLAG_INIT;
static void* Find(PrintedRegion,unsigned* why){*why=helper?0:3;return helper?helperEntity:nullptr;}
static bool Hidden(void*,bool& value){value=hidden;return true;}
static bool Hide(void*,bool value){unsigned id=0;std::memcpy(&id,helperEntity+0xC,4);if(id!=1042)++foreignHide;++hideCalls;if(hideFault)return false;hidden=value;return true;}
static void CacheHelperM43(void*,std::uint32_t){}
static void PumpPaperM43(){}
static bool PaperCleanupCompleteM43(){return true;}
static unsigned hidePaperCalls=0;
static void HidePaperM43(){++hidePaperCalls;paperVisible=false;}
static bool Place(void*,const float* p,unsigned& why){unsigned id=0;std::memcpy(&id,helperEntity+0xC,4);if(id!=1042)++foreignPlace;
 if(replaceInPlace){const unsigned foreign=2042;std::memcpy(helperEntity+0xC,&foreign,4);}++placeCalls;why=placeFault?2:0;if(placeFault)return false;std::memcpy(placedPose,p,sizeof(placedPose));if(stallPlace)now+=81;if(occupyAfterPlace)tracked=false;return true;}
static bool PublishPaperM43(void* helper,unsigned id,std::uintptr_t actor,unsigned actorId,std::uint64_t epoch,std::uint64_t serial,std::uint64_t tick,const float* pose,const char* expected){
 unsigned why=0;if(!Place(helper,pose,why))return false;paperVisible=true;return true;
}
static void Ack(const char* art="trosecko"){
 held_map_feed_t2::ReadinessAck a{activation.load(),7,1,1,42,1042};a.native=NativePaperSelectedM43();std::snprintf(a.artwork,sizeof(a.artwork),"%s",art);
 assert(held_map_feed_t2::readinessAck.publish(a));
}
static void Reset(){
 now+=2000;Enable(false);Enable(true);SetNativePaperM43(false);SetRequested31(true,now);PublishAllowed(true,now);
 replaceInModel=replaceInPlace=replaceVmInModel=false;foreignPlace=foreignHide=0;
 helper=true;hidden=true;hideFault=false;placeFault=false;tracked=true;slotAvailable=true;slotChanged=false;
 hidePaperCalls=0;paperVisible=false;shown=false;loseAfterModel=false;stallPlace=false;bodyAfterModel=false;changedBody=false;occupyAfterPlace=false;hideCalls=0;slotCalls=placeCalls=rightReads=requestCalls=missingCalls=0;
 filename="Objects/kcd2vr/staticmap40/trosecko.cgf";held_map_feed_t2::readinessVm=1;held_map_feed_t2::missingSerial=0;
 const auto actorVft=base+0x4072C78,statVft=base+0x3A2F888;const unsigned id=42,helperId=1042;
 std::memcpy(helperEntity,&actorVft,8);std::memcpy(helperEntity+0xC,&helperId,4);std::memcpy(actorEntity,&actorVft,8);std::memcpy(actorEntity+0xC,&id,4);std::memcpy(staticObject,&statVft,8);
 assert(held_map_feed_t2::readinessAck.publish({}));
}
'''
test=r'''
int main(){
 Reset();char expected[160]{};
 Check(!held_map_feed_t2::StaticModelM41(42,7,activation.load(),1042,expected),"spawn/readiness absent cannot reveal");
 Ack();Check(held_map_feed_t2::StaticModelM41(42,7,activation.load(),1042,expected)&&!std::strcmp(expected,"objects/kcd2vr/staticmap40/trosecko.cgf"),"current ACK joins exact baked asset");
 nativePaperM43.store(true);Check(!held_map_feed_t2::StaticModelM41(42,7,activation.load(),1042,expected),"WORLD ACK cannot grant native renderer even with unchanged activation");nativePaperM43.store(false);
 Check(!held_map_feed_t2::StaticModelM41(43,7,activation.load(),1042,expected),"actor mismatch");
 Check(!held_map_feed_t2::StaticModelM41(42,8,activation.load(),1042,expected),"body mismatch");
 Check(!held_map_feed_t2::StaticModelM41(42,7,activation.load()+1,1042,expected),"activation mismatch");
 held_map_feed_t2::readinessVm=2;Check(!held_map_feed_t2::StaticModelM41(42,7,activation.load(),1042,expected),"replaced VM");held_map_feed_t2::readinessVm=1;
 held_map_feed_t2::missingSerial=1;Check(!held_map_feed_t2::StaticModelM41(42,7,activation.load(),1042,expected),"missing helper invalidates former receipt");held_map_feed_t2::missingSerial=0;
 Ack("unknown");Check(!held_map_feed_t2::StaticModelM41(42,7,activation.load(),1042,expected),"unknown artwork rejected");Ack();
 Check(StaticModelReadyM41(actorEntity,"objects/kcd2vr/staticmap40/trosecko.cgf"),"actual native loaded filename normalized");
 filename="objects/kcd2vr/map32/trosky.cdf";Check(!StaticModelReadyM41(actorEntity,"objects/kcd2vr/staticmap40/trosecko.cgf"),"old CHR/UI route rejected");
 filename="objects/kcd2vr/staticmap40/kutnohorsko.cgf";Check(!StaticModelReadyM41(actorEntity,"objects/kcd2vr/staticmap40/trosecko.cgf"),"cached wrong region art rejected");
 filename="objects/kcd2vr/staticmap40/trosecko.cgf";slotAvailable=false;Check(!StaticModelReadyM41(actorEntity,filename),"asynchronous static slot pending");slotAvailable=true;
 slotChanged=true;slotCalls=0;Check(!StaticModelReadyM41(actorEntity,filename),"model replaced during readback");slotChanged=false;
 const auto old=base+0x3A2F888,wrong=base+1;std::memcpy(staticObject,&wrong,8);Check(!StaticModelReadyM41(actorEntity,filename),"wrong native stat object class");std::memcpy(staticObject,&old,8);
 Reset();NativeFrameTick();Check(hidden&&!shown&&placeCalls==0,"no ACK remains hidden");
 now+=201;PublishAllowed(true,now);Ack();filename="objects/kcd2vr/map32/trosky.cdf";NativeFrameTick();Check(hidden&&!shown&&placeCalls==0,"no old material helper reveal");
 now+=201;PublishAllowed(true,now);filename="objects/kcd2vr/staticmap40/trosecko.cgf";NativeFrameTick();Check(!hidden&&!paperVisible&&shown&&placeCalls==1&&placedPose[4]>39&&placedPose[4]<41,"loaded static artwork shown at RIGHT socket");
 SetRequested31(false,now);NativeFrameTick();Check(hidden&&!shown,"B close hides immediately");
 Reset();Ack();NativeFrameTick();tracked=false;NativeFrameTick();Check(hidden&&!shown,"stale/right tracking loss hides, no frozen paper");
 Reset();Ack();NativeFrameTick();SetRequested31(false,now);hideFault=true;NativeFrameTick();Check(shown&&!hidden,"failed native hide retains obligation after close");
 hideFault=false;NativeFrameTick();Check(!shown&&hidden&&!paperVisible,"close retries helper hide debt");
 Reset();Ack();NativeFrameTick();helper=false;NativeFrameTick();Check(!shown&&missingCalls==1,"actual helper absent invalidates ready receipt");
 Reset();Ack();placeFault=true;NativeFrameTick();Check(hidden&&!shown,"failed placement never reveals");
 Reset();Ack();NativeFrameTick();held_map_feed_t2::readinessVm=2;NativeFrameTick();Check(hidden&&!shown,"VM replacement hides prior paper");
 Reset();Ack();NativeFrameTick();PublishAllowed(false,now);NativeFrameTick();Check(hidden&&!shown,"menu/native owner refusal hides");
 Reset();Ack();loseAfterModel=true;NativeFrameTick();Check(hidden&&!shown&&placeCalls==0,"RIGHT owner lost during model read never places");
 Reset();Ack();stallPlace=true;NativeFrameTick();Check(hidden&&!shown,"native placement stall cannot reveal old pose");
 Reset();Ack();const unsigned foreignId=2042;std::memcpy(helperEntity+0xC,&foreignId,4);NativeFrameTick();Check(hidden&&!shown&&placeCalls==0&&hideCalls==0,"foreign same-name model ID cannot consume old ACK");
 Reset();Ack();bodyAfterModel=true;NativeFrameTick();Check(hidden&&!shown&&placeCalls==0,"body changed during model lookup cannot place");
 Reset();Ack();occupyAfterPlace=true;NativeFrameTick();Check(hidden&&!shown,"RIGHT owner loss after placement prevents reveal");
 // These substitutions establish missing revalidation, not a live retail race.
 Reset();Ack();replaceInModel=true;NativeFrameTick();
 Check(foreignPlace==0&&foreignHide==0,"helper recycled during model read: no foreign pose/show/hide write");
 Reset();Ack();replaceInPlace=true;NativeFrameTick();
 Check(foreignHide==0,"helper recycled before Place returned: no foreign reveal/hide");
 Reset();Ack();replaceVmInModel=true;NativeFrameTick();
 Check(placeCalls==0&&hidden&&!shown,"VM replaced during model read: no stale ACK place/reveal");
 Reset();Ack();NativeFrameTick();native_paper_m43::visible=true;
 const auto beforeShutdownHide=hidePaperCalls,beforeShutdownPlace=placeCalls,beforeShutdownWorld=hideCalls;
 ::Shutdown();Check(!Requested()&&!Allowed(now)&&!native_paper_m43::visible&&native_paper_m43::retireRequested&&
  hidePaperCalls==beforeShutdownHide&&placeCalls==beforeShutdownPlace&&hideCalls==beforeShutdownWorld,"loader-lock shutdown is scalar request only, not native cleanup");
 std::printf("M41 WORLD actual model/ACK/presenter checks=%u failures=%u nativeLeaves=SUBSTITUTED\n",checks,failures);
 return failures?1:0;
}
'''
test=test.replace('NativeFrameTick();','::NativeFrameTick();')
cpp=prefix+actual_ack+middle+actual_model+'\n'+actual_frame+'\n'+actual_shutdown+'\n'+test
(OUT/'presenter.cpp').write_text(cpp)
p,log=compile_and_run(args,OUT/'presenter.cpp','presenter',[ROOT/'src'])
print(log[-3000:]);assert (p.returncode!=0 and 'FAIL'in log)if control else(p.returncode==0 and 'failures=0'in log),(p.returncode,log)
(OUT/'RESULT.json').write_text(json.dumps(dict(exit=p.returncode,result=log,expected_control_failure=control,actual_model_sha256=hashlib.sha256(actual_model.encode()).hexdigest(),actual_ack_sha256=hashlib.sha256(actual_ack.encode()).hexdigest(),actual_frame_sha256=hashlib.sha256(actual_frame.encode()).hexdigest(),native_leaves='SUBSTITUTED',headset='NOT_RUN'),indent=2))
