// Included inside interaction_alignment.cpp's existing anonymous namespace.
// Install only after retail112 fingerprint + all entry/callsite/vtable pins.
// This observes already-enumerated AlchemyItem geometry, then accommodates a
// source-qualified close-palm nomination at facing and descriptor-ray input.
// Native eligibility, held-item exclusion, final selection and verbs remain.
namespace selection68=kcd2vr::subsystems::alchemy::native_selection68;
namespace craft68=kcd2vr::subsystems::alchemy::bridge67;
namespace geometry68=kcd2vr::subsystems::alchemy::geometry68;
namespace headR116=kcd2vr::subsystems::alchemy::head_select_r116; // R116 view/hand selection
using Selector68Fn=std::uintptr_t(__fastcall*)(std::uintptr_t,std::uintptr_t,std::uintptr_t,std::uintptr_t,unsigned);
using Bounds68Fn=std::uintptr_t(__fastcall*)(std::uintptr_t,std::uintptr_t);
using Facing68Fn=char(__fastcall*)(std::uintptr_t,std::uintptr_t,std::uintptr_t);
static Selector68Fn g_selector_original68=nullptr;
static Bounds68Fn g_bounds_original68=nullptr;
static Facing68Fn g_facing_original68=nullptr;
static std::atomic<bool> g_selection_ready68{false};
static thread_local selection68::Scope g_selection_scope68{};
static thread_local unsigned g_selection_depth68=0;

