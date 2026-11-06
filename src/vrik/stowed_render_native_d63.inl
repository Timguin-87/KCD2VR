// D63: exact-retail CAttachmentBONE render leaf. Included in frik's private
// namespace AFTER coherent_pose.inl; include stowed_render_pose_d63.h at file
// scope. This is a render-parent substitution, not a pose/cache/native write.
// Proof: native_chain/BONE_RENDER_CALLER_PROOF.txt and BONE_RENDER_CHAIN_PINS.json.
// WHGame SHA256 bdf8f9e4a11257a72b64c84700e284c29e4c4ccaf5b8d4bfa7d0b2a7294479f7.

using StowedRenderFnD63=void(__fastcall*)(void* context,void* bone);
static StowedRenderFnD63 g_stowed_render_originalD63=nullptr;
static std::optional<DetourModKit::hook::Hook> g_stowed_render_hookD63;
static std::atomic<std::uintptr_t> g_stowed_render_baseD63{0};
static constexpr std::uintptr_t kStowedRenderLeafD63=0x20EB4B0;
static constexpr std::uintptr_t kStowedBoneVftD63=0x3A657C0;

// RCX is this four-pointer context, RDX is the live BONE attachment. The native
// manager holds its attachment lock throughout this call. Only parent changes
// in our PRIVATE copy; original flags/params/passInfo remain the same pointers.
struct alignas(16) StowedRenderContextD63 {
    void* hideMask=nullptr;
    void* params=nullptr;
    void* parent=nullptr;
    void* passInfo=nullptr;
};
static_assert(sizeof(StowedRenderContextD63)==32);
static_assert(offsetof(StowedRenderContextD63,parent)==0x10);

struct StowedRenderHeaderD63 {
    std::uintptr_t vft=0;
    void* manager=nullptr;
    void* character=nullptr;
    void* binding=nullptr;
    std::uint32_t flags=0;
    std::int16_t joint=-1;
    char name[40]{};
};

