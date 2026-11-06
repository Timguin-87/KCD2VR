// C46 carafes: observe the EXACT native event interval/list; never replace,
// postpone, duplicate or fabricate the recipe callbacks. Included in owner TU.
// 842248(self,includeStart,from,to,info,animData,segment) loops native events in
// (from,to]. 84351C copies the same event into AnimEventInstance for 6C29D8.
struct SemiResourceScan46 {
    void* skel=nullptr;const void* info=nullptr;
    float from=0,to=0;int segment=-1;bool valid=false;
};
static thread_local SemiResourceScan46 g_semi_scan46{};
using SemiResourceScanFn46=void(__fastcall*)(void*,bool,float,float,const void*,void*,int);
using SemiResourceEventFn46=void(__fastcall*)(void*,void*,const void*);
SemiResourceScanFn46 g_semi_scan_original46=nullptr;
SemiResourceEventFn46 g_semi_resource_original46=nullptr;
struct SemiResourceInfo46 {
    std::uintptr_t list=0,path=0;int first=0,animation=-1,last=0;float weight=0;
};
static_assert(sizeof(SemiResourceInfo46)==0x20);
// R25: explain the actual refusal at the native resource boundary. Copies only
// values already read by this invocation; never performs a diagnostic native read.
static void SemiResourcePathR25(const char* stage,const semi46::WorkWitness& w,
    float from=0.f,float to=0.f,int animation=-1) noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;
    constexpr unsigned mask=(1u<<unsigned(cap::Domain::Interaction));
    if(!(cap::gate.mask.load(std::memory_order_relaxed)&mask))return;
    const auto now=std::uint64_t(GetTickCount64());
    const auto domain=cap::Domain::Interaction;
    if(!cap::active(domain,now))return;
    const auto session=cap::gate.session[unsigned(domain)].load(std::memory_order_acquire);
    static thread_local kcd2vr::diagnostics::interaction_receipt59c10::Gate gate;
    unsigned key=0;for(const char* p=stage;*p;++p)key=key*33+unsigned(*p);
    if(!gate.observe(session,now,{w.child,std::uint32_t(w.kind),w.phase,int(key)},0,256))return;
    char line[640]{};
    std::snprintf(line,sizeof(line),
        "R25 ALCHEMY_RESOURCE_PATH stage=%s session=%llu thread=%u parent=%llX child=%llX token=%u instance=%llX lifetime=%llu phase=%d scan=%.6f..%.6f animation=%d seen=%llu coalesced=%llu budgetExhausted=%d nativeEvents=UNCHANGED",
        stage,(unsigned long long)session,unsigned(GetCurrentThreadId()),(unsigned long long)w.parent,
        (unsigned long long)w.child,w.token,(unsigned long long)w.instance,(unsigned long long)w.lifetime,w.phase,
        double(from),double(to),animation,(unsigned long long)gate.seen,(unsigned long long)gate.coalesced,gate.exhausted?1:0);
    diagnostic_trace::Write(line);
}
// D60 F5 receipts follow the existing native resource callback, including the
// formerly invisible DetachResource/Detach stages. Snapshot reads only occur
// while finite Interaction capture is active. No event/cost/emitter is invoked
// here; handler return is evidence of dispatch, never a recipe/effect result.
struct SemiResourceReceiptD60 {
    semi46::WorkWitness owner{};
    std::uint64_t session=0,serial=0,tick=0,admission=0,sample=0,pose=0,source=0,mode=0;
    std::uintptr_t attachment=0,object=0,eventList=0;
    std::uint32_t event=0,entity=0;int type=-1,side=-1,animation=-1;
    float eventTime=-1,scanFrom=-1,scanTo=-1,firstWork=-1,useTime=-1,detachTime=-1;
    double producerTotal=0;bool controlJoined=false,toolJoined=false,producerJoined=false,clockStop=true;
    unsigned held=0;bool foreign=false,intervalJoined=false,armed=false,exhausted=false;
};
struct SemiResourceBudgetD60 {std::uint64_t session=0,used=0;};
static thread_local SemiResourceBudgetD60 g_semi_resource_budget_d60{};
static SemiResourceReceiptD60 SemiResourceBeginD60(const semi46::WorkWitness& w,std::uint32_t name,
    const void* event,const SemiResourceState46* interval,bool foreign) noexcept {
    SemiResourceReceiptD60 r{};
    namespace cap=kcd2vr::diagnostics::capture168;constexpr auto domain=cap::Domain::Interaction;
    if(!(cap::gate.mask.load(std::memory_order_relaxed)&(1u<<unsigned(domain)))||
       (name!=0xDEC97C81u&&name!=0xE5D8547Bu&&name!=0x94B1CC4Bu&&name!=0xD3845300u))return r;
    const auto now=std::uint64_t(GetTickCount64());if(!cap::active(domain,now))return r;
    const auto session=cap::gate.session[unsigned(domain)].load(std::memory_order_acquire);
    if(!session||w.kind!=semi46::Resource||!w.instance||!w.lifetime||!w.parent)return r;
    auto& budget=g_semi_resource_budget_d60;
    if(budget.session!=session)budget={session,0};
    if(budget.used>=257)return r;
    r.armed=true;r.session=session;r.serial=++budget.used;r.exhausted=r.serial==257;
    r.owner=w;r.event=name;r.tick=now;r.foreign=foreign;if(r.exhausted)return r;
    // Event time at +0 is already qualified/used by the native interval reader.
    // This bounded F5-only value read does not touch animation/effect state.
    r.eventTime=Rd<float>(event,0,-1.f);
    if(g_semi_scan46.valid){r.scanFrom=g_semi_scan46.from;r.scanTo=g_semi_scan46.to;}
    if(interval&&semi46::same(w,interval->owner)){
        r.intervalJoined=true;r.eventList=interval->eventList;r.animation=interval->animationId;
        r.firstWork=interval->firstWork;r.useTime=interval->useTime;r.detachTime=interval->detachTime67;
    }
    namespace ab=kcd2vr::subsystems::alchemy::bridge67;
    ab::Control c{};
    r.controlJoined=ab::control.read(c)&&c.enabled&&semi46::fresh(c.tick,now)&&c.instance==w.instance&&
        c.lifetime==w.lifetime&&(!c.parent||c.parent==w.parent)&&c.entity&&c.side>=0&&c.side<2&&
        c.modeEpoch==kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire);
    if(r.controlJoined){
        r.admission=c.admissionTickR22;r.entity=c.entity;r.type=c.type;r.side=c.side;
        r.pose=c.poseEpoch;r.source=c.sourceEpoch;r.mode=c.modeEpoch;r.held=(c.held[0]?1u:0u)|(c.held[1]?2u:0u);
        ab::NativeTool tool{};
        r.toolJoined=ab::tools[c.side].read(tool)&&tool.joined&&semi46::fresh(tool.tick,now)&&
            tool.instance==w.instance&&tool.lifetime==w.lifetime&&tool.parent==w.parent&&tool.poseEpoch==c.poseEpoch&&
            tool.entity==c.entity&&tool.type==c.type&&tool.side==c.side&&tool.binding&&
            tool.binding.action==w.parent&&tool.binding.entity==c.entity&&tool.binding.instance==w.instance&&
            tool.binding.lifetime==w.lifetime&&tool.binding.side==c.side&&tool.binding.type==c.type;
        if(r.toolJoined){r.attachment=tool.binding.attachment;r.object=tool.binding.object;}
    }
    semi46::Control producer{};
    r.producerJoined=semi46::control.read(producer)&&producer.enabled&&semi46::fresh(producer.tick,now)&&
        producer.instance==w.instance&&producer.lifetime==w.lifetime&&producer.parent==w.parent&&
        producer.child==w.child&&producer.token==w.token;
    if(r.producerJoined){r.sample=producer.sequence;r.producerTotal=producer.totalSeconds;r.clockStop=producer.clockStop;}
    return r;
}
static void SemiResourceEndD60(const SemiResourceReceiptD60& r,bool originalCalled) noexcept {
    if(!r.armed)return;
    namespace cap=kcd2vr::diagnostics::capture168;constexpr auto domain=cap::Domain::Interaction;
    const auto now=std::uint64_t(GetTickCount64());
    if(!cap::active(domain,now)||cap::gate.session[unsigned(domain)].load(std::memory_order_acquire)!=r.session)return;
    char line[1700]{};
    if(r.exhausted){std::snprintf(line,sizeof(line),"D60 ALCHEMY_RESOURCE_EVENT event=BUDGET_EXHAUSTED session=%llu thread=%u budget=256",
        (unsigned long long)r.session,unsigned(GetCurrentThreadId()));diagnostic_trace::Write(line);return;}
    const char* name=r.event==0xDEC97C81u?"ATTACH":r.event==0xE5D8547Bu?"DETACH_RESOURCE":r.event==0x94B1CC4Bu?"USE":"DETACH";
    std::snprintf(line,sizeof(line),
        "D60 ALCHEMY_RESOURCE_EVENT event=%s session=%llu serial=%llu thread=%u foreign=%d begin=%llu returned=%llu nativeHandler=%s instance=%llX life=%llu parent=%llX child=%llX token=%u nativeTime=%.6f scan=%.6f..%.6f intervalJoined=%d list=%llX animation=%d firstWork=%.6f useTime=%.6f detachTime=%.6f controlJoined=%d admission=%llu heldEntity=%u type=%d side=%d heldMask=%u pose=%llu source=%llu mode=%llu toolJoined=%d attachment=%llX object=%llX producerJoined=%d sample=%llu physicalStop=%d producerTotal=%.6f bindingPhase=PRE_HANDLER_SNAPSHOT mouth=UNPROVEN emitter=UNJOINED semanticEffect=NOT_OBSERVED",
        name,(unsigned long long)r.session,(unsigned long long)r.serial,unsigned(GetCurrentThreadId()),r.foreign,
        (unsigned long long)r.tick,(unsigned long long)now,originalCalled?"RETURNED_ONCE":"NOT_CALLED",
        (unsigned long long)r.owner.instance,(unsigned long long)r.owner.lifetime,(unsigned long long)r.owner.parent,
        (unsigned long long)r.owner.child,r.owner.token,r.eventTime,r.scanFrom,r.scanTo,r.intervalJoined,
        (unsigned long long)r.eventList,r.animation,r.firstWork,r.useTime,r.detachTime,r.controlJoined,
        (unsigned long long)r.admission,r.entity,r.type,r.side,r.held,(unsigned long long)r.pose,(unsigned long long)r.source,
        (unsigned long long)r.mode,r.toolJoined,(unsigned long long)r.attachment,(unsigned long long)r.object,
        r.producerJoined,(unsigned long long)r.sample,r.clockStop,r.producerTotal);
    diagnostic_trace::Write(line);
}
struct SemiResourceIntervalTraceR25 {
    const semi46::WorkWitness& owner;const char* stage="SCAN_OR_SKELETON";
    float from=0,to=0;int animation=-1;
    ~SemiResourceIntervalTraceR25(){SemiResourcePathR25(stage,owner,from,to,animation);}
};
static bool SemiResourceInterval46(const semi46::WorkWitness& w,const void* event,SemiResourceState46& out) noexcept {
    const auto& scan=g_semi_scan46;const auto b=g_base.load(std::memory_order_acquire);
    SemiResourceIntervalTraceR25 trace{w};trace.from=scan.from;trace.to=scan.to;
    if(!scan.valid||!event||!scan.skel||scan.skel!=animation_filter::PlayerSkeletonAnim()||
       Rd<std::uintptr_t>(scan.skel,0,0)!=b+kSkelVft||!std::isfinite(scan.from)||!std::isfinite(scan.to)||
       scan.from<0||scan.to<scan.from||scan.to>1.f)return false;
    trace.stage="FRAGMENT_OR_PARENT_EVENT_IDS";
    auto* parent=reinterpret_cast<void*>(w.parent);auto* child=reinterpret_cast<void*>(w.child);
    const auto player=Rd<void*>(reinterpret_cast<void*>(w.instance),0x18,nullptr);
    const auto params=Rd<void*>(player,0xA50,nullptr);
    const int fragment=Rd<int>(child,kActFragId,-1);std::uint32_t crc=0;
    if(!params||fragment<0||fragment!=Rd<int>(params,0x90,-2)||
       !FragmentCrc19c4(g_henry_ctrl.load(std::memory_order_acquire),fragment,crc)||crc!=0xC5990708u||
       Rd<std::uint32_t>(parent,0x68,0)!=0xDEC97C81u||Rd<std::uint32_t>(parent,0x6C,0)!=0xD3845300u||
       Rd<std::uint32_t>(parent,0x70,0)!=0x94B1CC4Bu)return false;
    trace.stage="EVENT_LIST_IDENTITY";
    SemiResourceInfo46 info{};
    if(!SehCopy(scan.info,&info,sizeof(info))||!info.list||!info.path||info.animation<0||
       info.last<=info.first||info.last-info.first>64||
       Rd<std::uintptr_t>(reinterpret_cast<void*>(info.list),0,0)!=b+0x3A7D9D0)return false;
    trace.animation=info.animation;trace.stage="EVENT_LIST_COUNT";
    const auto entries=Rd<std::uintptr_t>(reinterpret_cast<void*>(info.list),8,0);
    const int count=entries?Rd<int>(reinterpret_cast<void*>(entries),-4,-1):-1;
    if(count<3||count>256||!entries||entries>UINTPTR_MAX-std::size_t(count)*0x50)return false;
    // The callback must originate from THIS native list and THIS scan. An
    // arbitrary Attach notification or another actor/layer cannot bootstrap it.
    trace.stage="ATTACH_SOURCE_MEMBERSHIP";
    std::uintptr_t source=0,eventPath=0;int animation=-1,number=-1;float attach=-1;std::uint32_t eventCrc=0;
    if(!SehCopy(static_cast<const unsigned char*>(event)+0x50,&source,sizeof(source))||
       !SehCopy(static_cast<const unsigned char*>(event)+0x10,&eventPath,sizeof(eventPath))||
       !SehCopy(static_cast<const unsigned char*>(event)+0x18,&animation,sizeof(animation))||
       !SehCopy(static_cast<const unsigned char*>(event)+8,&number,sizeof(number))||
       !SehCopy(event,&attach,sizeof(attach))||!SehCopy(static_cast<const unsigned char*>(event)+0x1C,&eventCrc,sizeof(eventCrc))||
       eventPath!=info.path||animation!=info.animation||number!=info.last-info.first-1||eventCrc!=0xDEC97C81u||
       source<entries||source>=entries+std::size_t(count)*0x50||(source-entries)%0x50||
       !std::isfinite(attach)||attach<scan.from||attach>scan.to)return false;
    trace.stage="EVENT_MARKER_DATA";
    float use=-1,detach=-1,firstWork=2.f,firstEventD86=2.f;
    std::uintptr_t firstEventPtrD86=0,detachPtrD86=0;std::uint32_t firstEventCrcD86=0;
    unsigned uses=0,attaches=0,detaches=0;bool sourceFound=false,turnMarkersValidD86=true;
    for(int i=0;i<count;++i){const auto at=entries+std::size_t(i)*0x50;
        float time=-1;std::uint32_t name=0;int segment=-1;
        if(!SehCopy(reinterpret_cast<void*>(at),&time,sizeof(time))||
           !SehCopy(reinterpret_cast<void*>(at+8),&name,sizeof(name))||!SehCopy(reinterpret_cast<void*>(at+0xC),&segment,sizeof(segment)))return false;
        if(segment!=scan.segment)continue;
        const bool workMarker=name==0xDEC97C81u||name==0xD3845300u||name==0x94B1CC4Bu||name==0xE5D8547Bu;
        if(!std::isfinite(time)||time<0.f||time>1.f){
            if(workMarker)return false;
            turnMarkersValidD86=false;continue; // retain old work admission; omit only the new fast segment
        }
        // Do not presume that Use is the first real work/FX boundary. Any
        // authored post-Attach event, including unknown/custom/filtered ones,
        // closes Henry-only acceleration before it can be dispatched.
        if(time>attach&&time<firstEventD86){firstEventD86=time;firstEventPtrD86=at;firstEventCrcD86=name;}
        if(name!=0xDEC97C81u&&name!=0xD3845300u&&name!=0x94B1CC4Bu&&name!=0xE5D8547Bu)continue;
        // The scanner also has a native actor-name filter and custom callbacks.
        // Qualified work markers must be ordinary, unfiltered native events.
        std::uintptr_t filter=0,custom=0;char first=1;
        if(!SehCopy(reinterpret_cast<void*>(at+0x28),&filter,sizeof(filter))||!filter||
           !SehCopy(reinterpret_cast<void*>(filter),&first,sizeof(first))||first||
           !SehCopy(reinterpret_cast<void*>(at+0x48),&custom,sizeof(custom))||custom)return false;
        if(name==0xDEC97C81u){++attaches;if(at==source&&time==attach)sourceFound=true;}
        if(name==0x94B1CC4Bu){++uses;use=time;if(time<firstWork)firstWork=time;}
        if(name==0xD3845300u){++detaches;detach=time;detachPtrD86=at;}
        if(name==0xE5D8547Bu&&time<firstWork)firstWork=time; // native DetachResource, when present
    }
    trace.stage="EVENT_ORDER_OR_ATTACH_WORK_SAME_SCAN";
    if(!sourceFound||attaches!=1||uses!=1||detaches!=1||!(attach<firstWork&&firstWork<=use&&use<detach)||
       !(scan.to<firstWork))return false; // same-frame Attach+work burst stays completely native
    trace.stage="EVENT_LIST_CHANGED";
    SemiResourceInfo46 again{};if(!SehCopy(scan.info,&again,sizeof(again))||std::memcmp(&again,&info,sizeof(info))!=0||
       Rd<std::uintptr_t>(reinterpret_cast<void*>(info.list),8,0)!=entries||Rd<int>(reinterpret_cast<void*>(entries),-4,-1)!=count)return false;
    trace.stage="RESOURCE_VERB_RAISED_OR_TAGS";
    out=SemiResourceState46{};out.owner=w;out.owner.phase=1;out.owner.working=true;out.phase=1;
    if(!SehCopy(reinterpret_cast<void*>(w.instance+0x304),&out.verb,sizeof(out.verb))||out.verb<13||out.verb>16||
       !SehCopy(reinterpret_cast<void*>(w.instance+0x428),&out.raised,sizeof(out.raised))||out.raised>1||
       !SehCopy(reinterpret_cast<void*>(w.child+kActTags),out.tags,sizeof(out.tags)))return false;
    out.firstWork=firstWork;out.useTime=use;out.detachTime67=detach;out.eventList=info.list;out.skel=reinterpret_cast<std::uintptr_t>(scan.skel);out.animationId=animation;
    out.turnD86.boundary=firstEventD86;out.turnD86.boundaryEvent=turnMarkersValidD86?firstEventPtrD86:0;
    out.turnD86.boundaryCrc=firstEventCrcD86;out.turnD86.detachEvent=detachPtrD86;out.turnD86.detach=detach;
    trace.stage="INTERVAL_QUALIFIED";
    return true;
}
static void __fastcall SemiResourceScanDetour46(void* skel,bool includeStart,float from,float to,const void* info,void* animData,int segment) {
    g_semi_inflight46.fetch_add(1,std::memory_order_acq_rel);
    const auto xr34Previous=XR34BeginScanNative(skel,includeStart,from,to,info,segment);
    const auto previous=g_semi_scan46;g_semi_scan46=SemiResourceScan46{};
    if(semi46::hooksReady.load(std::memory_order_acquire)&&!g_semi_stop46.load(std::memory_order_acquire)&&
       kcd2vr::activity_input163::witness175.active[0].load(std::memory_order_acquire)&&
       (kcd2vr::minigame_hands45::modeFor(kcd2vr::minigame_hands45::Alchemy)!=kcd2vr::minigame_hands45::Native||kcd2vr::guided_crafting55::active(std::uint64_t(GetTickCount64())))&&
       skel&&skel==animation_filter::PlayerSkeletonAnim())g_semi_scan46={skel,info,from,to,segment,true};
#if defined(_MSC_VER) && !defined(KCD2VR_GRASP_HOST_TEST)
    __try {if(g_semi_scan_original46)g_semi_scan_original46(skel,includeStart,from,to,info,animData,segment);}
    __finally {XR34EndScanNative(xr34Previous);g_semi_scan46=previous;g_semi_inflight46.fetch_sub(1,std::memory_order_acq_rel);}
#else
    try {if(g_semi_scan_original46)g_semi_scan_original46(skel,includeStart,from,to,info,animData,segment);}
    catch(...){XR34EndScanNative(xr34Previous);g_semi_scan46=previous;g_semi_inflight46.fetch_sub(1,std::memory_order_acq_rel);throw;}
    XR34EndScanNative(xr34Previous);g_semi_scan46=previous;g_semi_inflight46.fetch_sub(1,std::memory_order_acq_rel);
#endif
}
static void SemiResourceDetachedD86(void* ctrl,const semi46::WorkWitness& observed) noexcept {
    if(!OnAlchemyUpdateOwnerR27()||!semi46::same(observed,g_semi_resource46.owner))return;
    semi46::WorkWitness w{};
    if(!SemiAlchemyParent46(ctrl,w,observed.parent)||!semi46::same(w,observed))return;
    g_semi_resource46.phase=3;w.phase=3;w.working=false;(void)semi46::work.publish(w);
    if(g_semi_alchemy46.written&&semi46::same(w,g_semi_alchemy46.owner))
        (void)SemiAlchemyRestore46("NATIVE_DETACH_FINISH_D86");
}
static bool SemiResourceDirectClockD86(void* ctrl,void* character,const void* event,SemiResourceState46& interval) noexcept;
static void SemiResourceObserve19(void* self,void* character,const void* event);
static void __fastcall SemiResourceEventDetour46(void* self,void* character,const void* event) {
    // INVESTIGATE_D67R1[RESOURCE_FLOW_ORIGIN]: male_alchemy.adb raised-water
    // fragment 4845232d-937d-4d5c-b04e-dc78fc9a3d3c starts ParticleEffect at
    // procedural ExitTime 3.266667, JointName=LeftHandSlot, AttachmentName="".
    // A current NativeTool entity/attachment -> actual emitter/mouth transform
    // and start/stop lifetime must be joined before claiming flow follows the
    // vessel opening. A CAF clock alone does not relocate or stop that emitter.
    // INVESTIGATE_D67R1[KETTLE_CLOCK]: native parent Update 0x17AEC54 obtains
    // the kettle entity at instance+0x168, then 0x239E6E2 adds parent dt through
    // its slot-2 interface getter+0x260/setter+0x258. This independent clock is
    // not this child's +0x5C. Qualify its current object, duration and lifetime
    // before synchronizing liquid/pot motion; never advance recipe Use twice.
    SemiCallbackScope46 inflight;
    if(!OnOwnerThread13()){SemiResourceObserve19(self,character,event);return;}SemiResourceState46 candidate{};bool claim=false;
    semi46::WorkWitness detachedD86{};SemiResourceReceiptD60 receiptD60{};
    std::uintptr_t carafeChildD85=0;std::uint32_t carafeTokenD85=0,carafeNameD85=0; // D85 craft
    {OwnerTransaction19c4 transaction;if(transaction){
        semi46::WorkWitness w{};void* ctrl=g_henry_ctrl.load(std::memory_order_acquire);
        if(SemiAlchemyParent46(ctrl,w,reinterpret_cast<std::uintptr_t>(self))&&w.kind==semi46::Resource){
            const auto name=Rd<std::uint32_t>(event,0x1C,0);
            carafeChildD85=w.child;carafeTokenD85=w.token;carafeNameD85=name; // D85 craft
            if(name==Rd<std::uint32_t>(self,0x70,0)&&name==0x94B1CC4Bu){
                if(semi46::same(w,g_semi_resource46.owner))g_semi_resource46.phase=2;
                const bool holdAfterUse67=kcd2vr::workstation178r20c1::fullVrAlchemy63(GetTickCount64())&&
                    g_semi_scan46.valid&&g_semi_scan46.to<g_semi_resource46.detachTime67;
                if(!holdAfterUse67&&
                   g_semi_alchemy46.written&&semi46::same(w,g_semi_alchemy46.owner))(void)SemiAlchemyRestore46("NATIVE_POUR_COMPLETED");
            }else if(name==0xDEC97C81u&&!semi46::same(w,g_semi_retired46)&&!g_semi_alchemy46.written){
                claim=character&&g_semi_scan46.valid&&character==Rd<void*>(g_semi_scan46.skel,kSkelChar,nullptr)&&
                    SemiResourceInterval46(w,event,candidate);
                if(!claim)SemiAlchemyReceipt46("RESOURCE_INTERVAL_NATIVE_FALLBACK",w);
            }
            if(name==0xD3845300u&&name==Rd<std::uint32_t>(self,0x6C,0)&&semi46::same(w,g_semi_resource46.owner))detachedD86=w;
            receiptD60=SemiResourceBeginD60(w,name,event,claim?&candidate:&g_semi_resource46,false);
        }
    }}
    // Henry's actual attachment/effects always execute once through the original.
    const bool calledD60=g_semi_resource_original46!=nullptr;
    if(g_semi_resource_original46)g_semi_resource_original46(self,character,event);
    SemiResourceEndD60(receiptD60,calledD60);
    if(calledD60)kcd2vr::subsystems::alchemy::carafe_d85::ledger.note(carafeChildD85,carafeTokenD85,carafeNameD85); // D85 craft: returned once
    if(calledD60&&detachedD86.child){OwnerTransaction19c4 transaction;if(transaction)
        SemiResourceDetachedD86(g_henry_ctrl.load(std::memory_order_acquire),detachedD86);}
    if(!claim||!calledD60)return;
    OwnerTransaction19c4 transaction;if(!transaction)return;
    semi46::WorkWitness w{};void* ctrl=g_henry_ctrl.load(std::memory_order_acquire);
    if(!SemiAlchemyParent46(ctrl,w,reinterpret_cast<std::uintptr_t>(self))||!semi46::same(w,candidate.owner))return;
    // Both the same-owner callback and the deferred observation must join the
    // live CAF before taking its clock. Event markers alone cannot qualify
    // the ordinary post-Use tail or its original Detach boundary.
    if(!SemiResourceDirectClockD86(ctrl,character,event,candidate)){
        SemiAlchemyReceipt46("RESOURCE_ATTACH_CLOCK_NATIVE_FALLBACK_D86",w);return;
    }
    g_semi_resource46=candidate;w.phase=1;w.working=true;(void)semi46::work.publish(w);
    if(SemiAlchemyAcquire46(ctrl,w,false,true))SemiAlchemyReceipt46("RESOURCE_ATTACH_BEFORE_FIRST_WORK",w,0.f);
    else {g_semi_resource46.phase=3;w.phase=3;w.working=false;(void)semi46::work.publish(w);}
}