static bool SelectionCopy68(std::uintptr_t address,void* out,std::size_t n) noexcept {
    if(!address||!out||!n)return false;
    __try {std::memcpy(out,reinterpret_cast<const void*>(address),n);return true;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
template<class T>static bool SelectionRead68(std::uintptr_t address,T& out) noexcept {
    return SelectionCopy68(address,&out,sizeof(out));
}
static bool SelectionVft68(std::uintptr_t p,std::uintptr_t rva) noexcept {
    std::uintptr_t vf=0;return g_wh_base&&SelectionRead68(p,vf)&&vf==g_wh_base+rva;
}
// Map layout is the native 16-byte key/pointer vector read by63D504:
// interactor+D8=data, +E0=count. This never calls the lookup or inserts a key.
static bool SelectionPolicy68(std::uintptr_t interactor,std::uintptr_t player,
                              std::uintptr_t& policy) noexcept {
    policy=0;std::uintptr_t back=0,data=0,count=0;
    if(!SelectionVft68(interactor,0x3A6A0C8)||!SelectionRead68(interactor+8,back)||back!=player||
       !SelectionVft68(player,0x3AA4B98)||!SelectionRead68(player+0xB08,back)||back!=interactor||
       !SelectionRead68(interactor+0xD8,data)||!SelectionRead68(interactor+0xE0,count)||!data||!count||count>16)return false;
    // Supported retail constructor installs exactly object1/dog2/chat4. The
    // bounded scan tolerates additional native entries but never reads a world.
    for(std::uintptr_t i=0;i<count;++i){unsigned key=0;std::uintptr_t p=0;
        if(!SelectionRead68(data+16*i,key))return false;
        if(key!=1)continue;
        if(policy||!SelectionRead68(data+16*i+8,p)||!SelectionVft68(p,0x3A69D90)||
           !SelectionRead68(p+8,back)||back!=player)return false;
        policy=p;
    }
    return policy!=0;
}
// Door scope does not depend on the separate alchemy table. Exact caller,
// key, native owner graph and outermost collector are still required.
static kcd2vr::door_collector_c54::Scope DoorCollectorBeginC54(std::uintptr_t interactor,unsigned key,std::uintptr_t caller,unsigned parentDepth,const char** refusal=nullptr) noexcept {
 kcd2vr::door_collector_c54::Scope s{};std::uintptr_t player=0,policy=0,entity=0;
 if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Interaction,GetTickCount64()))return s;
 const auto reject=[&](const char* reason) noexcept {if(refusal)*refusal=reason;return s;};
 if(parentDepth)return reject("NESTED_SELECTOR");
 if(!g_selection_ready68.load(std::memory_order_acquire))return reject("EXISTING_SELECTOR_NOT_READY");
 if(!g_enabled.load(std::memory_order_acquire))return reject("INTERACTION_DISABLED");
 if(key!=1)return reject("NON_OBJECT_POLICY");
 if(caller!=g_wh_base+0x7F5727)return reject("CALLER_NOT_7F5727");
 if(!SelectionRead68(interactor+8,player))return reject("INTERACTOR_PLAYER_READ_FAILED");
 if(!SelectionPolicy68(interactor,player,policy))return reject("CURRENT_NATIVE_PLAYER_POLICY_GRAPH_REFUSED");
 if(!SelectionRead68(player+0x38,entity))return reject("PLAYER_ENTITY_READ_FAILED");
 if(!SelectionVft68(entity,0x4072C78))return reject("HENRY_ENTITY_VFT_REFUSED");
 s={interactor,player,policy,kcd2vr::door_collector_c54::allocate()};
 if(refusal)*refusal=s.serial?"NONE":"COLLECTOR_SERIAL_EXHAUSTED_OR_CONTENDED";return s;
}
static bool SelectionScopeCurrent68(selection68::Table& current,std::uint64_t now) noexcept {
    const auto& s=g_selection_scope68;std::uintptr_t policy=0;
    return g_selection_ready68.load(std::memory_order_acquire)&&g_enabled.load(std::memory_order_acquire)&&
        s.active&&g_selection_depth68==1&&craft68::selectionTable68(current,now)&&
        selection68::sameTable(s.table,current)&&SelectionPolicy68(s.interactor,current.player,policy)&&policy==s.policy;
}
static bool SelectionEntity68(std::uintptr_t entity,std::uint32_t& id) noexcept {
    id=0;std::uintptr_t cls=0,name=0;
    if(!SelectionVft68(entity,0x4072C78)||!SelectionRead68(entity+0xC,id)||!id||
       !SelectionRead68(entity+0x20,cls)||!SelectionVft68(cls,0x3B282A0)||
       !SelectionRead68(cls+0x10,name)||!name)return false;
    // R8b: the class name is no longer required to be "AlchemyItem". The mounted bellows (table slot 18, 952233 in
    // Miki's 24 Oct run) is a registered member of the SAME native alchemy table but another entity class, so its
    // candidate was never captured, its facing never forced and the hand filter left the native with NO selection
    // (G2 selected=0/21 while near=952233/18 and grip=1.000, 09:47:35-38). every caller still requires the id to be
    // registered in the current native alchemy table, so nothing outside the station can be captured.
    char first=0;return SelectionRead68(name,first)&&first;
}
static bool SelectionCandidateCurrent68(const selection68::Table& table) noexcept {
    const auto& c=g_selection_scope68.candidate;std::uint32_t id=0;selection68::Ref ref{};
    return c.valid&&SelectionEntity68(c.entity,id)&&id==c.ref.entity&&selection68::registered(table,id,&ref)&&
        kcd2vr::subsystems::alchemy::near67::same(ref,c.ref);
}
// R116: the station item the HEADSET view looks at. Same eye ray as the D75 names (final rendered
// camera origin + forward), against the boxes the native collector offered in the last 80 ms.
// Exact box hit first (nearest), else the box centre closest to the view centre within 6 deg.
// Not the native eye/camera: in alchemy that one is the authored station camera, 16-65 deg off.
static subsystems::alchemy::near67::Ref HeadPickR116(const selection68::Table& table,
    const std::array<craft68::NativeOffer,subsystems::alchemy::near67::Capacity>& offers,
    geometry68::Point eye,geometry68::Point forward,std::uint64_t now) noexcept {
    namespace n67=subsystems::alchemy::near67;n67::Ref result{};headR116::Score best{};
    if(!craft68::current(table,now))return result;
    for(unsigned i=0;i<n67::Capacity;++i){
        const auto& observed=offers[i];const auto& offer=observed.offer;
        if(observed.instance!=table.instance||observed.player!=table.player||
           observed.lifetime!=table.lifetime||!n67::valid(table.refs[i])||
           table.refs[i].slot!=int(i)||!n67::same(offer.ref,table.refs[i])||
           !offer.valid||!offer.boundsValid||!n67::fresh(offer.tick,now))continue;
        const auto s=headR116::score(offer.bounds,eye,forward);
        if(headR116::better(s,best)){best=s;result=offer.ref;}
    }
    return result;
}
// false = switch OFF or no current rendered eye ray (recenter, stale): callers keep the R115 path.
// true with target 0 = you look at no station item.
static bool HeadTargetR116(const selection68::Table& table,std::uint64_t now,std::uint32_t& target,float* eyeOut=nullptr) noexcept {
    target=0;visual_ray_d75::Sample ray{};
    if(!headR116::enabled.load(std::memory_order_acquire)||!CameraHook::GetLatestVisualRayD75(ray,80))return false;
    std::array<craft68::NativeOffer,subsystems::alchemy::near67::Capacity> offers{};
    for(unsigned i=0;i<offers.size();++i)(void)craft68::offers[i].read(offers[i]);
    const auto ref=HeadPickR116(table,offers,{ray.origin[0],ray.origin[1],ray.origin[2]},
        {ray.forward[0],ray.forward[1],ray.forward[2]},now);
    if(eyeOut)for(unsigned i=0;i<3;++i)eyeOut[i]=ray.origin[i];
    if(subsystems::alchemy::near67::valid(ref)&&selection68::registered(table,ref.entity))target=ref.entity;
    return true;
}
// R118: rendered eye ray for the native look ray while the 6DOF alchemy bridge owns a live station.
// false = not at the table / switch OFF / no current rendered ray -> the caller keeps the native origin.
static bool AlchemyEyeRayR118(float& ox,float& oy,float& oz,float& dx,float& dy,float& dz) noexcept {
    if(!headR116::enabled.load(std::memory_order_acquire)||!craft68::enabled.load(std::memory_order_acquire)||
       !g_selection_ready68.load(std::memory_order_acquire))return false;
    const auto& w=activity_input163::witness175;
    if(!w.ready.load(std::memory_order_acquire)||w.transitionDepth175.load(std::memory_order_acquire)||
       !w.active[unsigned(activity_input163::Kind175::Alchemy)].load(std::memory_order_acquire))return false;
    visual_ray_d75::Sample ray{};
    if(!CameraHook::GetLatestVisualRayD75(ray,80))return false;
    ox=ray.origin[0];oy=ray.origin[1];oz=ray.origin[2];dx=ray.forward[0];dy=ray.forward[1];dz=ray.forward[2];
    return true;
}
#include "interaction_door_diagnostics_c54r3.inl"
static std::uintptr_t __fastcall SelectorDetour68(std::uintptr_t interactor,std::uintptr_t prior,
    std::uintptr_t output,std::uintptr_t pose,unsigned key) {
    // Always mask an ancestor scope during nested/reentrant selectors. The
    // actual native call is original-once and all five inputs are unchanged.
    const auto saved=g_selection_scope68;const auto depth=g_selection_depth68;
    const auto savedDoor=kcd2vr::door_collector_c54::current;
    const char* doorDiagnosticScope3="NOT_CAPTURED";
    kcd2vr::door_collector_c54::current=DoorCollectorBeginC54(interactor,key,reinterpret_cast<std::uintptr_t>(_ReturnAddress()),depth,g_door_diag_ready3.load(std::memory_order_acquire)?&doorDiagnosticScope3:nullptr);
    g_selection_scope68={};g_selection_depth68=depth+1;
    if(!depth&&g_selection_ready68.load(std::memory_order_acquire)&&g_enabled.load(std::memory_order_acquire)&&key==1&&
       reinterpret_cast<std::uintptr_t>(_ReturnAddress())==g_wh_base+0x7F5727){
        auto& s=g_selection_scope68;std::uintptr_t entity=0;
        const auto now=std::uint64_t(GetTickCount64());
        if(craft68::selectionTable68(s.table,now)&&SelectionPolicy68(interactor,s.table.player,s.policy)&&
           SelectionRead68(s.table.player+0x38,entity)&&entity==s.table.playerEntity&&SelectionVft68(entity,0x4072C78)){
            s.interactor=interactor;s.active=true;
        }
    }
    const auto doorDiagnosticActivation3=DoorDiagBegin3(interactor,key,reinterpret_cast<std::uintptr_t>(_ReturnAddress()),depth,doorDiagnosticScope3);
    std::uintptr_t result=0;
    __try {result=g_selector_original68(interactor,prior,output,pose,key);DoorDiagEnd3(interactor,depth,doorDiagnosticActivation3);}
    __finally {g_selection_scope68=saved;g_selection_depth68=depth;kcd2vr::door_collector_c54::current=savedDoor;}
    return result;
}
static std::uintptr_t __fastcall BoundsDetour68(std::uintptr_t entity,std::uintptr_t out) {
    const bool candidateCall=reinterpret_cast<std::uintptr_t>(_ReturnAddress())==g_wh_base+0x63FF4A;
    // A known failed current capture cannot inherit the previous candidate.
    if(candidateCall)g_selection_scope68.candidate={};
    const auto result=g_bounds_original68(entity,out); // preserve raw native return bits, not a guessed sret
    if(candidateCall&&out)DoorDiagBounds3(entity,out);
    if(!candidateCall||!out)return result;
    const auto now=std::uint64_t(GetTickCount64());selection68::Table table{};std::uint32_t id=0;selection68::Ref ref{};
    if(!SelectionScopeCurrent68(table,now)||!SelectionEntity68(entity,id)||!selection68::registered(table,id,&ref))return result;
    selection68::Candidate c{};c.entity=entity;c.ref=ref;
    if(!SelectionCopy68(out,c.bounds.local,sizeof(c.bounds.local))||
       !SelectionCopy68(entity+0x58,c.bounds.world,sizeof(c.bounds.world))||!geometry68::valid(c.bounds))return result;
    selection68::Table again{};std::uint32_t againId=0;
    if(!SelectionScopeCurrent68(again,now)||!selection68::sameTable(table,again)||
       !SelectionEntity68(entity,againId)||againId!=id)return result;
    c.valid=true;g_selection_scope68.candidate=c;
    craft68::selectionBounds68(table,ref,c.bounds,now);
    {std::uint32_t nominee=0;if(craft68::selectionNear68(table,nominee,now)&&nominee==id)
        craft68::steerNote68(nominee,craft68::steerCandidateTick68,craft68::steerCandidates68,now);} // R8b steering receipt
    return result;
}
static char __fastcall FacingDetour68(std::uintptr_t interactor,std::uintptr_t point,std::uintptr_t quat) {
    const auto caller=reinterpret_cast<std::uintptr_t>(_ReturnAddress());
    const char result=g_facing_original68(interactor,point,quat); // facing predicate only, original once
    if(caller==g_wh_base+0x64032E)DoorDiagPredicate3(dd3::Facing,result,result);
    if(result||caller!=g_wh_base+0x64032E||interactor!=g_selection_scope68.interactor)return result;
    const auto now=std::uint64_t(GetTickCount64());selection68::Table table{};std::uint32_t nominated=0;
    if(!SelectionScopeCurrent68(table,now)||!SelectionCandidateCurrent68(table))return result;
    if(!craft68::selectionNear68(table,nominated,now)){
        // R116: no hand owns the station selection -> the item the headset looks at passes the
        // head-facing test (that test uses the native station camera, not your head).
        const auto candidate=g_selection_scope68.candidate.ref.entity;std::uint32_t head=0;
        return candidate&&HeadTargetR116(table,now,head)&&head==candidate?char(1):result;
    }
    if(nominated!=g_selection_scope68.candidate.ref.entity)return result;
    // Real near-surface source nomination or the actual held-mortar destination
    // aim replaces only the head-facing requirement. No remote source grab.
    // Held exclusion, OnScreen nomination, native policy60/eligibility, completed
    // selected entity and native contextual action are all still downstream.
    craft68::steerNote68(nominated,craft68::steerForcedTick68,craft68::steerForced68,now); // R8b steering receipt
    return 1;
}
// Called by the EXISTING CandidateFilterDetour after its original call. Never
// promotes a native exclusion false, or rejects an unrelated/non-table object.
static char FilterNativeCandidate68(std::uintptr_t interactor,std::uint32_t id,char originalResult,std::uint64_t now) noexcept {
    if(!originalResult||interactor!=g_selection_scope68.interactor)return originalResult;
    selection68::Table table{};std::uint32_t nominated=0;
    if(!SelectionScopeCurrent68(table,now)||!craft68::selectionNear68(table,nominated,now)||
       (nominated&&!selection68::registered(table,nominated))||!selection68::registered(table,id))return originalResult;
    // A current held hand with no near item owns an empty station selection.
    // Do not let an unrelated head target take over; non-station candidates
    // and every original native exclusion are untouched.
    if(!nominated)return 0;
    if(id!=nominated)craft68::steerNote68(nominated,craft68::steerFilteredTick68,craft68::steerFiltered68,now); // R8b
    return id==nominated?originalResult:0;
}

