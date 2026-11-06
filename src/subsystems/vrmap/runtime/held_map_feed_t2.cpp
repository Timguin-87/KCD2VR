#include "held_map_feed_t2.h"
#include "held_map_m1.h"
#include "native_script_t2.h"
#include "core/snapshot_mailbox.h"
#include "retail_build112.h"
#include "diagnostic_trace.h"
#include "diagnostics/capture168.h"
#include "native_map_live_h28.h"
#include "activity_input163.h"
#include "map_diagnostics_d57.h"
#include "../static_paper_m41.h"
#include "../quest_packet_d75.h"
#include <windows.h>
#include <atomic>
#include <cstdio>
#include <cstring>

namespace kcd2vr::held_map_feed_t2 {namespace {
std::atomic<bool> ready{false};
std::atomic<std::uint64_t> desiredUntil{0};
std::atomic<bool> clearPending{false};
core::SnapshotMailbox<Snapshot> snapshot;
struct ReadinessRequest {std::uint64_t tick=0,activation=0,epoch=0,recovery=0;std::uint32_t actor=0,helper=0;bool native=false;};
// An actual HELPERS_READY reply and an exhausted retry budget are different
// terminal facts. Only a successful reply grants a helper-loss recovery token.
struct ReadinessAck {std::uint64_t activation=~0ull,epoch=0,vm=0,serial=0;std::uint32_t actor=0,helper=0;char artwork[64]{};bool native=false;};
struct ReadinessTerminal {std::uint64_t activation=~0ull,epoch=0,vm=0,recovery=0;std::uint32_t actor=0;bool native=false;};
core::SnapshotMailbox<ReadinessRequest> readinessRequest;
core::SnapshotMailbox<ReadinessAck> readinessAck;
struct LetterAckD44 {std::uint64_t activation=~0ull,epoch=0,vm=0,helperSerial=0,tick=0;std::uint32_t actor=0;};
core::SnapshotMailbox<LetterAckD44> letterAckD44;
core::SnapshotMailbox<ReadinessTerminal> readinessTerminal;
std::atomic<std::uint64_t> readinessUntil{0};
std::atomic<std::uint64_t> readinessVm{0},readySerial{0},missingSerial{0};
std::uintptr_t base=0;
bool Wanted() noexcept {return held_map_m1::Requested()||held_map_m1::closeWork31.load(std::memory_order_acquire)||native_map_live_h28::Wanted()||native_map_open_h25::Pending()||readinessUntil.load(std::memory_order_acquire)!=0;}
#if defined(KCD2VR_MAP_T2_HOST_TEST)
bool HostRead(std::uintptr_t,void*,std::size_t) noexcept;
void HostFlag(void*,float*) noexcept;
#endif
struct Reader {
    bool operator()(std::uintptr_t from,void* to,std::size_t size) noexcept {
#if defined(KCD2VR_MAP_T2_HOST_TEST)
        return HostRead(from,to,size);
#else
        if(from<0x10000||!to||!size||from+size<from)return false;
        // D81: Capture alternates each bounded pointer-array row with its
        // pointed-to object. A single VirtualQuery-region cache therefore
        // missed twice per item on the 500ms quest refresh. Fault containment
        // and exact native identity/final registry joins remain authoritative.
        __try{std::memcpy(to,reinterpret_cast<void*>(from),size);return true;}
        __except(EXCEPTION_EXECUTE_HANDLER){return false;}
#endif
    }
};
bool Flag(std::uintptr_t apse,float (&xy)[2]) noexcept {
    Reader read;std::uintptr_t vt=0;Span flags{};unsigned count=0;
    if(!apse||!read(apse,&vt,8)||vt!=base+0x3ED9140||!read(apse+0x1A90,&flags,sizeof(flags))||!Count(flags,16,128,count))return false;
    // Getter DCB460 selects the CURRENT native level, never checkpoint[0].
    __try{
#if defined(KCD2VR_MAP_T2_HOST_TEST)
        HostFlag(reinterpret_cast<void*>(apse+0x1508),xy);
#else
        reinterpret_cast<void*(__fastcall*)(void*,float*)>(base+0x2AFEE18)(reinterpret_cast<void*>(apse+0x1508),xy);
#endif
    }
    __except(EXCEPTION_EXECUTE_HANDLER){ready.store(false,std::memory_order_release);return false;}
    return Coordinate(xy[0])&&Coordinate(xy[1]);
}
bool Encode(const NativeSnapshot& native,bool valid,bool waypoint,const float (&xy)[2],std::uint64_t activation,char (&out)[16384]) noexcept {
    if(native.count>native.markers.size())return false;
    const unsigned status=valid?(1u|(native.compassKnown?2u:0u)):0u;
    int used=std::snprintf(out,sizeof(out),"T2|%llu|%u|",static_cast<unsigned long long>(activation),status);
    if(waypoint){const int n=std::snprintf(out+used,sizeof(out)-used,"%.3f,%.3f",xy[0],xy[1]);if(n<0||std::size_t(n)>=sizeof(out)-used)return false;used+=n;}
    out[used++]='|';out[used]=0;
    if(valid)for(unsigned i=0;i<native.count;++i){const auto& m=native.markers[i];
        const int n=std::snprintf(out+used,sizeof(out)-used,"%d,%.3f,%.3f,%.3f,%llx;",m.nativeType,m.x,m.y,m.z,static_cast<unsigned long long>(m.id));
        if(n<0||std::size_t(n)>=sizeof(out)-used)return false;used+=n;
    }
    return true;
}
void ServiceReadiness(const native_script_t2::Context& context) noexcept {
    static ReadinessRequest key{};static std::uint64_t vm=0,due=0;static unsigned attempts=0;
    ReadinessRequest request{};const auto now=context.now;const auto until=readinessUntil.load(std::memory_order_acquire);
    if(!until)return;
    if(!ready.load(std::memory_order_acquire)||now>until||!held_map_m1::Requested()||!held_map_m1::Allowed(now)||
       !readinessRequest.read(request)||!request.actor||!request.epoch||!request.tick||now<request.tick||now-request.tick>80||
       !held_map_m1::RendererIntentMatches43({request.activation,true,request.native})){
        auto expected=until;(void)readinessUntil.compare_exchange_strong(expected,0,std::memory_order_acq_rel);return;}
    const bool changed=request.actor!=key.actor||request.epoch!=key.epoch||request.activation!=key.activation||
        request.recovery!=key.recovery||request.native!=key.native||vm!=context.generation;
    if(changed){key=request;vm=context.generation;due=0;attempts=0;}
    // One positive native helper ID may complete hidden creation immediately;
    // a helper change never resets the activation's finite attempt budget.
    if(request.helper&&request.helper!=key.helper){key.helper=request.helper;due=0;}
    if(now<due)return;
    due=now+500;
    if(attempts>=24){
        (void)readinessTerminal.publish({request.activation,request.epoch,vm,request.recovery,request.actor,request.native});
        readinessUntil.store(0,std::memory_order_release);return;}
    ++attempts;
    char input[128]{},reply[256]{},expectedPrefix[192]{};
    std::snprintf(input,sizeof(input),"%s|%llu|%llu|%u|%u",request.native?"R3N44":"R3",static_cast<unsigned long long>(request.activation),static_cast<unsigned long long>(request.epoch),request.actor,request.helper);
    const bool called=context.CallPacket("kcd2vr_map_t3_ready",input,reply,sizeof(reply));
    std::snprintf(expectedPrefix,sizeof(expectedPrefix),"%s|41.0-static-paper|%u|%llu|%llu|",request.native?"R3N44":"R3",request.actor,static_cast<unsigned long long>(request.epoch),static_cast<unsigned long long>(request.activation));
    const auto prefixLength=std::strlen(expectedPrefix);const char* tail=called&&std::strncmp(reply,expectedPrefix,prefixLength)==0?reply+prefixLength:nullptr;
    bool ack=false;char art[64]{};std::uint32_t helperId=0;
    if(tail){constexpr const char* prefix="1|1|HELPER_READY|";
        if(std::strncmp(tail,prefix,std::strlen(prefix))==0){const auto* start=tail+std::strlen(prefix);const auto* end=std::strchr(start,'|');
            if(end&&end>start&&std::size_t(end-start)<sizeof(art)){
                std::memcpy(art,start,std::size_t(end-start));const auto* number=end+1;const auto* last=number+std::strlen(number);
                const auto parsed=std::from_chars(number,last,helperId);
                ack=parsed.ec==std::errc{}&&parsed.ptr==last&&helperId&&helperId==request.helper&&static_paper_m41::valid(art);}}}
    // The VM callback may synchronously close/reopen the utility or change its
    // backend. Never promote an old reply into a new presentation lease.
    ReadinessRequest latest{};
    const auto afterNow=std::uint64_t(GetTickCount64());
    const bool sameRequest=context.Current()&&readinessVm.load()==context.generation&&
        held_map_m1::RendererIntentMatches43({request.activation,true,request.native})&&
        held_map_m1::Allowed(afterNow)&&readinessRequest.read(latest)&&
        latest.actor==request.actor&&latest.epoch==request.epoch&&latest.activation==request.activation&&
        latest.native==request.native&&latest.helper==request.helper&&latest.recovery==request.recovery&&
        latest.tick&&afterNow>=latest.tick&&afterNow-latest.tick<=80;
    if(!sameRequest)return; // a newer caller owns its own pending lease
    if(ack){
        const auto serial=readySerial.fetch_add(1,std::memory_order_acq_rel)+1;
        ReadinessAck current{request.activation,request.epoch,vm,serial,request.actor,helperId};
        std::snprintf(current.artwork,sizeof(current.artwork),"%s",art);current.native=request.native;
        (void)readinessAck.publish(current);
        readinessUntil.store(0,std::memory_order_release);
    }else if(attempts==24){
        (void)readinessTerminal.publish({request.activation,request.epoch,vm,request.recovery,request.actor,request.native});
        readinessUntil.store(0,std::memory_order_release);
    }
    if(ack||attempts==1||attempts==24){char message[448]{};
        std::snprintf(message,sizeof(message),"T5 MAP_READINESS actor=%u epoch=%llu activation=%llu attempt=%u/24 ack=%d outcome=%s recovery=%llu reply=%s selectedModel=SEPARATE_NATIVE_SLOT_RECEIPT",
            request.actor,static_cast<unsigned long long>(request.epoch),static_cast<unsigned long long>(request.activation),attempts,ack,
            ack?"HELPERS_READY":attempts==24?"RETRY_EXHAUSTED":"PENDING",static_cast<unsigned long long>(request.recovery),called?reply:"MISSING_PRIVATE_BOOTSTRAP_CALLBACK");
        diagnostic_trace::Write(message);}
}
#include "native_map_read_service_h25.inl"
#include "native_map_live_service_h28.inl"
// An explicit close outlives a transient feed/pose hide. It never creates a
// helper or opens Inventory, and no live navigation/hand lease is renewed here.
static void ServiceCloseM34(const native_script_t2::Context& context) noexcept {
    const auto current=held_map_m1::CloseTicket31();
    if(!current)return;
    static std::uint64_t serial=~0ull,vm=0,due=0;static unsigned attempts=0;
    if(current!=serial||vm!=context.generation){serial=current;vm=context.generation;due=0;attempts=0;}
    if(context.now<due||attempts>=120)return;
    due=context.now+33;++attempts;
    char command[96]{},answer[128]{},prefix[96]{};
    std::snprintf(command,sizeof(command),"I31|%llu|0",static_cast<unsigned long long>(serial));
    std::snprintf(prefix,sizeof(prefix),"I31|%llu|",static_cast<unsigned long long>(serial));
    const bool called=context.CallPacket("kcd2vr_map_m31_intent",command,answer,sizeof(answer));
    const char* phase=called&&std::strncmp(answer,prefix,std::strlen(prefix))==0?answer+std::strlen(prefix):"";
    const bool acknowledged=!std::strcmp(phase,"CLOSED")||!std::strcmp(phase,"FAULT");
    // A synchronous callback may publish a newer user intent. Never retire it.
    if(acknowledged||attempts==120)(void)held_map_m1::FinishClose31(serial,acknowledged,
        !std::strcmp(phase,"CLOSED")||!std::strcmp(phase,"FAULT"));
    // Exhaustion stops scheduler work but retains the unacknowledged obligation;
    // neither elapsed time nor failed observation is a CLOSED receipt.
    if(acknowledged||attempts==1||attempts==120){
        char message[320]{};std::snprintf(message,sizeof(message),"M34 MAP_CLOSE serial=%llu attempt=%u/120 ack=%d reply=%.120s liveFeed=NOT_REQUIRED inventory=NONE",
            static_cast<unsigned long long>(serial),attempts,int(acknowledged),called?answer:"CALLBACK_UNAVAILABLE");
        diagnostic_trace::Write(message);
    }
}
// D64: no world scans or texture rebakes. A bounded same-utility callback
// moves one player glyph; ordinary POIs are immutable mesh geometry in slot1.
void ServiceMarkersD64(const native_script_t2::Context& context) noexcept {
 static std::uint64_t due=0,serial=0,capturedAck=0;
 const auto now=context.now;const auto intent=held_map_m1::RendererIntent43();const auto activation=intent.serial;
 if(!held_map_m1::Requested()||!held_map_m1::Allowed(now)||!ready.load())return;
 if(serial!=activation){serial=activation;due=0;}if(now<due)return;due=now+200;
 ReadinessAck ack{};
 if(!readinessAck.read(ack)||ack.activation!=activation||ack.vm!=context.generation||!ack.actor||!ack.helper||!ack.epoch||ack.serial<=missingSerial.load()||ack.native!=intent.native||!held_map_m1::RendererIntentMatches43(intent))return;
 // Only the static world paper freezes ink; native-document behavior is unchanged.
 if(!intent.native&&capturedAck==ack.serial)return;
 char input[128]{},reply[64]{};
 std::snprintf(input,sizeof(input),"%s|%llu|%llu|%u|%u",ack.native?"D64N44":"D64",(unsigned long long)activation,(unsigned long long)ack.epoch,ack.actor,ack.helper);
 const bool called=context.CallPacket("kcd2vr_map_markers_d64",input,reply,sizeof(reply));
 ReadinessAck after{};
 if(!intent.native&&called&&std::strcmp(reply,"D64|PLAYER")==0&&context.Current()&&
    held_map_m1::RendererIntentMatches43(intent)&&readinessAck.read(after)&&
    after.serial==ack.serial&&ack.serial>missingSerial.load())capturedAck=ack.serial;
 if(diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,now)){
  static std::uint64_t next=0;if(now>=next){next=now+1000;char line[160]{};
   std::snprintf(line,sizeof(line),"D64 MAP_MARKERS actor=%u helper=%u reply=%.48s staticPoi=CACHED_MESH",ack.actor,ack.helper,called?reply:"UNAVAILABLE");diagnostic_trace::Write(line);}
 }
}
#include "quest_service_d75.inl"
void Service(const native_script_t2::Context& context) noexcept {
    // Independent ordinary native-document services retain their owner/lease.
    ServiceNativeDocumentH25(context);
    ServiceNativeLiveH28(context);
    // M41 static paper: reuse the original-first VM owner for event-bounded
    // bootstrap and close only. No private movie, compass scan or image redraw.
    readinessVm.store(context.generation,std::memory_order_release);
    ServiceReadiness(context);
    ServiceCloseM34(context);
    ServiceMarkersD64(context);
    ServiceQuestsD75(context);

}
}
bool Install() noexcept {
    if(ready.load(std::memory_order_acquire))return true;
    base=reinterpret_cast<std::uintptr_t>(GetModuleHandleW(L"WHGame.dll"));Reader read;unsigned char actual[14]{};
    const unsigned char flag[]{0x40,0x53,0x48,0x83,0xec,0x30,0x48,0x8b,0xda,0x48,0x8d,0x54,0x24,0x20};
    const bool ok=retail_build112::Verify(base)&&read(base+0x2AFEE18,actual,sizeof(actual))&&!std::memcmp(actual,flag,sizeof(flag))&&
        native_script_t2::Register(&Service,&Wanted)&&native_script_t2::Install();
    ready.store(ok,std::memory_order_release);return ok;
}
void NativeBookTick(std::uint64_t now,std::uintptr_t reading,std::uint64_t lifetime,bool allowed) noexcept {native_map_live_h28::Publish(now,reading,lifetime,allowed);}
void Tick(bool visible,std::uint64_t now) noexcept {
    // A native feed fault cannot become a per-frame Lua clear loop merely
    // because the separate paper scene remains visible. Retire its one active
    // edge, then stay idle until an explicit successful feed installation.
    const bool effective=visible&&ready.load(std::memory_order_acquire);
    const auto previous=desiredUntil.exchange(effective?now+250:0,std::memory_order_acq_rel);
    if(effective||previous)clearPending.store(true,std::memory_order_release);
}
void RequestReadiness(std::uint32_t actor,std::uint64_t epoch,std::uint64_t sourceTick,std::uint64_t now,std::uint32_t helperId) noexcept {
    const auto intent=held_map_m1::RendererIntent43();const auto activation=intent.serial;
    const auto vm=readinessVm.load(std::memory_order_acquire),recovery=missingSerial.load(std::memory_order_acquire);
    ReadinessAck ack{};ReadinessTerminal terminal{};
    if(!actor||!epoch||!sourceTick||now<sourceTick||now-sourceTick>80||!ready.load(std::memory_order_acquire)||!held_map_m1::Requested()||!held_map_m1::Allowed(now)||
       (readinessAck.read(ack)&&ack.actor==actor&&ack.epoch==epoch&&ack.activation==activation&&ack.vm==vm&&ack.helper==helperId&&ack.native==intent.native&&ack.serial>recovery)||
       (readinessTerminal.read(terminal)&&terminal.actor==actor&&terminal.epoch==epoch&&terminal.activation==activation&&
        terminal.vm==vm&&terminal.recovery==recovery&&terminal.native==intent.native)){
        readinessUntil.store(0,std::memory_order_release);return;}
    if(held_map_m1::RendererIntentMatches43(intent)&&readinessRequest.publish({sourceTick,activation,epoch,recovery,actor,helperId,intent.native}))readinessUntil.store(sourceTick+80,std::memory_order_release);
}
bool ReportHelperMissing(std::uint32_t actor,std::uint64_t epoch,std::uint64_t activation) noexcept {
    // This is a signal from an exact ABSENT lookup, not tracking/UI hide or an
    // entity/character fault. It performs no native/VM call and grants no pose.
    ReadinessAck ack{};
    if(!actor||!epoch||!ready.load(std::memory_order_acquire)||!held_map_m1::Requested()||
       activation!=held_map_m1::activation.load(std::memory_order_acquire)||!readinessAck.read(ack)||
       ack.actor!=actor||ack.epoch!=epoch||ack.activation!=activation||!ack.serial||
       !held_map_m1::RendererIntentMatches43({activation,true,ack.native})||
       ack.vm!=readinessVm.load(std::memory_order_acquire))return false;
    auto invalidated=missingSerial.load(std::memory_order_acquire);
    if(invalidated<ack.serial&&missingSerial.compare_exchange_strong(invalidated,ack.serial,std::memory_order_acq_rel)){
        char message[224]{};
        std::snprintf(message,sizeof(message),"T5 MAP_READINESS actor=%u epoch=%llu activation=%llu outcome=HELPER_ABSENT_ACK_INVALIDATED recovery=%llu freshRequest=REQUIRED",
            actor,static_cast<unsigned long long>(epoch),static_cast<unsigned long long>(activation),static_cast<unsigned long long>(ack.serial));
        diagnostic_trace::Write(message);return true;
    }
    return false;
}
bool StaticModelM41(std::uint32_t actor,std::uint64_t epoch,std::uint64_t activation,std::uint32_t helperId,char (&path)[160]) noexcept {
    path[0]=0;ReadinessAck ack{};
    if(!actor||!epoch||!helperId||!ready.load(std::memory_order_acquire)||!held_map_m1::Requested()||
       activation!=held_map_m1::activation.load(std::memory_order_acquire)||!readinessAck.read(ack)||
       ack.actor!=actor||ack.epoch!=epoch||ack.activation!=activation||ack.helper!=helperId||ack.vm!=readinessVm.load(std::memory_order_acquire)||
       ack.serial<=missingSerial.load(std::memory_order_acquire)||!static_paper_m41::valid(ack.artwork)||
       !held_map_m1::RendererIntentMatches43({activation,true,ack.native}))return false;
    const auto n=std::snprintf(path,sizeof(path),"%s%s.cgf",static_paper_m41::prefix,ack.artwork);
    return n>0&&std::size_t(n)<sizeof(path);
}
bool LetterReadyD44(std::uint32_t actor,std::uint64_t epoch,std::uint64_t activation,std::uint64_t now) noexcept {
    LetterAckD44 letter{};ReadinessAck helper{};
    return actor&&epoch&&ready.load(std::memory_order_acquire)&&held_map_m1::Requested()&&held_map_m1::Allowed(now)&&
        activation==held_map_m1::activation.load(std::memory_order_acquire)&&letterAckD44.read(letter)&&
        letter.actor==actor&&letter.epoch==epoch&&letter.activation==activation&&letter.tick&&now>=letter.tick&&now-letter.tick<=750&&
        letter.vm==readinessVm.load(std::memory_order_acquire)&&readinessAck.read(helper)&&
        helper.actor==actor&&helper.epoch==epoch&&helper.activation==activation&&helper.vm==letter.vm&&
        helper.serial==letter.helperSerial&&helper.serial>missingSerial.load(std::memory_order_acquire)&&
        held_map_m1::RendererIntentMatches43({activation,true,helper.native});
}
bool Read(Snapshot& out,std::uint64_t now) noexcept {
    out={};const auto until=desiredUntil.load(std::memory_order_acquire);
    return until&&now<=until&&held_map_m1::Requested()&&held_map_m1::Allowed(now)&&
        snapshot.read(out)&&Fresh(out,now,held_map_m1::activation.load(std::memory_order_acquire));
}
}