// A foreign native event may observe its current owner, never write its clock.
// All fields are values protected by OwnerTransaction19c4; no event/info stack
// pointer escapes. The original owner consumes once before its native Update.
struct SemiResourcePending19 {
    SemiResourceState46 interval{};void* controller=nullptr;void* character=nullptr;
    std::uintptr_t entries=0,attachSource=0;std::uint64_t tick=0;float scanEnd=0,attach=0;
    int segment=-1,count=0;unsigned event=0;bool holdAfterUse=false,fullVr=false;
    std::uint64_t modeEpoch=0,poseEpoch=0,sourceEpoch=0;int mode=-1,hand=-1;
    kcd2vr::subsystems::alchemy::bridge67::Control admitted{};
} g_semi_resource_pending19;
// Before Attach, an admitted resource has no joined tool: parent/held are zero.
// A normal same-admission attachment publication may fill them before this
// owner Tick. Only that exact current native LEFT attachment may bridge it.
// Retained Grip uses the admitted .35 threshold; phase0 has not acquired the
// gesture yet, so its derived held bit may still use the initial .55 threshold.
// The adapter keeps a released hand distinct (semi hand1); its deliberate R17
// same-admission regrip may resume only this still-before-firstWork observation.
static bool SemiResourceAttachedInput22(const SemiResourcePending19& p,const semi46::WorkWitness& w,
    const kcd2vr::subsystems::alchemy::bridge67::Control& c,std::uint64_t now) noexcept {
    namespace bridge=kcd2vr::subsystems::alchemy::bridge67;
    if(!p.fullVr||w.kind!=semi46::Resource||p.admitted.parent||p.admitted.held[0]||p.admitted.held[1]||
       !p.admitted.admissionTickR22||p.admitted.admissionTickR22!=c.admissionTickR22||
       p.admitted.side!=1||c.side!=1||c.parent!=w.parent||c.held[0])return false;
    bridge::NativeTool t{};kcd2vr::workstation_sample46::Frame f{};
    if(!bridge::tools[1].read(t)||!t.joined||t.side!=1||t.type!=c.type||t.entity!=c.entity||
       t.instance!=w.instance||t.lifetime!=w.lifetime||t.parent!=w.parent||
       t.character!=reinterpret_cast<std::uintptr_t>(p.character)||t.poseEpoch!=p.poseEpoch||!semi46::fresh(t.tick,now)||
       !frik_pose_probe::WorkstationFrame46(f)||!f.allowed||!kcd2vr::workstation_sample46::fresh(f,now)||
       f.poseEpoch!=p.poseEpoch||f.sourceEpoch!=p.sourceEpoch||!f.hand[1].valid||
       !kcd2vr::workstation_sample46::finite(f.hand[1])||f.hand[1].grip<.35f)return false;
    const auto& binding=t.binding;
    return binding&&binding.instance==w.instance&&binding.lifetime==w.lifetime&&
        binding.player==Rd<std::uintptr_t>(reinterpret_cast<void*>(w.instance),0x18,0)&&
        binding.character==t.character&&binding.attachment&&binding.object&&binding.entity==c.entity&&
        binding.side==1&&binding.type==c.type&&binding.action==w.parent&&binding.actionClass;
}
static bool SemiResourceInputCurrent19(SemiResourcePending19& p,const semi46::WorkWitness& w,std::uint64_t now,bool capture) noexcept {
    namespace hands=kcd2vr::minigame_hands45;namespace bridge=kcd2vr::subsystems::alchemy::bridge67;
    semi46::Control c{};if(!SemiAlchemyInput46(w,now,c))return false;
    const auto modeEpoch=hands::modeEpoch67.load(std::memory_order_acquire);
    const int mode=int(hands::modeFor(hands::Alchemy));
    const bool full=kcd2vr::workstation178r20c1::fullVrAlchemy63(now);
    bridge::Control admitted{};
    if(full&&(!bridge::control.read(admitted)||!admitted.enabled||admitted.passenger||
       admitted.instance!=w.instance||admitted.lifetime!=w.lifetime||!semi46::fresh(admitted.tick,now)||
       admitted.poseEpoch!=c.poseEpoch||admitted.sourceEpoch!=c.sourceEpoch||admitted.modeEpoch!=modeEpoch||
       admitted.side<0||admitted.side>1||!admitted.entity))return false;
    if(capture){p.modeEpoch=modeEpoch;p.mode=mode;p.poseEpoch=c.poseEpoch;p.sourceEpoch=c.sourceEpoch;p.hand=c.hand;p.fullVr=full;p.admitted=admitted;}
    else if(p.modeEpoch!=modeEpoch||p.mode!=mode||p.poseEpoch!=c.poseEpoch||p.sourceEpoch!=c.sourceEpoch||p.hand!=c.hand||p.fullVr!=full||
       (full&&(p.admitted.entity!=admitted.entity||p.admitted.type!=admitted.type||p.admitted.side!=admitted.side||
        p.admitted.admissionTickR22!=admitted.admissionTickR22||
        ((p.admitted.parent!=admitted.parent||p.admitted.held[0]!=admitted.held[0]||p.admitted.held[1]!=admitted.held[1])&&
         !SemiResourceAttachedInput22(p,w,admitted,now)))))return false;
    return modeEpoch==hands::modeEpoch67.load(std::memory_order_acquire);
}
static bool SemiResourceTagsCurrent19(const SemiResourceState46& s) noexcept {
    int verb=-1;unsigned char raised=0,tags[20]{};
    return SehCopy(reinterpret_cast<void*>(s.owner.instance+0x304),&verb,sizeof(verb))&&verb==s.verb&&
        SehCopy(reinterpret_cast<void*>(s.owner.instance+0x428),&raised,sizeof(raised))&&raised==s.raised&&
        SehCopy(reinterpret_cast<void*>(s.owner.child+kActTags),tags,sizeof(tags))&&!std::memcmp(tags,s.tags,sizeof(tags));
}
static bool SemiResourceMarkersCurrent19(const SemiResourcePending19& p) noexcept {
    if(p.count<3||p.count>256||!p.entries||p.segment!=0)return false;
    unsigned attaches=0,uses=0,detaches=0;float firstWork=2.f,use=-1,detach=-1;bool source=false;
    for(int i=0;i<p.count;++i){const auto at=p.entries+std::size_t(i)*0x50;
        float time=-1;std::uint32_t crc=0;int segment=-1;std::uintptr_t filter=0,custom=0;char first=1;
        if(!SehCopy(reinterpret_cast<void*>(at),&time,sizeof(time))||!SehCopy(reinterpret_cast<void*>(at+8),&crc,sizeof(crc))||
           !SehCopy(reinterpret_cast<void*>(at+0xC),&segment,sizeof(segment)))return false;
        if(segment!=p.segment)continue;
        if(crc!=0xDEC97C81u&&crc!=0xD3845300u&&crc!=0x94B1CC4Bu&&crc!=0xE5D8547Bu)continue;
        if(!std::isfinite(time)||time<0||time>1||!SehCopy(reinterpret_cast<void*>(at+0x28),&filter,sizeof(filter))||!filter||
           !SehCopy(reinterpret_cast<void*>(filter),&first,sizeof(first))||first||
           !SehCopy(reinterpret_cast<void*>(at+0x48),&custom,sizeof(custom))||custom)return false;
        if(crc==0xDEC97C81u){++attaches;if(at==p.attachSource&&time==p.attach)source=true;}
        if(crc==0x94B1CC4Bu){++uses;use=time;firstWork=std::min(firstWork,time);}
        if(crc==0xD3845300u){++detaches;detach=time;}
        if(crc==0xE5D8547Bu)firstWork=std::min(firstWork,time);
    }
    return source&&attaches==1&&uses==1&&detaches==1&&p.attach<firstWork&&firstWork<=use&&use<detach&&
        firstWork==p.interval.firstWork&&use==p.interval.useTime&&detach==p.interval.detachTime67;
}
// D64: a failed clock join used to collapse every term into one misleading
// "no longer before work" message. Reuse values already required by the join;
// no extra native reads and no formatting outside finite F5 capture.
static void SemiResourceJoinRefusedD64(const semi46::WorkWitness& w,const char* why,
    std::uint64_t actual=0,std::uint64_t expected=0) noexcept {
    namespace cap=kcd2vr::diagnostics::capture168;constexpr auto domain=cap::Domain::Interaction;
    if(!(cap::gate.mask.load(std::memory_order_relaxed)&(1u<<unsigned(domain))))return;
    const auto now=std::uint64_t(GetTickCount64());if(!cap::active(domain,now))return;
    const auto session=cap::gate.session[unsigned(domain)].load(std::memory_order_acquire);
    unsigned key=0;for(const char* p=why;*p;++p)key=key*33+unsigned(*p);
    static thread_local kcd2vr::diagnostics::interaction_receipt59c10::Gate gate;
    if(!gate.observe(session,now,{w.child,std::uint32_t(w.kind),w.phase,int(key)},0,96))return;
    char line[640]{};std::snprintf(line,sizeof(line),
        "D64 F5_RESOURCE_CLOCK_JOIN why=%s session=%llu parent=%llX child=%llX token=%u instance=%llX lifetime=%llu actual=%llX expected=%llX policy=ORIGINAL_VALIDATION_RETAINED nativeWrites=NONE",
        why,(unsigned long long)session,(unsigned long long)w.parent,(unsigned long long)w.child,w.token,
        (unsigned long long)w.instance,(unsigned long long)w.lifetime,(unsigned long long)actual,(unsigned long long)expected);
    diagnostic_trace::Write(line);
}
static bool SemiResourceClockBeforeWork19(SemiResourcePending19& p,void* ctrl) noexcept {
    const auto b=g_base.load(std::memory_order_acquire);const auto& w=p.interval.owner;
    const auto fail=[&](const char* why,std::uint64_t actual=0,std::uint64_t expected=0){SemiResourceJoinRefusedD64(w,why,actual,expected);return false;};
    const auto bits=[](float f){std::uint32_t v=0;std::memcpy(&v,&f,sizeof(v));return v;};
    if(!OnAlchemyUpdateOwnerR27()||!SemiAlchemyPins46())return fail("OWNER_THREAD_OR_RETAIL_PINS");
    if(p.segment!=0||!p.interval.skel||p.interval.skel!=reinterpret_cast<std::uintptr_t>(animation_filter::PlayerSkeletonAnim()))return fail("SKELETON_OR_SEGMENT",p.interval.skel,std::uint64_t(p.segment));
    if(Rd<void*>(reinterpret_cast<void*>(p.interval.skel),kSkelChar,nullptr)!=p.character)return fail("CHARACTER_BACKLINK");
    SemiAlchemyLease46 witness{};witness.owner=w;witness.controller=ctrl;void* memberRoot=nullptr;
    const int membership=SemiAlchemyMembership46(witness,memberRoot);if(membership!=2)return fail("ROOT_MEMBERSHIP",std::uint64_t(membership),2);
    const auto read=[](std::uintptr_t at,auto& value){return at&&SehCopy(reinterpret_cast<void*>(at),&value,sizeof(value));};
    std::uintptr_t root=0,v=0,context=0,character=0,skeleton=0,set=0,header=0,entries=0,events=0;
    unsigned layer=0,layers=0,count=0,animation=~0u,flags=0,eventCount=0;
    unsigned char previousSegment=255,segment=255,segments=0;
    float pending[2]{},normalized=-1,duration=0,total=0,start=0,end=0,first=-1,last=-1;
    if(!read(w.child+kActRootScope,root)||root!=reinterpret_cast<std::uintptr_t>(memberRoot)||!read(root,v)||v!=b+0x3AEA100)return fail("ROOT_IDENTITY",root,reinterpret_cast<std::uintptr_t>(memberRoot));
    if(!read(root+0x20,context)||!read(context+0x10,character)||character!=reinterpret_cast<std::uintptr_t>(p.character)||!read(character,v)||v!=b+0x40602F8)return fail("CONTEXT_CHARACTER",character,reinterpret_cast<std::uintptr_t>(p.character));
    if(!read(character+0xA70,skeleton)||!read(skeleton+0x218,set)||!read(set,v)||v!=b+0x406BB38)return fail("ANIMATION_SET",v,b+0x406BB38);
    if(!read(root+0x30,layer)||layer>15)return fail("ROOT_LAYER",layer,15);
    if(!read(root+0x34,layers))return fail("ROOT_LAYER_COUNT",layers,1);
    if(layers!=1){
        // D67R1: current D66 log line 329605 rejects normal FullBody 0+3. Keep every
        // existing event/CAF proof below; only a single root CAF with two empty
        // slots, exact native ownership and no foreign rate writer may join.
        // INVESTIGATE_D67R1[RESOURCE_ROOT_LAYERS]: nonempty extra CAFs/other root shapes need their own
        // event/segment and procedural-owner proof, not a layers>=1 relaxation.
        if(layers!=3)return fail("ROOT_LAYER_COUNT",layers,1);
        const auto proof=SemiResourceRoot3D67R1(p.interval,ctrl);
        if(!proof)return fail(kcd2vr::alchemy_resource_scope_d67r1::name(proof.reason),layers,3);
    }
    if(!read(root+0x60,pending)||pending[0]!=0||pending[1]!=0)return fail("ROOT_PENDING_BITS",bits(pending[0]),bits(pending[1]));
    if(!read(character+0x110+std::size_t(layer)*0x40+0x48,entries)||!entries)return fail("FIFO_POINTER");
    if(!read(entries-4,count)||count!=1)return fail("FIFO_COUNT",count,1);
    if(!read(entries,v)||v!=b+0x3A70A60||!read(entries+8,v)||v)return fail("FIFO_ENTRY_IDENTITY_OR_SAMPLER",v);
    if(!read(entries+0x10,animation)||animation!=unsigned(p.interval.animationId))return fail("FIFO_ANIMATION",animation,unsigned(p.interval.animationId));
    if(!read(entries+0x14,previousSegment)||previousSegment!=0||!read(entries+0x16,segment)||segment!=0)return fail("FIFO_SEGMENTS",previousSegment,segment);
    if(!read(entries+0x4C,flags)||(flags&1))return fail("FIFO_FLAGS",flags);
    if(!read(entries+0x44,normalized)||!std::isfinite(normalized)||normalized<p.scanEnd||normalized>=p.interval.firstWork)return fail("FIFO_TIME_BITS",bits(normalized),(std::uint64_t(bits(p.scanEnd))<<32)|bits(p.interval.firstWork));
    if(!read(entries+0x34,duration)||!std::isfinite(duration)||duration<=.1f||duration>60.f)return fail("FIFO_DURATION_BITS",bits(duration));
    if(Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x406BB38),0xF0,0)!=b+0x955DF4)return fail("CAF_LOOKUP_VTABLE_PIN");
    const auto lookup=Native13<std::uintptr_t(__fastcall*)(void*,int)>(0x955DF4);if(!lookup)return fail("CAF_LOOKUP_UNAVAILABLE");