// This hook also sees NPC attachments. Reject them with one guarded scalar
// read before clocks, strings, VirtualQuery, RTTI or a committed-packet copy.
static bool StowedRenderManagerD63(const void* bone,void*& manager) noexcept {
    if(!bone)return false;
    __try {
        std::memcpy(&manager,static_cast<const unsigned char*>(bone)+0x28,sizeof(manager));
        return manager!=nullptr;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

// POD-only SEH: protect our bounded observations, never the original render.
static bool StowedRenderHeaderReadD63(const void* bone,StowedRenderHeaderD63& out) noexcept {
    if(!bone)return false;
    __try {
        const auto* b=static_cast<const unsigned char*>(bone);
        std::memcpy(&out.vft,b,sizeof(out.vft));
        std::memcpy(&out.manager,b+0x28,sizeof(out.manager));
        std::memcpy(&out.binding,b+0x20,sizeof(out.binding));
        std::memcpy(&out.flags,b+8,sizeof(out.flags));
        std::memcpy(&out.joint,b+0x78,sizeof(out.joint));
        if(!out.manager)return false;
        std::memcpy(&out.character,static_cast<const unsigned char*>(out.manager)+0x70,sizeof(out.character));
        const char* name=nullptr;std::memcpy(&name,b+0x10,sizeof(name));
        if(!name)return false;
        for(std::size_t i=0;i<sizeof(out.name);++i){
            const char c=name[i];out.name[i]=c;
            if(!c)return true;
        }
        return false; // reject a truncated name; never accept a prefix match
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

static int StowedRenderSlotD63(const char* name) noexcept {
    for(std::size_t i=0;i<std::size(kStowedSlotsD63);++i)
        if(!std::strcmp(name,kStowedSlotsD63[i].attachment))return int(i);
    return -1;
}

static bool StowedRenderInputsD63(const void* context,const void* bone,
    StowedRenderContextD63& copy,kcd2vr::stowed_render_pose_d63::Mat34& parent,
    QuatT28& nativeCache,QuatT28& relative) noexcept {
    if(!context||!bone)return false;
    __try {
        std::memcpy(&copy,context,sizeof(copy));
        if(!copy.parent||!copy.params||!copy.hideMask)return false;
        std::memcpy(parent.m.data(),copy.parent,sizeof(float)*12);
        const auto* b=static_cast<const unsigned char*>(bone);
        std::memcpy(&nativeCache,b+0x30,sizeof(nativeCache));
        std::memcpy(&relative,b+0x9C,sizeof(relative));
        // Native additional QuatT begins at +0x4C. Do NOT include it here;
        // the unchanged original leaf composes it exactly once afterwards.
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

static kcd2vr::stowed_render_pose_d63::Pose StowedRenderPoseD63(const QuatT28& p) noexcept {
    return {{p.q[0],p.q[1],p.q[2],p.q[3]},{p.t[0],p.t[1],p.t[2]}};
}

static bool StowedRenderAuthorityD63(std::uintptr_t base,void* manager,void* character,
    std::uint64_t epoch,std::uint64_t policy,std::uint64_t publication,std::uint64_t now) noexcept {
    return base&&g_stowed_render_baseD63.load(std::memory_order_acquire)==base&&
        kcd2vr::retail_build112::Ready(base)&&manager&&character&&
        reinterpret_cast<std::uintptr_t>(manager)==reinterpret_cast<std::uintptr_t>(character)+8&&
        manager==g_henry_char_attachment_manager.load(std::memory_order_acquire)&&
        character==g_committed_character6689.load(std::memory_order_acquire)&&
        epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
        policy&&!(policy&1)&&policy==g_pose_policy_generation57.load(std::memory_order_acquire)&&
        publication&&!(publication&1)&&publication==ReadCompletedPublicationT23()&&
        g_committed_pose_valid6689.load(std::memory_order_acquire)&&
        !g_pose_restore_fault6689.load(std::memory_order_acquire)&&
        g_cal_state.load(std::memory_order_acquire)==2&&TrackingHeadUsable111(now)&&
        !animation_filter::nativeBodyOwned112.load(std::memory_order_acquire)&&
        !diagnostics::lab::NativeReference();
    // No generic menu/gameplay deny: a qualified full workstation body is a
    // valid producer. Arms-only/passenger/reload packets have no valid slots.
}

// Visibility is a current character/slot preference, not a grant to pose bones.
// CCGF bindings bypass the entity attachment updater and can render between the
// 250ms holder-hide ticks. Enforce the same policy at their exact render leaf.
// No native flag/entity/cache is changed; OFF or detach needs no cleanup debt.
static bool StowedRenderHiddenR92(std::uintptr_t base,const StowedRenderHeaderD63& h,
                                  std::uint64_t now,bool extraBackR117=false) noexcept {
    if(!base||g_stowed_render_baseD63.load(std::memory_order_acquire)!=base||
       !kcd2vr::retail_build112::Ready(base)||!h.character||!h.manager||
       reinterpret_cast<std::uintptr_t>(h.manager)!=reinterpret_cast<std::uintptr_t>(h.character)+8||
       g_cal_state.load(std::memory_order_acquire)!=2||g_ctx_cutscene.load(std::memory_order_acquire)||
       diagnostics::lab::NativeReference())return false;
    const auto publication=g_player_publication37.load(std::memory_order_acquire);
    const auto lifetime=g_player_lifetime_d44.load(std::memory_order_acquire);
    if((publication&1u)||!lifetime||g_player_char37.load(std::memory_order_acquire)!=reinterpret_cast<std::uintptr_t>(h.character)||
       !PlayerIdentityAccepts37(h.character,now))return false;
    const auto in=StowedInputsD63();
    bool wanted=false,known=false;
    for(const auto& slot:kcd2vr::stowed_hide_hw::kSlots)if(!std::strcmp(slot.name,h.name)){
        known=true;wanted=kcd2vr::stowed_hide_hw::wantSlot(slot,in,StowedLegacyNameD63(slot.name));break;
    }
    // R117: an unnamed weapon socket on the spine is a back weapon (the polearm):
    // VrHideBackWeapons / station hiding / the attachment list apply to it too.
    if(!known&&extraBackR117)
        wanted=kcd2vr::stowed_hide_hw::wantSlot({h.name,"",kcd2vr::stowed_hide_hw::Place::Back},in,StowedLegacyNameD63(h.name));
    return wanted&&publication==g_player_publication37.load(std::memory_order_acquire)&&
        lifetime==g_player_lifetime_d44.load(std::memory_order_acquire)&&
        g_player_char37.load(std::memory_order_acquire)==reinterpret_cast<std::uintptr_t>(h.character)&&
        g_henry_char_attachment_manager.load(std::memory_order_acquire)==h.manager&&
        PlayerIdentityAccepts37(h.character,now);
}

// Diagnostic-only cache presence, not a claim that the native cached path was
// taken, refreshed, or presented. Its dirty bits remain entirely native.
static int StowedRenderCachePresentD63(const void* params) noexcept {
    if(!params)return -1;
    __try {
        void* cache=nullptr;std::memcpy(&cache,static_cast<const unsigned char*>(params)+0x30,sizeof(cache));
        return cache?1:0;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

static bool StowedRenderAdditionalReadD63(const void* bone,QuatT28& additional) noexcept {
    if(!bone)return false;
    __try {
        std::memcpy(&additional,static_cast<const unsigned char*>(bone)+0x4C,sizeof(additional));
        return true;
    } __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

static std::uintptr_t StowedRenderBindingVftD63(const void* binding) noexcept {
    if(!binding)return 0;
    __try { std::uintptr_t vft=0;std::memcpy(&vft,binding,sizeof(vft));return vft; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

static void StowedRenderReceiptD63(int slot,const StowedRenderHeaderD63& h,
    std::uint64_t now,std::uint64_t publication,std::uint64_t policy,const char* reason,
    const void* params,const void* bone,const QuatT28* nativeCache=nullptr,
    const kcd2vr::stowed_render_pose_d63::Pose* desiredCache=nullptr) noexcept {
    namespace d=kcd2vr::diagnostics::vrik_h29;
    if(!d::selected()||!d::active(now)||slot<0||slot>=int(std::size(kStowedSlotsD63)))return;
    static std::atomic<std::uint64_t> last[std::size(kStowedSlotsD63)]{};
    auto previous=last[slot].load(std::memory_order_relaxed);
    if((now>=previous&&now-previous<1000)||!last[slot].compare_exchange_strong(previous,now,std::memory_order_relaxed))return;
    // Additional transform and binding reads are diagnostic-only, after the
    // F7 lease and rate gate. Preserving native additional motion is intended;
    // this receipt can identify it without claiming every source of lean gone.
    QuatT28 additional{};const bool additionalKnown=StowedRenderAdditionalReadD63(bone,additional);
    const auto bindingVft=StowedRenderBindingVftD63(h.binding);
    const auto base=g_stowed_render_baseD63.load(std::memory_order_acquire);
    const auto bindingRva=base&&bindingVft>=base?bindingVft-base:0;
    const double nx=nativeCache?nativeCache->t[0]:0,ny=nativeCache?nativeCache->t[1]:0,nz=nativeCache?nativeCache->t[2]:0;
    const double vx=desiredCache?desiredCache->t[0]:0,vy=desiredCache?desiredCache->t[1]:0,vz=desiredCache?desiredCache->t[2]:0;
    const double delta=nativeCache&&desiredCache?std::sqrt((vx-nx)*(vx-nx)+(vy-ny)*(vy-ny)+(vz-nz)*(vz-nz)):-1.;
    double angle=-1.;
    if(nativeCache&&desiredCache){
        double dot=0,normNative=0,normDesired=0;
        for(int i=0;i<4;++i){const double n=nativeCache->q[i],v=desiredCache->q[i];dot+=n*v;normNative+=n*n;normDesired+=v*v;}
        const double denom=std::sqrt(normNative*normDesired);
        if(std::isfinite(denom)&&denom>1.e-12&&std::isfinite(dot))
            angle=2.*std::acos(std::clamp(std::abs(dot)/denom,0.,1.))*57.29577951308232;
    }
    char line[960]{};std::snprintf(line,sizeof(line),
        "D63 STOWED_RENDER slot=%s joint=%d publication=%llu policy=%llu reason=%s nativeKnown=%d vrKnown=%d nativeModel=(%.4f,%.4f,%.4f) vrModel=(%.4f,%.4f,%.4f) deltaM=%.5f deltaDeg=%.3f bindingVftRva=%llX cacheContextPresent=%d additionalKnown=%d additionalQ=(%.4f,%.4f,%.4f,%.4f) additionalT=(%.4f,%.4f,%.4f) rendererCache=UNCHANGED additional=NATIVE writes=PRIVATE_CONTEXT_ONLY presented=UNPROVEN rendererFreshness=UNVERIFIED",
        kStowedSlotsD63[slot].attachment,int(h.joint),(unsigned long long)publication,(unsigned long long)policy,reason,
        nativeCache?1:0,desiredCache?1:0,nx,ny,nz,vx,vy,vz,delta,angle,(unsigned long long)bindingRva,
        StowedRenderCachePresentD63(params),additionalKnown?1:0,
        double(additional.q[0]),double(additional.q[1]),double(additional.q[2]),double(additional.q[3]),
        double(additional.t[0]),double(additional.t[1]),double(additional.t[2]));
    diagnostic_trace::Write(line);
}

// R117: names of Henry's own weapon sockets outside the named table.
static bool StowedExtraWeaponNameR117(const char* name) noexcept {
    char low[sizeof(StowedRenderHeaderD63::name)]{};
    for(std::size_t i=0;i+1<sizeof(low)&&name[i];++i){const char c=name[i];low[i]=(c>='A'&&c<='Z')?char(c-'A'+'a'):c;}
    return std::strstr(low,"weapon")||std::strstr(low,"scabbard")||std::strstr(low,"polearm")||std::strstr(low,"spear");
}
// One KCD2VR.log line per unnamed socket and skeleton, so the item he sees can
// be named next time. Not F7 gated: at most 32 lines per load.
static void StowedExtraLogR117(int extra,const StowedRenderHeaderD63& h,bool torso,bool weapon,bool hidden) noexcept {
    auto& t=g_stowed_extraR117;const std::uint32_t bit=1u<<unsigned(extra);
    if(!(t.evaluatedMask.load(std::memory_order_acquire)&bit)||(t.loggedMask.load(std::memory_order_relaxed)&bit))return;
    if(!torso&&!weapon)return;
    if(t.loggedMask.fetch_or(bit,std::memory_order_acq_rel)&bit)return;
    char line[256]{};std::snprintf(line,sizeof(line),
        "R117 STOWED_EXTRA attachment=%s joint=%d torsoSocket=%d weaponName=%d hiddenAsBackWeapon=%d result=%s",
        h.name,int(h.joint),torso?1:0,weapon?1:0,hidden?1:0,
        hidden?"HIDDEN":torso?"FOLLOWS_SOLVED_TORSO":"NATIVE_NOT_ON_SPINE");
    diagnostic_trace::Write(line);
}

#include "../subsystems/vrmap/native_paper_render_m43.inl"

static void __fastcall StowedRenderDetourD63(void* context,void* bone) {
    const auto original=g_stowed_render_originalD63;
    if(!original)return; // hook is enabled only after its trampoline is set
    void* const manager=g_henry_char_attachment_manager.load(std::memory_order_acquire);
    void* observedManager=nullptr;
    if(!manager||!StowedRenderManagerD63(bone,observedManager)||observedManager!=manager){
        original(context,bone);return;
    }
    const auto base=g_stowed_render_baseD63.load(std::memory_order_acquire);
    StowedRenderHeaderD63 h{};
    if(!base||!StowedRenderHeaderReadD63(bone,h)||h.vft!=base+kStowedBoneVftD63||
       h.manager!=manager||!h.binding||h.joint<0){original(context,bone);return;}
    const int slot=StowedRenderSlotD63(h.name);
    // R117: any other Henry BONE socket is registered for the completed body
    // (by joint). Only spine-mounted ones are changed; the rest stay native.
    int extra=-1;bool extraTorso=false,extraWeapon=false;
    if(slot<0&&g_stowed_extraR117.character.load(std::memory_order_acquire)==reinterpret_cast<std::uintptr_t>(h.character)&&
       h.character==g_committed_character6689.load(std::memory_order_acquire)){
        extra=g_stowed_extraR117.Find(h.joint,true);
        if(extra>=0){
            extraTorso=(g_stowed_extraR117.torsoMask.load(std::memory_order_acquire)>>unsigned(extra))&1u;
            extraWeapon=StowedExtraWeaponNameR117(h.name);
        }
    }
    const auto now=(slot>=0||extra>=0)?std::uint64_t(GetTickCount64()):0;
    if(slot>=0&&StowedRenderHiddenR92(base,h,now)){
        StowedRenderReceiptD63(slot,h,now,0,0,"HIDDEN_BY_CURRENT_SLOT_POLICY",nullptr,bone);
        return; // same draw-only early return as the original leaf's native hide-mask branch
    }
    if(extra>=0){
        const bool hidden=extraTorso&&extraWeapon&&StowedRenderHiddenR92(base,h,now,true);
        StowedExtraLogR117(extra,h,extraTorso,extraWeapon,hidden);
        if(hidden)return; // same draw-only early return as a named hidden back slot
    }
    void* const character=g_committed_character6689.load(std::memory_order_acquire);
    if(!character||h.character!=character){original(context,bone);return;}
    if(NativePaperRenderM43(context,bone,h,base,manager,character,original))return;
    if(slot<0&&(extra<0||!extraTorso)){original(context,bone);return;}

    // Visible slots retain the original completed-pose parent correction.
    const auto publication=ReadCompletedPublicationT23();
    const auto epoch=g_pose_epoch6689.load(std::memory_order_acquire);
    const auto policy=g_pose_policy_generation57.load(std::memory_order_acquire);
    const char* refusal=nullptr;
    if(!StowedRenderAuthorityD63(base,manager,character,epoch,policy,publication,now))refusal="CURRENT_OWNER_UNAVAILABLE";
    CommittedFrame6695 packet{};
    if(!refusal&&!g_committed_frame6695.read(packet))refusal="PACKET_UNAVAILABLE";
    if(!refusal&&(packet.publicationR24!=publication||packet.stowedPolicyD63!=policy||
       !kcd2vr::pose_core::fresh(packet.stamp,epoch,reinterpret_cast<std::uintptr_t>(character),now)))refusal="STALE_OR_UNPAIRED";
    if(!refusal&&(!packet.doorTrackingEpoch66||
       packet.doorTrackingEpoch66!=tracking59c6::epoch.load(std::memory_order_acquire)))refusal="TRACKING_SPACE_CHANGED";
    const StowedPoseD63& stowedPose=slot>=0?packet.stowedD63[slot]:packet.stowedExtraR117[extra];
    if(!refusal&&(!packet.fistSkeletonH31||!packet.fistJointCountH31||
       unsigned(h.joint)>=packet.fistJointCountH31||!stowedPose.valid||
       stowedPose.joint!=h.joint))refusal="NO_MATCHED_FULL_BODY_SLOT";

    alignas(16) StowedRenderContextD63 privateContext{};
    alignas(16) kcd2vr::stowed_render_pose_d63::Mat34 parent{},correctedParent{};
    QuatT28 nativeCache{},relative{};
    kcd2vr::stowed_render_pose_d63::Pose desiredCache{};
    bool inputsKnown=false,desiredKnown=false;
    if(!refusal){
        inputsKnown=StowedRenderInputsD63(context,bone,privateContext,parent,nativeCache,relative);
        if(!inputsKnown)refusal="NATIVE_INPUT_UNAVAILABLE";
    }
    if(!refusal){
        desiredKnown=kcd2vr::stowed_render_pose_d63::compose(StowedRenderPoseD63(stowedPose.model),StowedRenderPoseD63(relative),desiredCache);
        if(!desiredKnown||!kcd2vr::stowed_render_pose_d63::privateParent(parent,StowedRenderPoseD63(nativeCache),
            desiredCache,{true,false},correctedParent))refusal="INVALID_TRANSFORM";
    }
    // Recheck all atomic publication/ownership evidence immediately before the
    // original call. No native body/cache/property is mutated, even on refusal.
    if(!refusal){
        const auto renderNow=std::uint64_t(GetTickCount64());
        if(!StowedRenderAuthorityD63(base,manager,character,epoch,policy,publication,renderNow)||
           !kcd2vr::pose_core::fresh(packet.stamp,epoch,reinterpret_cast<std::uintptr_t>(character),renderNow)||
           packet.doorTrackingEpoch66!=tracking59c6::epoch.load(std::memory_order_acquire))
            refusal="OWNER_CHANGED_BEFORE_RENDER";
    }
    if(refusal){
        original(context,bone);
        StowedRenderReceiptD63(slot,h,now,publication,policy,refusal,inputsKnown?privateContext.params:nullptr,bone,
            inputsKnown?&nativeCache:nullptr,desiredKnown?&desiredCache:nullptr);
        return;
    }
    privateContext.parent=correctedParent.m.data();
    original(&privateContext,bone); // exactly once; original owns final matrix and all rendering semantics
    StowedRenderReceiptD63(slot,h,now,publication,policy,"PRIVATE_PARENT_APPLIED",privateContext.params,bone,&nativeCache,&desiredCache);
}

static void ShutdownStowedRenderD63() noexcept {
    kcd2vr::native_paper_m43::renderHookReady.store(false);
    kcd2vr::native_paper_m43::RequestRetire();
    g_stowed_render_baseD63.store(0,std::memory_order_release);
    if(g_stowed_render_hookD63){(void)g_stowed_render_hookD63->disable();g_stowed_render_hookD63.reset();}
    g_stowed_render_originalD63=nullptr;
}

static bool InstallStowedRenderD63(std::uintptr_t base) noexcept {
    if(g_stowed_render_hookD63)return g_stowed_render_originalD63&&g_stowed_render_baseD63.load(std::memory_order_acquire)==base;
    // SupportedWhBuild alone checks only the timestamp. Verify additionally
    // pins SHA256 and retains the exact disk file lease; no hashing in render.
    if(!SupportedWhBuild(base)||!kcd2vr::retail_build112::Verify(base))return false;
    static constexpr unsigned char expected[]{
        0x48,0x81,0xEC,0xF8,0x00,0x00,0x00,0x48,0x83,0x7A,0x20,0x00,0x4C,0x8B,0xC9,0x0F,
        0x84,0x2C,0x05,0x00,0x00,0x49,0x8B,0x01,0x8B,0x4A,0x08,0x85,0x08,0x0F,0x85,0x1E};
    unsigned char actual[sizeof(expected)]{};
    if(!Read(reinterpret_cast<const void*>(base+kStowedRenderLeafD63),actual)||std::memcmp(actual,expected,sizeof(expected)))return false;
    DetourModKit::hook::InlineRequest request{"kcd2vr_d63_named_stowed_render_parent",
        DetourModKit::hook::Target{DetourModKit::Address{base+kStowedRenderLeafD63}},DetourModKit::hook::Options{}};
    auto hook=DetourModKit::hook::inline_at(std::move(request),&StowedRenderDetourD63);
    if(!hook)return false;
    g_stowed_render_hookD63=std::move(*hook);
    g_stowed_render_originalD63=g_stowed_render_hookD63->original<StowedRenderFnD63>();
    g_stowed_render_baseD63.store(base,std::memory_order_release);
    if(g_stowed_render_originalD63&&g_stowed_render_hookD63->enable()){kcd2vr::native_paper_m43::renderHookReady.store(true,std::memory_order_release);return true;}
    ShutdownStowedRenderD63();return false;
}
