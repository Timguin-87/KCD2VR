// Included in the exact native action-owner TU. Script work runs ONLY from the
// shared original-first CScriptSystem update, never from Mannequin/physics/XR.
namespace st2=supply_t2;
static core::SnapshotMailbox<st2::Input> g_supply_input_t2;
static core::SnapshotMailbox<st2::Lease> g_supply_lease_t2;
static std::atomic<bool> g_supply_cleanup_t2{false};
using SupplyDrawFnT2=bool(__fastcall*)(void*,std::uint64_t,int);
static SupplyDrawFnT2 g_supply_draw_original_t2=nullptr;
static std::optional<DetourModKit::hook::Hook> g_supply_draw_hook_t2;
// C62: equipment uses this same entry. Keep the exact post-enable patch as an
// immutable receipt; an arbitrary jump (or merely having enabled supply) is not
// permission to skip the retail entry check. This hook/module has process life.
struct SupplyDrawEntry62 {std::uintptr_t base=0,trampoline=0;unsigned char bytes[16]{};};
static core::SnapshotMailbox<SupplyDrawEntry62> g_supply_draw_entry62;
static bool PublishSupplyDrawEntry62(std::uintptr_t base) noexcept {
    SupplyDrawEntry62 e{};e.base=base;e.trampoline=reinterpret_cast<std::uintptr_t>(g_supply_draw_original_t2);
    const unsigned char pristine[]{0x48,0x83,0xec,0x28,0x4c,0x8b,0xd9,0x48,0x89,0x54,0x24,0x38};
    if(!base||!e.trampoline||!g_supply_draw_hook_t2||!g_supply_draw_hook_t2->is_enabled()||
       !SehCopy(reinterpret_cast<const void*>(base+0x8CEA48),e.bytes,sizeof(e.bytes))||
       !std::memcmp(e.bytes,pristine,sizeof(pristine)))return false;
    return g_supply_draw_entry62.publish(e);
}
static bool SupplyDrawEntryQualified62(std::uintptr_t base) noexcept {
    SupplyDrawEntry62 e{};unsigned char actual[16]{};
    return base&&base==WhBase()&&retail_build112::Ready(base)&&g_supply_draw_entry62.read(e)&&
        e.base==base&&e.trampoline&&SehCopy(reinterpret_cast<const void*>(base+0x8CEA48),actual,sizeof(actual))&&
        !std::memcmp(actual,e.bytes,sizeof(actual));
}
struct SupplyDrawScopeT2 {
    st2::Input input{};std::uint64_t serial=0,vm=0,key=0;int kind=-1,calls=0;
    bool submitted=false;
};
static thread_local SupplyDrawScopeT2* g_supply_draw_scope_t2=nullptr;
static bool SupplyEmptyT2(std::uint64_t key) noexcept {
    return !key||key==Rd<std::uint64_t>(reinterpret_cast<void*>(WhBase()+0x5325B60),0,~0ull);
}
static bool SupplyInventoryUnitT2(const st2::Input& i,std::uint64_t key,int kind) noexcept {
    if(!key||key==i.beltKey||kind<0||kind>1||!Vft13(reinterpret_cast<void*>(i.manager),0x3EC9958)||
       !Vft13(reinterpret_cast<void*>(i.player),0x3AA4B98)||
       Rd<std::uintptr_t>(reinterpret_cast<void*>(i.player),0x9D0,0)!=i.manager||
       Rd<unsigned>(reinterpret_cast<void*>(i.player),0x30,0)!=i.entity||
       reinterpret_cast<std::uintptr_t>(HenryCharacter18())!=i.character)return false;
    void* player=nullptr;std::uint64_t right=0,left=0;
    if(!HandKey13(reinterpret_cast<void*>(i.manager),0,right,&player)||reinterpret_cast<std::uintptr_t>(player)!=i.player||
       !HandKey13(reinterpret_cast<void*>(i.manager),1,left)||!SupplyEmptyT2(right)||left==key)return false;
    void* item=Item13(key);void* definition=Rd<void*>(item,0x48,nullptr);void* cls=Rd<void*>(definition,0xE8,nullptr);void* inventory=nullptr;
    __try{inventory=Native13<void*(__fastcall*)(void*)>(np13::kActorInventory)(player);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    // The own Lua callback separately checks the exact optional item GUID and
    // current inventory enumeration. C++ repeats the native unit/owner/class
    // witnesses here, immediately before allowing that registered native call.
    return item&&inventory&&(Vft13(definition,0x3A4E390)||Vft13(definition,0x3A4D890))&&
        Rd<int>(cls,8,-1)==(kind==0?0:3)&&Rd<int>(item,0x50,0)==1&&
        !(Rd<unsigned>(item,0x60,~0u)&2u)&&Rd<void*>(item,0x90,nullptr)==inventory&&
        !Rd<void*>(item,0x98,reinterpret_cast<void*>(-1));
}
static bool __fastcall SupplyDrawDetourT2(void* manager,std::uint64_t key,int side) noexcept {
    auto* scope=g_supply_draw_scope_t2;
    if(!scope)return g_supply_draw_original_t2?g_supply_draw_original_t2(manager,key,side):false;
    // Only this invocation's CURRENT VM callback may reserve. Other callers of
    // the native wrapper are untouched outside this short synchronous scope.
    const bool own=KCD2VR_GRASP_CALLER()==WhBase()+0x2AB9CD1&&
        side==0&&reinterpret_cast<std::uintptr_t>(manager)==scope->input.manager;
    if(!own)return g_supply_draw_original_t2?g_supply_draw_original_t2(manager,key,side):false;
    ++scope->calls;
    if(scope->calls!=1||!scope->vm||!scope->serial||!g_native_ready13.load(std::memory_order_acquire)||!st2::enabled.load()||!weapon_release_t1::TestEnabledH21()||
       !st2::live(scope->input,GetTickCount64())||!SupplyInventoryUnitT2(scope->input,key,scope->kind))return false;
    scope->key=key;scope->submitted=true;
    __try{return g_supply_draw_original_t2?g_supply_draw_original_t2(manager,key,side):false;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static bool SupplyVisualAckT2(const st2::Input& in,std::uint64_t key,int kind,std::uint64_t serial,st2::Lease& out) noexcept {
    void* player=nullptr;std::uint64_t held=0;
    if(!st2::live(in,GetTickCount64())||!HandKey13(reinterpret_cast<void*>(in.manager),0,held,&player)||
       held!=key||reinterpret_cast<std::uintptr_t>(player)!=in.player||
       Rd<unsigned>(player,0x30,0)!=in.entity||
       reinterpret_cast<std::uintptr_t>(HenryCharacter18())!=in.character)return false;
    void* item=Item13(key);void* definition=Rd<void*>(item,0x48,nullptr);void* cls=Rd<void*>(definition,0xE8,nullptr);
    void* link=Rd<void*>(item,0x88,nullptr);const auto id=Rd<unsigned>(link,0x30,0);void* entity=Entity13(id);
    void* inventory=nullptr;
    __try{inventory=Native13<void*(__fastcall*)(void*)>(np13::kActorInventory)(player);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    if(!item||!inventory||Rd<int>(item,0x50,0)!=1||Rd<int>(cls,8,-1)!=(kind==0?0:3)||
       Rd<void*>(item,0x90,nullptr)!=inventory||Rd<void*>(item,0x98,reinterpret_cast<void*>(-1))||
       !id||!Vft13(entity,0x4072C78)||Rd<unsigned>(entity,0xC,0)!=id)return false;
    wrt1::Request r{};r.player=in.player;r.side=0;
    void* attachment=CanonicalWeaponAttachmentT1(r);void* object=Rd<void*>(attachment,0x20,nullptr);void* am=Rd<void*>(attachment,0x28,nullptr);
    if(!Vft13(attachment,0x3A657C0)||!Vft13(object,0x3A68F68)||Rd<unsigned>(object,8,0)!=id||
       reinterpret_cast<std::uintptr_t>(Rd<void*>(am,0x70,nullptr))!=in.character||Item13(key)!=item||
       Rd<void*>(item,0x88,nullptr)!=link||CanonicalWeaponAttachmentT1(r)!=attachment)return false;
    out={in.manager,in.player,in.character,reinterpret_cast<std::uintptr_t>(attachment),key,in.body,in.request,serial,
        GetTickCount64(),id,0,kind==0?0:3,true};return true;
}
static bool SupplyLeaseOwnsT2(const wrt1::Request& r) noexcept {
    st2::Lease l{};
    return g_supply_lease_t2.read(l)&&l.held&&st2::fresh(GetTickCount64(),l.tick,150)&&
        l.manager==r.manager&&l.player==r.player&&l.character==r.character&&l.key==r.key&&l.entity==r.entity&&
        l.attachment==r.attachment&&l.body==r.bodyEpoch&&l.request==r.requestEpoch&&l.side==r.side&&l.classId==r.classId;
}
static int SupplyPacketT2(const st2::Input& i,std::uint64_t serial,const char* action,int kind,char* out,std::size_t cap) noexcept {
    // Registered retail formatter: [Item-<low56>], never a Lua double WUID.
    char token[40]="-";
    if(i.beltKey&&(i.beltKey>>56)==2)std::snprintf(token,sizeof(token),"[Item-%llu]",(unsigned long long)(i.beltKey&0x00FFFFFFFFFFFFFFull));
    return std::snprintf(out,cap,"%llu|%s|%d|%u|%llu|%llu|%.5f|%.5f|%.5f|%.5f|%.5f|%.5f|%.5f|%.5f|%.5f|%s|%d|%d|%.5f|%.5f|%.5f|%.5f|%.5f|%.5f",
        (unsigned long long)serial,action,kind,i.entity,(unsigned long long)i.body,(unsigned long long)i.request,
        i.pos[0][0],i.pos[0][1],i.pos[0][2],i.pos[1][0],i.pos[1][1],i.pos[1][2],i.angles[0],i.angles[1],i.angles[2],token,int(i.beltShow),i.beltClass,
        i.beltPos[0],i.beltPos[1],i.beltPos[2],i.beltAngles[0],i.beltAngles[1],i.beltAngles[2]);
}
// No VM call per pose. Real visual changes may bypass 1Hz maintenance, but
// tracking/Grip chatter is coalesced to at most20Hz. Clear/draw/release edges
// retain priority; an uncertain draw is still never replayed.
struct SupplyVisualRefreshT12 {
    st2::Input prior{};bool selected=false,seen=false;std::uint64_t edgeAt=0;
    bool due(const st2::Input& in,bool active,std::uint64_t now,std::uint64_t renew,bool forced) noexcept {
        const bool changed=!seen||selected!=active||prior.beltKey!=in.beltKey||prior.beltShow!=in.beltShow||
            prior.body!=in.body||prior.request!=in.request||prior.player!=in.player||prior.character!=in.character;
        if(!forced&&now<renew&&(!changed||now<edgeAt))return false;
        prior=in;selected=active;seen=true;edgeAt=now+50;return true;
    }
};
struct SupplyReadinessReceiptT5 {char status[40]{};int daggers=-1,axes=-1;};
static bool SupplyReadinessParseT5(const char* text,std::size_t cap,std::uint64_t serial,SupplyReadinessReceiptT5& out) noexcept {
    if(!text||!std::memchr(text,0,cap))return false;
    SupplyReadinessReceiptT5 r{};const char* p=text;std::size_t n=0;
    while(*p&&*p!='|'){
        if(n+1>=sizeof(r.status)||!((*p>='A'&&*p<='Z')||*p=='_'||(*p>='0'&&*p<='9')))return false;
        r.status[n++]=*p++;
    }
    if(!n||*p++!='|')return false;
    const auto number=[&p](std::uint64_t max,std::uint64_t& value,char end) noexcept {
        if(*p<'0'||*p>'9')return false;value=0;
        do{const auto digit=std::uint64_t(*p++-'0');if(value>max/10||(value==max/10&&digit>max%10))return false;value=value*10+digit;}while(*p>='0'&&*p<='9');
        if(*p!=end)return false;if(end)++p;return true;
    };
    std::uint64_t receiptSerial=0,daggers=0,axes=0;
    if(!number(~std::uint64_t(0),receiptSerial,'|')||receiptSerial!=serial||
       !number(5,daggers,'|')||!number(3,axes,0))return false;
    r.daggers=int(daggers);r.axes=int(axes);out=r;return true;
}
static void SupplyReadinessTraceT5(const native_script_t2::Context& ctx,const st2::Input& in,bool current,
    bool called,const char* output,std::size_t cap,std::uint64_t serial) noexcept {
    // Same qualified script-owner callback as the actual operation. Never
    // interpret an unjoined Lua reply as actor or native inventory authority.
    struct State {st2::Input owner{};std::uint64_t vm=0;SupplyReadinessReceiptT5 receipt{};bool current=false,called=false,seen=false;};
    static State last{};static std::uint64_t loggedAt=0;
    State next{};next.vm=ctx.generation;next.current=current;next.called=called;
    if(current){next.owner.manager=in.manager;next.owner.player=in.player;next.owner.character=in.character;
        next.owner.entity=in.entity;next.owner.body=in.body;next.owner.request=in.request;}
    if(!called)std::memcpy(next.receipt.status,"CALLBACK_UNAVAILABLE",21);
    else if(!SupplyReadinessParseT5(output,cap,serial,next.receipt))std::memcpy(next.receipt.status,"BAD_RECEIPT",12);
    if(current&&called&&next.receipt.daggers>=0&&next.receipt.axes>=0){
        inventory_lifecycle_t8::daggers.store(next.receipt.daggers);inventory_lifecycle_t8::axes.store(next.receipt.axes);
    }
    const auto& a=last.owner;const auto& b=next.owner;
    const bool same=last.seen&&last.vm==next.vm&&last.current==next.current&&last.called==next.called&&
        a.manager==b.manager&&a.player==b.player&&a.character==b.character&&a.entity==b.entity&&a.body==b.body&&a.request==b.request&&
        last.receipt.daggers==next.receipt.daggers&&last.receipt.axes==next.receipt.axes&&
        !std::strcmp(last.receipt.status,next.receipt.status);
    if(same||(last.seen&&ctx.now>=loggedAt&&ctx.now-loggedAt<1000))return;
    // Only this change edge formats a diagnostic. The normal refresh is already
    // bounded to1Hz; rapid context/identity churn cannot increase log frequency.
    char m[384]{};std::snprintf(m,sizeof(m),
        "T5 SUPPLY_STATUS stage=READINESS current=%d actor=%u player=%llX body=%llu request=%llu vm=%llu status=%s daggers=%d axes=%d callback=%d authority=RECEIPT_ONLY",
        int(current),b.entity,(unsigned long long)b.player,(unsigned long long)b.body,(unsigned long long)b.request,
        (unsigned long long)ctx.generation,next.receipt.status,next.receipt.daggers,next.receipt.axes,int(called));
    diagnostic_trace::Write(m);next.seen=true;last=next;loggedAt=ctx.now;
}
static void SupplyScriptServiceT2(const native_script_t2::Context& ctx) noexcept {
    kcd2vr::subsystems::alchemy::feedback_d80::Service(ctx); // one queued verified herb sound, never a separate timer
    {   // R116: the weapon whoosh queued at the physical release (vrik/swing_whoosh_r116.h), next script update
        char replyR116[96]{},nameR116[kcd2vr::swing_whoosh_r116::kNameCap]{};
        const bool playedR116=kcd2vr::swing_whoosh_r116::Service(ctx,replyR116,sizeof(replyR116),nameR116);
        if(nameR116[0]&&kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Combat,ctx.now)){
            char m[260]{};std::snprintf(m,sizeof(m),"R116 SWING_WHOOSH trigger=%s played=%d reply=%.90s clipWhooshMuted=%llu clipWhooshPassed=%llu",
                nameR116,int(playedR116),replyR116[0]?replyR116:"STALE_OR_OFF",(unsigned long long)kcd2vr::swing_whoosh_r116::state.muted.load(),
                (unsigned long long)kcd2vr::swing_whoosh_r116::state.passed.load()); // R124: passed = clip flicks you still heard
            diagnostic_trace::Write(m);}
    }
    // INVESTIGATE_D67R1 KNIFE_SOURCE: D66's complete latest capture has no
    // T18/K35/D65 preview chain. Join current source publication, this actual
    // script callback/VM generation, enable/location, left-hand occupancy,
    // stock and first admission refusal before changing item classification.
    // Historical RIGHT equipment draw is not current LEFT Ammo preview; keep
    // finite reservation -> held readback -> once-only native launch/cost.
    // See repair_evidence/d67r1/OPEN_SOURCE_POINTS.txt (KNIFE_SOURCE).
    // This state has exactly one writer: the verified script update callback.
    static st2::Gesture gesture{};static st2::Lease lease{};
    static st2::Input pendingInput{};static std::uint64_t pendingKey=0,pendingSerial=0,pendingAt=0,serial=0,next=0,vm=0,lastCommit=0;
    static int pendingKind=-1;static bool visible=false,release=false,pendingUncertain=false;
    if(kcd2vr::ammo_service_t22::integrated)AmmoServiceReadinessT22(ctx);
    st2::Input in{};const bool haveInput=g_supply_input_t2.read(in);
    if(AmmoServiceOwnsT22()){AmmoServiceScriptT22(ctx,in);return;}
    if(!haveInput)return;
    if(vm!=ctx.generation){vm=ctx.generation;gesture={};lease={};pendingKey=0;pendingSerial=0;pendingKind=-1;pendingUncertain=false;next=0;visible=false;release=true;}
    const bool selected=weapon_release_t1::TestEnabledH21()&&st2::enabled.load(std::memory_order_acquire)&&g_native_ready13.load()&&st2::live(in,ctx.now);
    static std::uint64_t transferSeenT13[2]{};
    for(int side=0;side<2;++side){DropTransferT13 t{};
        if(!g_drop_transfer_t13[side].read(t)||!t.serial||t.serial<=transferSeenT13[side])continue;
        transferSeenT13[side]=t.serial;
        const auto matches=[&](const st2::Input& owner,std::uint64_t key) noexcept {
            return key&&t.source.key==key&&t.source.manager==owner.manager&&t.source.player==owner.player&&
                t.source.character==owner.character&&t.source.bodyEpoch==owner.body;
        };
        st2::Input heldOwner{};heldOwner.manager=lease.manager;heldOwner.player=lease.player;heldOwner.character=lease.character;heldOwner.body=lease.body;
        if((lease.held&&matches(heldOwner,lease.key))||(pendingKey&&matches(pendingInput,pendingKey))){
            lease={};pendingKey=0;pendingSerial=0;pendingKind=-1;pendingUncertain=false;release=true;next=0;
        }
    }
    wrt1::CommittedT2 committed{};
    if(g_committed_drop_t2.read(committed)&&committed.serial>lastCommit){lastCommit=committed.serial;
        if((lease.held&&committed.source.key==lease.key)||(pendingKey&&committed.source.key==pendingKey)){
            lease={};pendingKey=0;pendingSerial=0;pendingKind=-1;release=true;next=0;
        }}
    if(lease.held&&st2::live(in,ctx.now)){st2::Lease actual{};
        if(lease.player==in.player&&lease.character==in.character&&SupplyVisualAckT2(in,lease.key,lease.classId==0?0:1,lease.serial,actual))lease=actual;
        else {lease={};release=true;next=0;}
    }
    if(pendingKey){
        const bool live=st2::live(in,ctx.now);
        const bool same=pendingInput.manager==in.manager&&pendingInput.player==in.player&&pendingInput.character==in.character;
        if(live&&same&&SupplyVisualAckT2(in,pendingKey,pendingKind,pendingSerial,lease)){
            char m[224]{};std::snprintf(m,sizeof(m),"T2 SUPPLY stage=HELD_ACK serial=%llu key=%016llX actualHand=RIGHT class=%d attachment=VERIFIED",
                (unsigned long long)pendingSerial,(unsigned long long)pendingKey,lease.classId);diagnostic_trace::Write(m);
            pendingKey=0;pendingSerial=0;pendingKind=-1;pendingUncertain=false;next=0;
        }else if(live&&!same){
            diagnostic_trace::Write("T2 SUPPLY stage=RETIRED reason=NATIVE_ACTOR_OR_CHARACTER_REPLACED retry=NONE nativeHand=UNMODIFIED");
            pendingKey=0;pendingSerial=0;pendingKind=-1;pendingUncertain=false;release=true;gesture.armed=false;next=0;
        }else if(!pendingUncertain&&!st2::fresh(ctx.now,pendingAt,1500)){
            // A timeout cannot prove native queued attachment was cancelled.
            // Keep THIS reservation, suppress further draws, and accept only a
            // later actual same-unit hand+visual ACK (or actor replacement).
            pendingUncertain=true;gesture.armed=false;
            diagnostic_trace::Write("T2 SUPPLY stage=UNCERTAIN reason=DRAW_ACK_NOT_YET_OBSERVED reservation=RETAINED retry=NONE nativeHand=UNMODIFIED");
        }
    }
    (void)g_supply_lease_t2.publish(st2::live(in,ctx.now)?lease:st2::Lease{});
    st2::nativeLease.store(lease.held||pendingKey,std::memory_order_release);
    const int desire=gesture.next(in,ctx.now,selected,pendingKey||lease.held);
    const bool draw=desire>=0&&desire<2;
    g_supply_cleanup_t2.store(selected||visible||release||lease.held||pendingKey,std::memory_order_release);
    if(!selected&&!visible&&!release)return;
    static SupplyVisualRefreshT12 visual{};
    if(!visual.due(in,selected,ctx.now,next,draw||release||!selected))return;
    const char* action=!selected?"clear":release?"release":draw?"draw":"refresh";
    const int kind=draw?desire:0;char input[512]{},output[160]{};const auto callSerial=++serial;
    const int len=SupplyPacketT2(in,callSerial,action,kind,input,sizeof(input));
    if(len<=0||std::size_t(len)>=sizeof(input))return;
    SupplyDrawScopeT2 scope{in,callSerial,ctx.generation,0,kind};
    if(draw)g_supply_draw_scope_t2=&scope;
    const bool called=ctx.CallPacket("kcd2vr_supply_native_t2",input,output,sizeof(output));
    g_supply_draw_scope_t2=nullptr;
    SupplyReadinessTraceT5(ctx,in,selected,called,output,sizeof(output),callSerial);
    next=ctx.now+1000;visible=selected;release=false;
    if(draw){gesture.armed=false;
        if(scope.submitted&&scope.calls==1&&scope.key){pendingInput=in;pendingKey=scope.key;pendingKind=kind;pendingSerial=callSerial;pendingAt=ctx.now;pendingUncertain=false;}
        char m[256]{};std::snprintf(m,sizeof(m),"T2 SUPPLY stage=DRAW_REQUEST serial=%llu kind=%d calls=%d nativeSubmitted=%d callback=%d receipt=%.100s",
            (unsigned long long)callSerial,kind,scope.calls,scope.submitted,called,called?output:"UNAVAILABLE");diagnostic_trace::Write(m);
        // Even errors AFTER the native request are spent. Never retry this edge.
        if(!scope.submitted){release=true;next=ctx.now+1000;}
    }
    g_supply_cleanup_t2.store(selected||visible||release||lease.held||pendingKey,std::memory_order_release);
}
static bool SupplyWantedT2() noexcept {return kcd2vr::subsystems::alchemy::feedback_d80::Wanted()||kcd2vr::swing_whoosh_r116::Wanted()||AmmoServiceOwnsT22()||(kcd2vr::ammo_service_t22::integrated&&AmmoServiceReadinessWantedT22())||st2::enabled.load(std::memory_order_acquire)||g_supply_cleanup_t2.load(std::memory_order_acquire);}
#include "inventory_delivery_t7.inl"
static bool InstallSupplyT2() noexcept {
    kcd2vr::subsystems::alchemy::feedback_d80::transportReady.store(false,std::memory_order_release);
    if(!inventory_t7::Install())diagnostic_trace::Write("T7 INVENTORY callback=UNAVAILABLE noItemsGranted=1");
    if(!g_supply_draw_hook_t2){const auto b=WhBase();
        const unsigned char pin[]{0x48,0x83,0xec,0x28,0x4c,0x8b,0xd9,0x48,0x89,0x54,0x24,0x38};
        const unsigned char caller[]{0xe8,0x77,0x4d,0xe1,0xfd,0x48,0x8b,7};
        if(!retail_build112::Ready(b)||!MatchBytes(b+0x8CEA48,pin,sizeof(pin))||!MatchBytes(b+0x2AB9CCC,caller,sizeof(caller)))return false;
        DetourModKit::hook::InlineRequest r{"kcd2vr_t2_supply_explicit_hand",DetourModKit::hook::Target{DetourModKit::Address{b+0x8CEA48}},DetourModKit::hook::Options{}};
        auto h=DetourModKit::hook::inline_at(std::move(r),&SupplyDrawDetourT2);if(!h)return false;
        g_supply_draw_hook_t2=std::move(*h);g_supply_draw_original_t2=g_supply_draw_hook_t2->original<SupplyDrawFnT2>();
        if(!g_supply_draw_original_t2||!PinOwnCallbackT2(reinterpret_cast<const void*>(&SupplyDrawDetourT2))||!g_supply_draw_hook_t2->enable()||!PublishSupplyDrawEntry62(b))return false;
    }
    // Original trampolines and hooks have process lifetime, including a native
    // owner re-install. Every callback is pass-through when its client is idle.
    const bool registered=g_supply_draw_original_t2&&g_supply_draw_hook_t2->is_enabled()&&SupplyDrawEntryQualified62(WhBase())&&
        native_script_t2::Register(&SupplyScriptServiceT2,&SupplyWantedT2)&&native_script_t2::Install();
    kcd2vr::subsystems::alchemy::feedback_d80::transportReady.store(registered,std::memory_order_release);
    return registered;
}