#if defined(_MSC_VER) && !defined(KCD2VR_GRASP_HOST_TEST)
    __try {header=lookup(reinterpret_cast<void*>(set),int(animation));}
    __except(EXCEPTION_EXECUTE_HANDLER){return fail("CAF_LOOKUP_FAULT");}
#else
    header=lookup(reinterpret_cast<void*>(set),int(animation));
#endif
    if(!header||header+0x48!=p.interval.eventList)return fail("CAF_EVENT_LIST",header?header+0x48:0,p.interval.eventList);
    if(!read(header+0xA4,segments)||segments!=1)return fail("CAF_SEGMENT_COUNT",segments,1);
    if(!read(header+0x60,first)||first!=0.f||!read(header+0x64,last)||last!=1.f)return fail("CAF_SEGMENT_DOMAIN_BITS",bits(first),bits(last));
    if(!read(header+0x34,total)||!std::isfinite(total)||std::abs(total-duration)>.001f)return fail("CAF_TOTAL_DURATION_BITS",bits(total),bits(duration));
    if(!read(header+0x2C,start)||!std::isfinite(start)||!read(header+0x30,end)||!std::isfinite(end)||std::abs((end-start)-duration)>.001f)return fail("CAF_RANGE_DURATION_BITS",bits(end-start),bits(duration));
    if(!read(header+0x48,v)||v!=b+0x3A7D9D0||!read(header+0x50,events)||events!=p.entries)return fail("CAF_EVENT_STORAGE",events,p.entries);
    if(!read(events-4,eventCount)||eventCount!=unsigned(p.count))return fail("CAF_EVENT_COUNT",eventCount,unsigned(p.count));
    if(!SemiResourceTagsCurrent19(p.interval))return fail("RESOURCE_TAGS_CHANGED");
    if(!SemiResourceMarkersCurrent19(p))return fail("RESOURCE_MARKERS_CHANGED");
    // Preserve the original final rejoin: a successful lookup is not a lease.
    std::uintptr_t again=0;float now=-1;unsigned currentId=~0u,currentCount=0,currentFlags=0;unsigned char currentSegment=255,currentPrevious=255;
    semi46::WorkWitness current{};
    const bool joined=read(w.child+kActRootScope,again)&&again==root&&
        read(character+0x110+std::size_t(layer)*0x40+0x48,again)&&again==entries&&
        read(entries-4,currentCount)&&currentCount==1&&read(entries+8,again)&&again==0&&
        read(entries+0x10,currentId)&&currentId==animation&&read(entries+0x14,currentPrevious)&&currentPrevious==0&&
        read(entries+0x16,currentSegment)&&currentSegment==0&&read(entries+0x4C,currentFlags)&&currentFlags==flags&&
        read(header+0xA4,currentSegment)&&currentSegment==1&&
        read(entries+0x44,now)&&now==normalized&&read(header+0x50,again)&&again==events&&
        read(events-4,currentCount)&&currentCount==eventCount&&
        read(root+0x30,currentCount)&&currentCount==layer&&read(root+0x34,currentCount)&&currentCount==layers&&
        read(root+0x60,pending)&&pending[0]==0&&pending[1]==0&&read(root+0x20,again)&&again==context&&
        read(context+0x10,again)&&again==character&&
        SemiAlchemyParent46(ctrl,current,w.parent)&&semi46::same(current,w)&&SemiResourceTagsCurrent19(p.interval);
    if(joined){
        auto& proof=p.interval.turnD86;
        proof.root=root;proof.context=context;proof.character=character;proof.entries=entries;
        proof.header=header;proof.eventEntries=events;proof.layer=layer;proof.layers=layers;
        proof.eventCount=eventCount;proof.animation=int(animation);proof.duration=duration;proof.headerDuration=total;proof.afterAttach=normalized;
        // Failure only withholds the new fast presentation; original physical
        // work admission and its effects retain their previous validation.
        if(!read(root+0x40,proof.procBegin)||!read(root+0x48,proof.procEnd))proof.procBegin=proof.procEnd=0;
    }
    return joined||fail("FINAL_OWNER_FIFO_REJOIN",currentId,animation);
}
static bool SemiResourceDirectClockD86(void* ctrl,void* character,const void* event,SemiResourceState46& interval) noexcept {
    if(!event||!character||!g_semi_scan46.valid||g_semi_scan46.skel!=reinterpret_cast<void*>(interval.skel))return false;
    SemiResourcePending19 p{};p.interval=interval;p.controller=ctrl;p.character=character;
    p.segment=g_semi_scan46.segment;p.scanEnd=g_semi_scan46.to;
    p.attach=Rd<float>(event,0,-1.f);p.attachSource=Rd<std::uintptr_t>(event,0x50,0);
    p.entries=Rd<std::uintptr_t>(reinterpret_cast<void*>(interval.eventList),8,0);
    p.count=p.entries?Rd<int>(reinterpret_cast<void*>(p.entries),-4,-1):-1;
    if(!SemiResourceClockBeforeWork19(p,ctrl))return false;
    interval=p.interval;return true;
}
static bool SemiResourceTurnPinsD86() noexcept {
    const auto b=g_base.load(std::memory_order_acquire);if(!b||!retail_build112::Ready(b))return false;
    static std::atomic<std::uintptr_t> pinned{0};if(pinned.load(std::memory_order_acquire)==b)return true;
    constexpr unsigned char layers[]{0x49,0x8B,0x6E,0x48,0x45,0x33,0xFF,0x49,0x2B,0x6E,0x40,0x48,0xC1,0xFD,0x06};
    constexpr unsigned char active[]{0x48,0x8B,0x4C,0x1E,0x28,0x0F,0x28,0xCF,0xF3,0x0F,0x58,0x4C,0x1E,0x20,0xC7,0x44,0x1E,0x20,0x00,0x00,0x00,0x00};
    constexpr unsigned char deadline[]{0xF3,0x0F,0x10,0x44,0x1E,0x1C,0x0F,0x2F,0xF0,0x72,0x1C};
    if(Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3AEA100),0x128,0)!=b+0x4B4638||
       !MatchBytes(b+0x4B46A6,layers,sizeof(layers))||!MatchBytes(b+0x4B470A,active,sizeof(active))||
       !MatchBytes(b+0x4B4748,deadline,sizeof(deadline)))return false;
    // Complete-object RTTI locators, offset0: active clips must be the native
    // Henry GroundAlignment / PositionAdjustAlignBone implementations.
    if(Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3AB5680),-8,0)!=b+0x4159508||
       Rd<std::uint32_t>(reinterpret_cast<void*>(b+0x4159508),12,0)!=0x4B438A0||
       Rd<std::uintptr_t>(reinterpret_cast<void*>(b+0x3AB60A8),-8,0)!=b+0x40C0EC0||
       Rd<std::uint32_t>(reinterpret_cast<void*>(b+0x40C0EC0),12,0)!=0x5032BC0)return false;
    pinned.store(b,std::memory_order_release);return true;
}
static float SemiResourceTurnRateD86(void* ctrl,const semi46::WorkWitness& w,float dt,double effectiveDt,bool credit) noexcept {
    namespace turn=kcd2vr::subsystems::alchemy::resource_turn_d86;
    const auto& interval=g_semi_resource46;
    if(!credit||w.kind!=semi46::Resource||w.phase!=1||!semi46::same(w,interval.owner)||
       interval.phase!=1||interval.turnD86.boundary>interval.firstWork||!OnAlchemyUpdateOwnerR27()||!SemiAlchemyPins46()||
       !SemiResourceTagsCurrent19(interval)||!SemiResourceTurnPinsD86()||g_semi_alchemy46.saved!=1.f||
       !kcd2vr::workstation178r20c1::fullVrAlchemy63(GetTickCount64()))return 0.f;
    const auto read=[](std::uintptr_t at,auto& value){return at&&SehCopy(reinterpret_cast<void*>(at),&value,sizeof(value));};
    float normalized=-1.f,beforeProcedure=-1.f;
    if(!turn::current(read,interval.turnD86,{g_base.load(std::memory_order_acquire),w.child,w.instance,w.token},normalized,beforeProcedure))return 0.f;
    semi46::WorkWitness again{};
    if(!SemiAlchemyParent46(ctrl,again,w.parent)||!semi46::same(w,again)||again.phase!=1)return 0.f;
    return turn::rate(AlchemyTransferSpeedD65(),dt,effectiveDt,
        normalized,interval.turnD86.duration,interval.turnD86.boundary,beforeProcedure,true);
}
static float SemiResourceTailRateD86(const semi46::WorkWitness& w,float ratio,double effectiveDt,bool home) noexcept {
    if(w.kind!=semi46::Resource||w.phase!=2||ratio<=0.f)return ratio;
    // Genuine Use has happened once. Continued tilt may advance its authored
    // effects at ordinary gesture rate, but cannot put the carafe down while
    // the hand is still above the pot. Current upright-home input permits the
    // original Detach; explicit grip release keeps the existing native return.
    if(home)return ratio;
    const auto& interval=g_semi_resource46;
    if(!semi46::same(w,interval.owner)||interval.phase!=2||!SemiResourceTagsCurrent19(interval)||
       !std::isfinite(effectiveDt)||effectiveDt<=.000001||g_semi_alchemy46.saved<=0.f)return 0.f;
    const auto read=[](std::uintptr_t at,auto& value){return at&&SehCopy(reinterpret_cast<void*>(at),&value,sizeof(value));};
    float normalized=-1.f;
    if(!kcd2vr::subsystems::alchemy::resource_turn_d86::cafCurrent(read,interval.turnD86,
       {g_base.load(std::memory_order_acquire),w.child,w.instance,w.token},normalized))return 0.f;
    if(interval.detachTime67!=interval.turnD86.detach)return 0.f;
    const double beforeDetach=double(interval.detachTime67-normalized)*interval.turnD86.duration-.080;
    if(!std::isfinite(beforeDetach)||beforeDetach<=.00001)return 0.f; // native float clock roundoff is not return intent
    return float(std::min(double(ratio),beforeDetach/(effectiveDt*double(g_semi_alchemy46.saved))));
}
static void SemiResourceConsume19(void* ctrl,std::uint64_t now) noexcept {
    if(!OnAlchemyUpdateOwnerR27()||!g_owner_transaction_depth19c4||!g_semi_resource_pending19.event)return;
    auto p=g_semi_resource_pending19;g_semi_resource_pending19={}; // once, including refusal
    semi46::WorkWitness w{};
    // Attach expires as an admission. An actual Use is durable completion for
    // the same still-current native owner; no new input can erase that event.
    if(p.controller!=ctrl||!p.tick||now<p.tick||
       (p.event!=0x94B1CC4Bu&&p.event!=0xD3845300u&&!semi46::fresh(p.tick,now))||!SemiAlchemyParent46(ctrl,w,p.interval.owner.parent)||
       !semi46::same(w,p.interval.owner)||!SemiResourceTagsCurrent19(p.interval))return;
    if(p.event==0xD3845300u){SemiResourceDetachedD86(ctrl,w);return;}
    if(p.event==0x94B1CC4Bu){
        if(semi46::same(w,g_semi_resource46.owner))g_semi_resource46.phase=2;
        if(!p.holdAfterUse&&g_semi_alchemy46.written&&semi46::same(w,g_semi_alchemy46.owner))
            (void)SemiAlchemyRestore46("NATIVE_POUR_COMPLETED");
        return;
    }
    if(p.event!=0xDEC97C81u||g_semi_alchemy46.written||semi46::same(w,g_semi_retired46)){
        SemiResourcePathR25("OWNER_ALREADY_WRITTEN_OR_RETIRED",w);return;}
    // D64: the genuine Attach and admitted current binding already happened.
    // Keep its event ledger independently of clock authority. phase0 does NOT
    // grant a lease; a later genuine Use can now advance this exact owner to
    // phase2 and permit its normal native Return instead of remaining GripRequired.
    g_semi_resource46=p.interval;g_semi_resource46.phase=0;
    g_semi_resource46.owner.phase=0;g_semi_resource46.owner.working=false;
    if(!SemiResourceInputCurrent19(p,w,now,false)){
        SemiResourcePathR25("ADMITTED_HAND_OR_ATTACHMENT_CHANGED",w);return;}
    if(!SemiResourceClockBeforeWork19(p,ctrl)){
        SemiResourcePathR25("NATIVE_CLOCK_OR_FIFO_NO_LONGER_BEFORE_WORK",w,p.scanEnd,p.interval.firstWork,p.interval.animationId);return;}
    if(!SemiResourceInputCurrent19(p,w,GetTickCount64(),false)){
        SemiResourcePathR25("INPUT_CHANGED_DURING_CLOCK_JOIN",w);return;}
    SemiResourcePathR25("ATTACH_CONSUMED_ON_OWNER",w,p.scanEnd,p.interval.firstWork,p.interval.animationId);
    g_semi_resource46=p.interval;w.phase=1;w.working=true;(void)semi46::work.publish(w);
    if(SemiAlchemyAcquire46(ctrl,w,false,true))SemiAlchemyReceipt46("RESOURCE_ATTACH_OWNER_HANDOFF",w,0.f);
    else {g_semi_resource46.phase=3;w.phase=3;w.working=false;(void)semi46::work.publish(w);}
}
static void SemiResourceObserve19(void* self,void* character,const void* event) {
    SemiResourcePending19 candidate{};SemiResourceReceiptD60 receiptD60{};
    std::uintptr_t carafeChildD85=0;std::uint32_t carafeTokenD85=0,carafeNameD85=0; // D85 craft
    {OwnerTransaction19c4 transaction;if(transaction&&g_semi_scan46.valid&&
       semi46::hooksReady.load(std::memory_order_acquire)&&!g_semi_stop46.load(std::memory_order_acquire)&&
       g_semi_scan46.skel==animation_filter::PlayerSkeletonAnim()&&character&&
       character==Rd<void*>(g_semi_scan46.skel,kSkelChar,nullptr)){
        semi46::WorkWitness w{};auto* ctrl=g_henry_ctrl.load(std::memory_order_acquire);
        if(SemiAlchemyParent46(ctrl,w,reinterpret_cast<std::uintptr_t>(self),false,true)){
            const auto name=Rd<std::uint32_t>(event,0x1C,0);
            if(w.kind==semi46::Resource){carafeChildD85=w.child;carafeTokenD85=w.token;carafeNameD85=name;} // D85 craft
            if(name==0xDEC97C81u&&!g_semi_alchemy46.written&&!semi46::same(w,g_semi_retired46)&&
               !semi46::same(w,g_semi_resource46.owner)&&SemiResourceInterval46(w,event,candidate.interval)&&
               SemiResourceInputCurrent19(candidate,w,w.tick,true)){
                candidate.event=name;candidate.scanEnd=g_semi_scan46.to;candidate.segment=g_semi_scan46.segment;
                candidate.attach=Rd<float>(event,0,-1);candidate.attachSource=Rd<std::uintptr_t>(event,0x50,0);
                candidate.entries=Rd<std::uintptr_t>(reinterpret_cast<void*>(candidate.interval.eventList),8,0);
                candidate.count=candidate.entries?Rd<int>(reinterpret_cast<void*>(candidate.entries),-4,-1):0;
            }else if(name==0x94B1CC4Bu&&name==Rd<std::uint32_t>(self,0x70,0)&&semi46::same(w,g_semi_resource46.owner)){
                candidate.event=name;candidate.interval=g_semi_resource46;
                candidate.holdAfterUse=kcd2vr::workstation178r20c1::fullVrAlchemy63(GetTickCount64())&&g_semi_scan46.to<g_semi_resource46.detachTime67;
            }else if(name==0xD3845300u&&name==Rd<std::uint32_t>(self,0x6C,0)&&semi46::same(w,g_semi_resource46.owner)){
                candidate.event=name;candidate.interval=g_semi_resource46;
            }
            if(candidate.event){candidate.controller=ctrl;candidate.character=character;candidate.tick=w.tick;}
            receiptD60=SemiResourceBeginD60(w,name,event,candidate.event?&candidate.interval:&g_semi_resource46,true);
            // A genuine later native event supersedes an unconsumed Attach.
            if(name==0x94B1CC4Bu||name==0xD3845300u){
                if(semi46::same(w,g_semi_resource_pending19.interval.owner)){
                    if(g_semi_resource_pending19.event==0xDEC97C81u)g_semi_resource_pending19={};
                    else if(name==0xD3845300u&&g_semi_resource_pending19.event==0x94B1CC4Bu)
                        g_semi_resource_pending19.holdAfterUse=false;
                }
            }
        }
    }}
    const bool calledD60=g_semi_resource_original46!=nullptr;
    if(g_semi_resource_original46)g_semi_resource_original46(self,character,event);
    SemiResourceEndD60(receiptD60,calledD60);
    if(calledD60)kcd2vr::subsystems::alchemy::carafe_d85::ledger.note(carafeChildD85,carafeTokenD85,carafeNameD85); // D85 craft: returned once
    if(!candidate.event||(candidate.event==0xD3845300u&&!calledD60))return;
    OwnerTransaction19c4 transaction;if(!transaction)return;
    semi46::WorkWitness w{};
    if(SemiAlchemyParent46(candidate.controller,w,reinterpret_cast<std::uintptr_t>(self),false,true)&&
       semi46::same(w,candidate.interval.owner)&&SemiResourceTagsCurrent19(candidate.interval)&&
       (candidate.event!=0xDEC97C81u||SemiResourceInputCurrent19(candidate,w,GetTickCount64(),false))&&
       semi46::hooksReady.load(std::memory_order_acquire)&&!g_semi_stop46.load(std::memory_order_acquire)){
        // Repeated receipt of the same observation cannot renew its timestamp.
        if(!g_semi_resource_pending19.event||!semi46::same(w,g_semi_resource_pending19.interval.owner)||
           g_semi_resource_pending19.event!=candidate.event)g_semi_resource_pending19=candidate;
    }
}