// The descriptor loop runs AFTER the collector TLS has ended. Its ray packet
// copies XYZ synchronously at530324; only this row's two input pointers change.
// The native endpoint, masks, skips, visibility callback and final result remain.
static std::atomic<bool> g_nominee_los_ready68{false};
static std::uintptr_t NomineeAlchemyInstance68() noexcept {
    const auto& w=activity_input163::witness175;
    return w.ready.load(std::memory_order_acquire)&&!w.transitionDepth175.load(std::memory_order_acquire)?
        w.active[unsigned(activity_input163::Kind175::Alchemy)].load(std::memory_order_acquire):0;
}
static bool NomineeActivityCurrent68(const selection68::Table& table) noexcept {
    return NomineeAlchemyInstance68()==table.instance&&
        activity_input163::witness175.lifetime175.load(std::memory_order_acquire)==table.lifetime;
}
static void NativeNomineeLos68(DetourModKit::hook::MidContext& ctx) noexcept {
    if(!g_nominee_los_ready68.load(std::memory_order_acquire)||
       !g_selection_ready68.load(std::memory_order_acquire)||!g_enabled.load(std::memory_order_acquire)||
       !craft68::enabled.load(std::memory_order_acquire)||!NomineeAlchemyInstance68())return;
    using namespace DetourModKit::hook;
    const auto interactor=gpr(ctx,Gpr::R15),row=gpr(ctx,Gpr::Rdi);
    unsigned key=0,id=0;float endpoint[3]{};
    if(!SelectionRead68(stack_pointer(ctx)+0x58,key)||key!=1||
       !SelectionRead68(row,id)||!SelectionCopy68(row+0x18,endpoint,sizeof(endpoint)))return;
    for(float v:endpoint)if(!std::isfinite(v))return;
    const auto now=std::uint64_t(GetTickCount64());selection68::Table table{},again{};
    craft68::Nomination nomination{};std::uintptr_t policy=0,entity=0;
    if(!craft68::selectionTable68(table,now)||!NomineeActivityCurrent68(table)||
       !SelectionPolicy68(interactor,table.player,policy)||
       !SelectionRead68(table.player+0x38,entity)||entity!=table.playerEntity||!SelectionVft68(entity,0x4072C78))return;
    // R116: a hand nominee keeps its palm-side ray; with no hand owner the headset-view target's
    // visibility ray starts at the rendered eye (what you see is what can be picked).
    const bool handR116=craft68::selectionOwner68(table,nomination,now)>=0;
    float headEyeR116[3]{};std::uint32_t headR116Target=0;
    if(handR116){if(nomination.decision.ref.entity!=id)return;}
    else if(!id||!HeadTargetR116(table,now,headR116Target,headEyeR116)||headR116Target!=id)return;
    if(!craft68::selectionTable68(again,now)||
       !selection68::sameTable(table,again)||!NomineeActivityCurrent68(again))return;
    // No engine call occurs before the original packet builder copies these
    // thread-local values. Async native visibility owns the copied packet, not
    // these pointers. A subsequent/nested query cannot mutate a prior packet.
    static thread_local struct {float origin[3],delta[3];} input{};
    if(handR116){
        const auto& palm=nomination.decision.palm.point;
        geometry68::Point origin{palm.x,palm.y,palm.z};
        // R116: R114 right-hand nominees 5-9 cm away (79392/7, 524263/19) were offered, every other
        // item filtered, and still the native result stayed 0: the ray began inside our own palm and
        // fingers. Start it 2 cm before the item's own box on the same palm->item line instead.
        craft68::NativeOffer o{};const auto& ref=nomination.decision.ref;
        if(headR116::enabled.load(std::memory_order_acquire)&&subsystems::alchemy::near67::valid(ref)&&
           craft68::offers[unsigned(ref.slot)].read(o)&&o.instance==table.instance&&o.player==table.player&&
           o.lifetime==table.lifetime&&subsystems::alchemy::near67::same(o.offer.ref,ref)&&o.offer.boundsValid&&
           subsystems::alchemy::near67::fresh(o.offer.tick,now))
            origin=headR116::losOrigin(o.offer.bounds,origin,{endpoint[0],endpoint[1],endpoint[2]});
        input.origin[0]=float(origin.x);input.origin[1]=float(origin.y);input.origin[2]=float(origin.z);
    }else for(unsigned i=0;i<3;++i)input.origin[i]=headEyeR116[i];
    for(unsigned i=0;i<3;++i){
        input.delta[i]=endpoint[i]-input.origin[i];
        if(!std::isfinite(input.origin[i])||!std::isfinite(input.delta[i]))return;
    }
    gpr(ctx,Gpr::Rdx)=reinterpret_cast<std::uintptr_t>(input.origin);
    gpr(ctx,Gpr::R8)=reinterpret_cast<std::uintptr_t>(input.delta);
}

