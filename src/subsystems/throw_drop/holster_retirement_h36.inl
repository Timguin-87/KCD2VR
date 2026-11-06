// Equipment mutation shares Henry's native attachment manager. A LEFT hand
// never borrows a live/retiring RIGHT collider, but its retired acknowledgement
// need not clear the independent RIGHT release rearm before LEFT can operate.
static bool PhysLeftHolsterRetiredH36(const ph::Input& in,const PhysEquipment6698& e) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if(!e.valid||!e.player||!e.manager||e.count<0||e.count>int(e.items.size())||
       e.epoch!=in.bodyEpoch||e.request!=in.epoch||in.bodyEpoch!=g_pose_epoch6689.load(std::memory_order_acquire)||
       in.epoch!=g_weapon_request_epoch6694.load(std::memory_order_acquire)||
       !in.tick||now<in.tick||now-in.tick>100||!e.tick||now<e.tick||now-e.tick>100||
       !e.handKnown[0]||!e.handKnown[1]||e.heldIndex[0]<0||e.heldIndex[0]>=e.count)return false;
    const auto admission=heldT4::AdmissionSerial();heldT4::Achieved retired{};
    if(NativeHeldBusyT4()||!heldT4::Read(retired)||!retired.colliderRetired||NativeHeldBusyT4()||
       admission!=heldT4::AdmissionSerial())return false;
    const auto& owner=retired.owner;const auto& item=e.items[e.heldIndex[0]];
    return owner.side==0&&owner.key&&owner.player==reinterpret_cast<std::uintptr_t>(e.player)&&
        owner.manager==reinterpret_cast<std::uintptr_t>(e.manager)&&owner.epoch==in.bodyEpoch&&owner.request==in.epoch&&
        owner.character&&owner.character==g_player_char37.load(std::memory_order_acquire)&&
        item.held&&item.profile.side==0&&e.handKey[0]==owner.key&&item.profile.key==owner.key&&
        item.profile.generation==owner.generation&&item.profile.entity==owner.entityId;
}
#include "holster_completion_h36.inl"
// Existing recursive-mutex native consumer owns every record. A newer command
// never overwrites or acknowledges an older entered/uncertain operation.
struct LeftEquipmentObligationH36 {
    ph::Command command{};void* player=nullptr;void* manager=nullptr;
    std::uintptr_t character=0;bool active=false,entered=false,uncertain=false;
    bool precise=false,returned=false,established=false;LeftBindingH36 before{};
    bool preparedD48=false,selectionEnteredD48=false,selectionReturnedD48=false,selectionConfirmedD48=false;
    kcd2vr::initial_draw_h36r3::Watch initialDrawH36r3{};
    LeftStowDestinationsH36 stowDestinations{};
};
static LeftEquipmentObligationH36 g_left_equipment_obligation_h36;
static std::array<LeftEquipmentObligationH36,7> g_left_equipment_successors_h36{};
static LeftBindingH36 g_left_equipment_established_h36{};
static bool g_left_equipment_overflow_h36=false;
static bool SameLeftEquipmentH36(const LeftEquipmentObligationH36& o,const PhysEquipment6698& e,
                                 const ph::Command& c) noexcept {
    return o.active&&o.player==e.player&&o.manager==e.manager&&o.command.kind==c.kind&&
        o.command.side==c.side&&o.command.set==c.set&&o.command.key==c.key&&o.command.generation==c.generation&&
        o.command.epoch==c.epoch&&o.command.bodyEpoch==c.bodyEpoch&&o.command.sample==c.sample&&o.command.tick==c.tick;
}
static LeftEquipmentObligationH36* FindLeftEquipmentH36(const PhysEquipment6698& e,const ph::Command& c) noexcept {
    if(SameLeftEquipmentH36(g_left_equipment_obligation_h36,e,c))return &g_left_equipment_obligation_h36;
    for(auto& o:g_left_equipment_successors_h36)if(SameLeftEquipmentH36(o,e,c))return &o;return nullptr;
}
static bool LeftEquipmentPendingH36() noexcept {
    if(g_left_equipment_overflow_h36||g_left_equipment_obligation_h36.active)return true;
    for(const auto& o:g_left_equipment_successors_h36)if(o.active)return true;return false;
}
static bool LeftEquipmentContextH36(const LeftEquipmentObligationH36& o,const PhysEquipment6698& e,std::uint64_t now) noexcept {
    return e.valid&&e.count>=0&&e.count<=int(e.items.size())&&e.tick&&now>=e.tick&&now-e.tick<=100&&
        e.player==o.player&&e.manager==o.manager&&e.epoch==o.command.bodyEpoch&&e.request==o.command.epoch&&
        e.epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&e.request==g_weapon_request_epoch6694.load(std::memory_order_acquire)&&
        o.character&&o.character==g_player_char37.load(std::memory_order_acquire)&&e.handKnown[1];
}
static void RefreshLeftEquipmentRecordH36(LeftEquipmentObligationH36& o,const PhysEquipment6698& e,std::uint64_t now) noexcept {
    if(!o.active)return;
    // This deadline applies only to the original command which NEVER invoked.
    // An entered native job is never completed by time, OFF, key or new epoch.
    if(!o.entered&&(!o.selectionEnteredD48||o.selectionReturnedD48)&&
       now>=o.command.tick&&now-o.command.tick>100&&!NativeHeldBusyT4()){
        kcd2vr::initial_draw_h36r3::retire(o.initialDrawH36r3);o={};return;
    }
    if(!o.entered||!o.precise||!o.returned||o.uncertain||!o.before.valid||NativeHeldBusyT4()||!LeftEquipmentContextH36(o,e,now))return;
    const auto* item=LeftItemH36(e,o.command,false);if(!item||item->item!=o.before.item)return;
    if(o.initialDrawH36r3.serial){
        namespace initial=kcd2vr::initial_draw_h36r3;initial::Returned original{};LeftBindingH36 live{};
        auto current=o.command;current.generation=item->profile.generation;
        if(!initial::read(o.initialDrawH36r3,original)||original.original.membership==UINT64_MAX||
           g_left_native_membership_h36.load(std::memory_order_acquire)!=original.original.membership+1||
           e.handKey[1]!=o.command.key||!item->held||!PhysEquipmentSlot126(e,current)||!ReadLeftBindingH36(e,current,live)||
           live.item!=o.before.item||live.link!=reinterpret_cast<void*>(original.link)||
           live.entity!=reinterpret_cast<void*>(original.entity)||item->profile.entity!=original.entityId||
           !SameSocketGraphH36(live.hand,o.before.hand)||!live.hand.object||live.hand.entity!=original.entityId||
           !SocketStillCurrentH36(live.hand)||!CurrentEquipmentKeysH36(e)||!LeftEquipmentContextH36(o,e,now))return;
        initial::retire(o.initialDrawH36r3);g_left_equipment_established_h36=live;o={};return;
    }
    auto current=o.command;current.generation=item->profile.generation;
    if(!PhysEquipmentSlot126(e,current))return;
    LeftSocketH36 hand{},stowed{};
    if(!ReadLeftHandH36(e,hand)||!SameSocketGraphH36(hand,o.before.hand))return;
    if(o.command.kind==ph::CommandKind::Draw){
        // Preexisting world-link =>8D0848 runs the first-stage native callback
        // synchronously. That callback submits exactly one actor attachment.
        // Positive current binding + original stowed binding gone closes both
        // observed submissions, without guessing actor queue/thread ordering.
        LeftBindingH36 live{};
        if(o.before.nativeMembership==UINT64_MAX||g_left_native_membership_h36.load(std::memory_order_acquire)!=o.before.nativeMembership+1||
           e.handKey[1]!=o.command.key||!item->held||item->profile.generation!=o.before.generation||
           !ReadLeftBindingH36(e,current,live)||live.link!=o.before.link||live.entity!=o.before.entity||
           !hand.object||hand.entity!=o.before.stowed.entity||
           !CurrentOriginalStowedH36(e,o.before,stowed)||stowed.object||
           !SocketStillCurrentH36(hand)||!SocketStillCurrentH36(stowed)||
           !CurrentEquipmentKeysH36(e)||!LeftEquipmentContextH36(o,e,now))return;
        live.hand=hand;live.stowed=stowed;live.stowedName=o.before.stowedName;live.stowedIndex=o.before.stowedIndex;
        g_left_equipment_established_h36=live;o={};return;
    }
    if(o.command.kind!=ph::CommandKind::Holster||!o.established||e.handKey[1]||hand.object||
       g_left_native_membership_h36.load(std::memory_order_acquire)!=o.before.nativeMembership)return;
    std::uint64_t key=0;void* link=nullptr;
    if(!Read(static_cast<unsigned char*>(item->item)+0x30,key)||key!=o.command.key||
       !Read(static_cast<unsigned char*>(item->item)+0x88,link))return;
    if(link){
        // Special STOW keeps its world-link and submits a NEW stowed attachment.
        // An original destination socket was proven empty immediately before
        // this STOW. Its new real matching binding is required.
        LeftBindingH36 live{};
        if(link!=o.before.link||item->profile.generation!=o.before.generation||!ReadLeftBindingH36(e,current,live)||
           live.entity!=o.before.entity||live.hand.object||!SameSocketGraphH36(live.hand,hand)||!CurrentStowDestinationH36(e,o.before,o.stowDestinations,stowed)||!stowed.object||
           stowed.entity!=o.before.hand.entity)return;
    }
    else if(item->worldLink54||item->profile.entity||item->entity)return;
    // Ordinary STOW destroys its item-world link (479745); special STOW has
    // now acknowledged both actual attachment transitions. Retired pointers
    // have only been compared, never dereferenced through the old record.
    if(!SocketStillCurrentH36(hand)||(link&&!SocketStillCurrentH36(stowed))||
       !CurrentEquipmentKeysH36(e)||!LeftEquipmentContextH36(o,e,now))return;
    o={};
}
static void PhysLeftEquipmentRefreshH36(const PhysEquipment6698& e,std::uint64_t now) noexcept {
    RefreshLeftEquipmentRecordH36(g_left_equipment_obligation_h36,e,now);
    for(auto& o:g_left_equipment_successors_h36)RefreshLeftEquipmentRecordH36(o,e,now);
    if(LeftEquipmentPendingH36())return;
    if(g_native_held_equipment_pending_h36.load(std::memory_order_acquire)){
        NativeHeldInvalidateT4();heldT4::SetEquipmentAdmissionSuspendedH36(false);
        g_native_held_equipment_pending_h36.store(false,std::memory_order_release);
    }
}
// D48: selection is a separate synchronous operation. Exact retail 8EEA04
// calls read-only 8FE304, then conditionally writes manager+40; neither leaf
// submits an attachment or world job. An abnormal return still has no receipt.
// Only a returned selection with exact readback can precede this command's DRAW.
static void PhysTraceLeftEquipmentD48(const PhysEquipment6698& e,const ph::Command& c,
                                      const char* stage,const char* reason) noexcept {
    const auto now=std::uint64_t(GetTickCount64());if(!PhysHolsterCaptureD48(now))return;
    namespace cap=kcd2vr::diagnostics::capture168;
    const auto domain=cap::active(cap::Domain::Interaction,now)?cap::Domain::Interaction:cap::Domain::Bug;
    const auto session=(cap::gate.session[unsigned(domain)].load(std::memory_order_relaxed)<<1)|
        (domain==cap::Domain::Interaction?1u:0u);
    static std::uint64_t previous=0;static unsigned lines=0;
    if(previous!=session){previous=session;lines=0;}if(lines>=96)return;++lines;
    const auto* o=FindLeftEquipmentH36(e,c);char line[640]{};
    std::snprintf(line,sizeof(line),
        "D48 HOLSTER_REQUEST stage=%s reason=%s side=%d set=%d key=%016llX generation=%llu sample=%llu prepared=%d entered=%d returned=%d precise=%d before=%d uncertain=%d selector=%d/%d/%d nativeAck=NOT_INFERRED budget=%u/96",
        stage,reason,c.side,c.set,(unsigned long long)c.key,(unsigned long long)c.generation,
        (unsigned long long)c.sample,o&&o->preparedD48,o&&o->entered,o&&o->returned,o&&o->precise,
        o&&o->before.valid,o&&o->uncertain,o&&o->selectionEnteredD48,o&&o->selectionReturnedD48,
        o&&o->selectionConfirmedD48,lines);diagnostic_trace::Write(line);
}
template<class Invoke> static bool PhysSelectEquipmentInvokeD48(const PhysEquipment6698& e,
    const ph::Command& c,Invoke&& invoke) {
    auto* o=c.side==1?FindLeftEquipmentH36(e,c):nullptr;
    if(o){
        if(o->entered||o->preparedD48||o->selectionEnteredD48||o->uncertain){
            PhysTraceLeftEquipmentD48(e,c,"SELECT_REFUSED","ORIGINAL_OPERATION_ALREADY_STARTED");return false;
        }
        o->selectionEnteredD48=true;
    }else if(c.side==1&&g_native_held_equipment_pending_h36.load(std::memory_order_acquire))return false;
    PhysTraceLeftEquipmentD48(e,c,"SELECT_ENTERED","SYNCHRONOUS_SET_INDEX");
    invoke(); // original native wrapper; exceptions retain selector-entry debt
    if(o)o->selectionReturnedD48=true;
    int selected=-1;const bool confirmed=Read(static_cast<unsigned char*>(e.manager)+0x40,selected)&&selected==c.set;
    if(o)o->selectionConfirmedD48=confirmed;
    PhysTraceLeftEquipmentD48(e,c,"SELECT_RETURNED",confirmed?"EXACT_SET_READBACK":"SET_NOT_CONFIRMED");
    return confirmed;
}
static bool PhysLeftEquipmentBeginH36(const PhysEquipment6698& e,const ph::Command& c,bool precise) noexcept {
    if(c.side!=1||(c.kind!=ph::CommandKind::Draw&&c.kind!=ph::CommandKind::Holster))return true;
    auto* o=FindLeftEquipmentH36(e,c);
    if(!o)return !g_native_held_equipment_pending_h36.load(std::memory_order_acquire);
    if(o->entered||o->preparedD48||o->uncertain||
       (o->selectionEnteredD48&&(!o->selectionReturnedD48||!o->selectionConfirmedD48))){
        PhysTraceLeftEquipmentD48(e,c,"PREPARE_REFUSED","ORIGINAL_OPERATION_ALREADY_STARTED_OR_SELECTION_UNKNOWN");return false;
    }
    // Preparation reads may fail before native entry. They do not create an
    // unknown entered job, alter an older receipt, or authorize a native call.
    LeftBindingH36 before{};LeftStowDestinationsH36 destinations{};
    kcd2vr::initial_draw_h36r3::Watch initial{};bool established=false;
    if(precise){
        if(!ReadLeftBindingH36(e,c,before)){
            if(c.kind!=ph::CommandKind::Draw||!ReadInitialLeftDrawH36r3(e,c,before,initial)){
                PhysTraceLeftEquipmentD48(e,c,"PREPARE_REFUSED","BINDING_OR_INITIAL_DRAW_UNPROVEN");return false;
            }
        }else if(c.kind==ph::CommandKind::Draw){
            if(before.hand.object||!FindOriginalStowedH36(e,before)||!SocketStillCurrentH36(before.hand)||
               !SocketStillCurrentH36(before.stowed)||!CurrentEquipmentKeysH36(e)){
                PhysTraceLeftEquipmentD48(e,c,"PREPARE_REFUSED","ORIGINAL_STOWED_GRAPH_UNPROVEN");return false;
            }
        }else{
            if(e.handKey[1]!=c.key||!CaptureStowDestinationsH36(e,before,destinations)){
                PhysTraceLeftEquipmentD48(e,c,"PREPARE_REFUSED","STOW_DESTINATION_UNPROVEN");return false;
            }
            established=true;
        }
    }
    o->before=before;o->initialDrawH36r3=initial;o->stowDestinations=destinations;
    o->established=established;o->precise=precise;o->preparedD48=true;
    PhysTraceLeftEquipmentD48(e,c,"PREPARED","NATIVE_NOT_ENTERED");return true;
}
static bool PhysLeftEquipmentEnterH36(const PhysEquipment6698& e,const ph::Command& c) noexcept {
    if(c.side!=1||(c.kind!=ph::CommandKind::Draw&&c.kind!=ph::CommandKind::Holster))return true;
    auto* o=FindLeftEquipmentH36(e,c);
    if(!o)return !g_native_held_equipment_pending_h36.load(std::memory_order_acquire);
    if(!o->preparedD48||o->entered||o->uncertain)return false;
    // Supersession is relevant only at an actual attachment invocation. A
    // selector or failed preflight cannot invalidate an older native receipt.
    auto mark=[&](LeftEquipmentObligationH36& old){if(old.active&&old.entered&& &old!=o)old.uncertain=true;};
    mark(g_left_equipment_obligation_h36);for(auto& old:g_left_equipment_successors_h36)mark(old);
    o->entered=true;g_left_equipment_established_h36={};
    PhysTraceLeftEquipmentD48(e,c,"ENTERED","ORIGINAL_ATTACHMENT_CALL");return true;
}
static void PhysLeftEquipmentReturnedH36(const PhysEquipment6698& e,const ph::Command& c) noexcept {
    if(auto* o=FindLeftEquipmentH36(e,c);o&&o->entered&&o->precise){
        const auto observed=g_left_native_membership_h36.load(std::memory_order_acquire);
        o->returned=o->before.valid&&(c.kind==ph::CommandKind::Draw?o->before.nativeMembership!=UINT64_MAX&&observed==o->before.nativeMembership+1:observed==o->before.nativeMembership);
    }
}
template<class Invoke> static bool PhysLeftEquipmentInvokeH36(const PhysEquipment6698& e,const ph::Command& c,Invoke&& invoke) {
    if(!PhysLeftEquipmentBeginH36(e,c,true))return false;
    kcd2vr::initial_draw_h36r3::Watch originalInitial{};
    if(auto* o=FindLeftEquipmentH36(e,c);o&&o->preparedD48)originalInitial=o->initialDrawH36r3;
    using OriginalCallH36r3=std::remove_reference_t<Invoke>;
    struct InvocationD48{const PhysEquipment6698* equipment;const ph::Command* command;OriginalCallH36r3* call;};
    InvocationD48 original{&e,&c,std::addressof(invoke)};
    const bool returned=kcd2vr::initial_draw_h36r3::invokeScoped(originalInitial,
        +[](void* call)->bool{auto& x=*static_cast<InvocationD48*>(call);
            return PhysLeftEquipmentEnterH36(*x.equipment,*x.command)&&(*x.call)();},
        &original); // same SEH-safe original scope; no abnormal return ACK
    if(returned)PhysLeftEquipmentReturnedH36(e,c);
    PhysTraceLeftEquipmentD48(e,c,"RETURNED",returned?"ORIGINAL_RETURNED":"ORIGINAL_FALSE_OR_NOT_ENTERED");return returned;
}
// D48: an open grip can retire only the exact operation that has never
// entered native code. Entered/uncertain records and their completion probes
// survive release unchanged. No timeout or local gesture reset acknowledges them.
static bool PhysCancelUnenteredEquipmentH36(const PhysEquipment6698& e,const ph::Command& command) noexcept {
    auto* record=FindLeftEquipmentH36(e,command);
    if(record&&(record->entered||(record->selectionEnteredD48&&!record->selectionReturnedD48)))return false;
    if(record){kcd2vr::initial_draw_h36r3::retire(record->initialDrawH36r3);*record={};}
    PhysLeftEquipmentRefreshH36(e,std::uint64_t(GetTickCount64()));
    return true;
}
enum class HolsterNativeH36 {Ready,Wait,Discard};
static HolsterNativeH36 PhysLeftEquipmentH36(const PhysFrame6698& frame,const PhysEquipment6698& e,
                                           const ph::Command& command) noexcept {
    if(command.side!=1||(command.kind!=ph::CommandKind::Draw&&command.kind!=ph::CommandKind::Holster))return HolsterNativeH36::Ready;
    if(!heldT4::Enabled()&&!NativeHeldBusyT4()&&!g_native_held_release_inhibit_t4.load(std::memory_order_acquire)&&
       !g_native_held_equipment_pending_h36.load(std::memory_order_acquire))return HolsterNativeH36::Ready;
    if(!frame.input.allowed||!frame.input.tracked[1]||!frame.input.intentTracked[1]||!e.valid||
       frame.player!=e.player||frame.manager!=e.manager||command.bodyEpoch!=e.epoch||command.epoch!=e.request||
       !e.handKnown[1]||!PhysEquipmentSlot126(e,command)||
       (command.kind==ph::CommandKind::Draw?e.handKey[1]!=0:e.handKey[1]!=command.key))return HolsterNativeH36::Discard;
    if(!FindLeftEquipmentH36(e,command)){
        LeftEquipmentObligationH36* free=nullptr;
        if(!g_left_equipment_obligation_h36.active)free=&g_left_equipment_obligation_h36;
        else for(auto& old:g_left_equipment_successors_h36)if(!old.active){free=&old;break;}
        if(free)*free={command,e.player,e.manager,g_player_char37.load(std::memory_order_acquire),true,false,false};
        else{
            // Capacity refusal is not native entry and cannot mint permanent
            // unknown work. Preserve every original debt and leave this exact
            // unentered command queued under its existing freshness limit.
            PhysTraceLeftEquipmentD48(e,command,"LEDGER_WAIT","ALL_ORIGINAL_RECEIPTS_RETAINED");
            return HolsterNativeH36::Wait;
        }
    }
    g_native_held_equipment_pending_h36.store(true,std::memory_order_release);
    heldT4::SetEquipmentAdmissionSuspendedH36(true);NativeHeldInvalidateT4();
    if(NativeHeldBusyT4())return HolsterNativeH36::Wait;
    if(g_native_held_release_inhibit_t4.load(std::memory_order_acquire)&&!PhysLeftHolsterRetiredH36(frame.input,e))return HolsterNativeH36::Wait;
    return HolsterNativeH36::Ready;
}