struct NativeSelectionPin68 {std::uintptr_t rva;const char* hex;};
// Generated from the independently SHA-qualified retail file; verifier reads
// these exact strings rather than a second unconnected pin inventory.
static constexpr NativeSelectionPin68 kNativeSelectionPins68[]{
    {0x63F9ACull,"488bc4488958104c8940185556574154415541564157488da808feffff4881ec"},
    {0x7F570Bull,"8b7d404c8d4424304d8bce897c2420488d542460488bcee885a2e4ff"},
    {0x63F7A0ull,"48895c2408574883ec204c8b81a8000000488bda488bf949"},
    {0x63FF3Aull,"488b06488d5518488bceff9018010000"},
    {0xDE88A8ull,"48895c2410574883ec60488b4108488bda0f29742450488d5424200f297c2440498bf8488b4838488b01ff9070010000488d442420482bd833c9488d4424204803c1f30f100418f30f5c00f30f11440c304883c1044883f90c7cdff30f106f080f57fff30f1077040f28cdf30f1007f30f1027f30f105f0c488b442430488b5c2478f30f59c74889842480000000f30f59f7f30f59cff30f5ce6f30f59dff30f5cc8f30f5cf5f30f58e3f30f584f0cf30f58f30f28d4f30f5927f30f595704f30f59cdf30f59f5f30f5cd1f30f108c2484000000f30f5cf40f28c2f30f58c7f30f58c2f30f114424200f28c6f30f5805a8152b03f30f58c60f28742450f30f11442424488b4424204889442470f30f10442470f30f59842480000000f30f594c2474f30f58c80f2ff90f287c24400f92c04883c4605fc3"},
    {0x640329ull,"e87a857a0084c00f8488010000"},
    {0x9EE29Dull,"488d0decba07034c8978304c8978384c8978404889084c8978084c8978104c89781889582089582489582889582c4889452f4c8d4527488d4527c7452701000000488d55ef488945df488bce488975e7e89ae9ffff488d4ddfe8"},
    {0x63D50Eull,"488b4108488bfa4c8b01488bd94885c0740948c1e0044903c0eb03498bc0"},
    {0x1A74140ull,"488d4158c3"},
    {0x1A76130ull,"8b410cc3"},
    {0x19A2A90ull,"488b4120c3"},
    {0x638480ull,"488b4110c3"}
};
// Optional descriptor input seam; independent of the original three hooks.
// The five-byte flags store is the complete stolen span. The E8 and its return
// PC are outside it, preserving the existing ray hook's caller classification.
static constexpr NativeSelectionPin68 kNativeNomineeLosPins68[]{
    {0x7F590Bull,"44894c2458"},
    {0x7F5AC7ull,"f30f100431f30f5c01f30f1104114883c1044983e80175e8"},
    {0x7F5ADFull,"c6442438024c8d459888442430488d4dd0488d85e006000041b91f0100004889442428498bd44489742420e815a8d3ff"},
    {0x29FF942ull,"833900488bd97508"},
    {0x29FF957ull,"8b13488b8890010000"}
};
static constexpr NativeSelectionPin68 kNativeNomineePacketPins68[]{
    {0x530324ull,"f20f10024c8bd14c8b5c2430f20f11018b4208894108f2410f1000f20f11410c418b4008894114"},
    {0x53039Full,"498bc2c3"}
};
static constexpr std::uintptr_t kNativeSelectionPointers68[][2]{
    {0x4072C78ull+0x118,0x63F7A0},{0x4072C78ull+0x100,0x1A74140},
    {0x4072C78ull+8,0x1A76130},{0x3A69D90ull+0x58,0x9CBBA4},
    {0x3A69D90ull+0x60,0x8AC3C8}
};
template<std::size_t N>static bool SelectionPinsMatch68(std::uintptr_t base,const NativeSelectionPin68 (&pins)[N]) noexcept {
    if(!base||base!=g_wh_base||!retail_build112::Ready(base))return false;
    for(const auto& p:pins){unsigned char bytes[320]{};std::size_t n=0;
        const auto digit=[](char c) noexcept->int {return c>='0'&&c<='9'?c-'0':c>='a'&&c<='f'?c-'a'+10:-1;};
        for(const char* s=p.hex;*s;s+=2){const int a=digit(s[0]),b=s[1]?digit(s[1]):-1;
            if(a<0||b<0||n>=sizeof(bytes))return false;bytes[n++]=static_cast<unsigned char>((a<<4)|b);}
        if(!n||!MatchEntry(base+p.rva,bytes,n))return false;
    }
    return true;
}
// Setup-only byte qualification before the existing packet-builder hook rewrites
// its prologue. This stamp is for this exact module, not an interaction owner.
static std::uintptr_t g_nominee_packet_base68=0;
static void PrepareNativeNomineeLos68(std::uintptr_t base) noexcept {
    g_nominee_packet_base68=SelectionPinsMatch68(base,kNativeNomineePacketPins68)?base:0;
}
static bool NativeSelectionPins68(std::uintptr_t base) noexcept {
    if(!SelectionPinsMatch68(base,kNativeSelectionPins68))return false;
    for(const auto& p:kNativeSelectionPointers68){const auto wanted=base+p[1];
        if(!MatchEntry(base+p[0],reinterpret_cast<const unsigned char*>(&wanted),sizeof(wanted)))return false;}
    return true;
}

static std::optional<DetourModKit::hook::Hook> g_selector_hook68,g_bounds_hook68,g_facing_hook68,g_nominee_los_hook68;
static bool g_nominee_los_attempted68=false;
static std::atomic<bool> g_selection_retired68{false};
static bool NativeSelectionReady68() noexcept {
    return g_selection_ready68.load(std::memory_order_acquire)&&!g_selection_retired68.load(std::memory_order_acquire);
}
static void StopNativeSelection68() noexcept {
    g_nominee_los_ready68.store(false,std::memory_order_release);
    g_nominee_packet_base68=0;
    g_selection_ready68.store(false,std::memory_order_release);
    if(g_selector_hook68||g_bounds_hook68||g_facing_hook68)g_selection_retired68.store(true,std::memory_order_release);
    // Never free a trampoline or null its original during a possible callback.
    // The callback module is pinned before any enable. Failed disable is safe:
    // retained callbacks only run their original with ready=false.
    for(auto* h:{&g_nominee_los_hook68,&g_selector_hook68,&g_facing_hook68,&g_bounds_hook68})if(*h){
        try{(void)(*h)->disable();}catch(...){/* retain inactive authority + live trampoline */}
    }
}
static void InstallNativeNomineeLos68(std::uintptr_t base) noexcept {
    if(g_nominee_los_attempted68)return;
    g_nominee_los_attempted68=true;
    bool ready=false;
    if(NativeSelectionReady68()&&g_nominee_packet_base68==base&&
       SelectionPinsMatch68(base,kNativeNomineeLosPins68)){
        try{
            using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
            auto hook=mid_at(MidRequest{"kcd2vr_alchemy_nominee_los68",Target{DetourModKit::Address{base+0x7F5B05}},options},NativeNomineeLos68);
            if(hook){
                g_nominee_los_hook68=std::move(*hook);
                ready=bool(g_nominee_los_hook68->enable())&&g_nominee_los_hook68->is_enabled();
            }
        }catch(...){/* optional input correction; retain original selector */}
    }
    g_nominee_los_ready68.store(ready,std::memory_order_release);
    if(!ready&&g_nominee_los_hook68){try{(void)g_nominee_los_hook68->disable();}catch(...){/* retain inactive handle */}}
    diagnostic_trace::Write(ready?
        "G1R1 ALCHEMY_NEAR68 descriptorLOS=READY origin=CURRENT_NOMINEE_PALM nativeVisibility=UNCHANGED":
        "G1R1 ALCHEMY_NEAR68 descriptorLOS=UNAVAILABLE selector=RETAINED nativeLOS=UNCHANGED");
}
static bool InstallNativeSelection68(std::uintptr_t base) noexcept {
    if(NativeSelectionReady68()){InstallNativeNomineeLos68(base);return true;}
    if(g_selection_retired68.load(std::memory_order_acquire)||g_selector_hook68||g_bounds_hook68||g_facing_hook68||
       !g_split_ready57c1.load(std::memory_order_acquire)||!NativeSelectionPins68(base))return false;
    HMODULE module=nullptr;
    if(!GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_PIN,
        reinterpret_cast<LPCWSTR>(&SelectorDetour68),&module)||!module)return false;
    try {
        using namespace DetourModKit::hook;Options options{};options.fail_if_already_hooked=true;
        auto selector=inline_at(InlineRequest{"kcd2vr_alchemy_selector68",Target{DetourModKit::Address{base+0x63F9AC}},options},&SelectorDetour68);
        if(!selector)return false;
        g_selector_hook68=std::move(*selector);g_selector_original68=g_selector_hook68->original<Selector68Fn>();
        auto bounds=inline_at(InlineRequest{"kcd2vr_alchemy_bounds68",Target{DetourModKit::Address{base+0x63F7A0}},options},&BoundsDetour68);
        if(!bounds){StopNativeSelection68();return false;}
        g_bounds_hook68=std::move(*bounds);g_bounds_original68=g_bounds_hook68->original<Bounds68Fn>();
        auto facing=inline_at(InlineRequest{"kcd2vr_alchemy_facing68",Target{DetourModKit::Address{base+0xDE88A8}},options},&FacingDetour68);
        if(!facing){StopNativeSelection68();return false;}
        g_facing_hook68=std::move(*facing);g_facing_original68=g_facing_hook68->original<Facing68Fn>();
        // The selector scope is enabled last; no partial set gains authority.
        if(!g_selector_original68||!g_bounds_original68||!g_facing_original68||
           !g_bounds_hook68->enable()||!g_bounds_hook68->is_enabled()||
           !g_facing_hook68->enable()||!g_facing_hook68->is_enabled()||
           !g_selector_hook68->enable()||!g_selector_hook68->is_enabled()){
            StopNativeSelection68();return false;}
        g_selection_ready68.store(true,std::memory_order_release);InstallNativeNomineeLos68(base);return true;
    }catch(...){StopNativeSelection68();return false;}
}
