// Independent raw-controller packet. Published on Henry's natural skeleton callback
// even when F10 is OFF or F12=2 deliberately suspends skeletal writes.
struct WeaponTracking6694 {
    void* character=nullptr;void* skeleton=nullptr;std::uint64_t epoch=0,tick=0,sample=0;
    QuatT28 nativeHand{},requestedHand{};float world[8]{};bool valid=false;
};
static std::atomic_flag g_weapon_tracking_lock6694=ATOMIC_FLAG_INIT;
static WeaponTracking6694 g_weapon_tracking6694; // legacy fixture/history only
static kcd2vr::core::SnapshotMailbox<WeaponTracking6694> g_weapon_frames6695;
static std::atomic<std::uint64_t> g_weapon_request_epoch6694{1};
static std::atomic<bool> g_weapon_fault6694{false};
static std::atomic<std::uint64_t> g_weapon_applied6694{0};
static std::atomic<int> g_weapon_reason6694{0};
static const char* WeaponReason6694(int r) noexcept {
    switch(r){case 1:return "WAIT_HELD_ITEM";case 2:return "WAIT_NATIVE_LINK_ID";case 3:return "WAIT_MATCHING_ATTACHMENT";
    case 4:return "WAIT_TRACKING_FRAME";case 5:return "NATIVE_REFERENCE_SUSPENDED";case 6:return "PLACEMENT_FAULT_LATCHED";
    case 7:return "CONTROLLER_PLACEMENT_ACTIVE";case 8:return "NATIVE_PLACEMENT_RESTORED";case 9:return "WAIT_COMMITTED_HAND_NO_RAW_SWITCH";default:return "OFF";}
}
// V66.97: native hand membership, not merely an item/entity still being alive.
// Retail +9D34E8 reads manager+38 -> player+990 -> interface+258.
// Retail +8D0758 writes interface[8+side*8]; +8D06F0 side=2 clears
// the matching hand via that writer. No gameplay getter is called here.
static bool NativeHandKey6697(void* manager,int side,std::uint64_t& key) noexcept {
    key=0;if(!manager||side<0||side>1)return false;
    const auto base=WhBase();void* player=nullptr;void* slots=nullptr;void* back=nullptr;
    std::uintptr_t vf=0;
    if(!base||!Read(manager,vf)||vf!=base+0x3EC9958ull||
       !Read(static_cast<unsigned char*>(manager)+0x38,player)||!player||
       !Read(player,vf)||vf!=base+0x3AA4B98ull||
       !Read(static_cast<unsigned char*>(player)+0x9D0,back)||back!=manager||
       !Read(static_cast<unsigned char*>(player)+0x990,slots)||!slots||
       !Read(static_cast<unsigned char*>(slots)+0x260+side*8,key))return false;
    // Verify the small chain after the read; changing equipment ownership is
    // not permission to keep a previously cached hand transform.
    void* again=nullptr;std::uint64_t keyAgain=0;
    return Read(static_cast<unsigned char*>(manager)+0x38,again)&&again==player&&
        Read(static_cast<unsigned char*>(player)+0x990,again)&&again==slots&&
        Read(static_cast<unsigned char*>(slots)+0x260+side*8,keyAgain)&&keyAgain==key;
}
// Cheap exclusion filter only. A match must subsequently pass the complete
// NativeHeldItemId6694 owner/role/coherence validation; this grants no writes.
static bool NativeCandidateItemId6699(int side,std::uint32_t& id) noexcept {
    id=0;if(side<0||side>1)return false;
    void* item=g_native_side_item[side].load(std::memory_order_acquire);
    void* link=nullptr;
    if(!item||g_native_side_item_state[side].load()!=1||
       !Read(static_cast<unsigned char*>(item)+0x88,link)||!link||
       !Read(static_cast<unsigned char*>(link)+0x30,id)||!id)return false;
    return item==g_native_side_item[side].load(std::memory_order_acquire);
}
static bool NativeHeldItemId6694(int side,std::uint32_t& id) noexcept {
    id=0;if(side<0||side>1)return false;
    void* manager=g_henry_item_attach_manager.load(std::memory_order_acquire);
    void* item=g_native_side_item[side].load(std::memory_order_acquire);
    if(!manager||!item||g_native_side_owner[side].load()!=manager||g_native_side_item_state[side].load()!=1)return false;
    std::uintptr_t vf=0;std::uint64_t key=0;void* link=nullptr;
    if(!Read(item,vf)||vf!=WhBase()+0x3A2DA88ull||!Read(static_cast<unsigned char*>(item)+0x30,key)||
       !key||key!=g_native_side_wuid0[side].load())return false;
    // +82D014 native getter: this is an ID record, NOT a CEntity pointer.
    if(!Read(static_cast<unsigned char*>(item)+0x88,link)||!link||!Read(static_cast<unsigned char*>(link)+0x30,id)||!id)return false;
    std::uint64_t handKey=0;
    if(!NativeHandKey6697(manager,side,handKey)||handKey!=key){id=0;return false;}
    return manager==g_henry_item_attach_manager.load(std::memory_order_acquire)&&
        item==g_native_side_item[side].load(std::memory_order_acquire)&&key==g_native_side_wuid0[side].load();
}
static void* CallEntityLookup6694(std::uintptr_t fn,void* sys,std::uint32_t id) noexcept {return MockNativeEntityLookup6694(fn,sys,id);}
static void* EntityById6694(std::uint32_t id) noexcept {
    if(!id||g_weapon_fault6694.load())return nullptr;void* sys=nullptr;std::uintptr_t vf=0,fn=0;
    if(!Read(reinterpret_cast<void*>(WhBase()+0x492D8A0ull),sys)||!sys||!Read(sys,vf)||vf!=WhBase()+0x4077048ull||
       !Read(reinterpret_cast<void*>(vf+0x70),fn)||fn!=WhBase()+0x4B4F98ull)return nullptr;
    void* ent=CallEntityLookup6694(fn,sys,id);
    std::uint32_t actual=0;std::uintptr_t ev=0;
    return ent&&Read(ent,ev)&&ev==WhBase()+kCEntityVftRva&&Read(static_cast<unsigned char*>(ent)+0x0C,actual)&&actual==id?ent:nullptr;
}
static Q4 AnatomicalGrip6694(Q4 frame,Q4 xr,Q4 boneToAnatomy) noexcept {
    const Q4 c=QMul(frame,XrToCryQ(xr));
    const V3 distal=QRotateV(c,XrToCryV({0,-1,0})),side=QRotateV(c,XrToCryV({0,0,-1}));
    return QNormalize(QMul(QFromBasis(distal,side,Normalize(Cross(distal,side))),QConj(boneToAnatomy)));
}
static void PublishWeaponTracking6694(void* character,void* skeleton,void* pose,PoseGetFn getter,
                                    const RigMap& rig,const TrackingSnapshot& ts) noexcept {
    if(!character||!skeleton||!ts.allowed||!ts.head_valid||!ts.right_valid||!ts.tick)return;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());if(now<ts.tick||now-ts.tick>100)return;
    WeaponTracking6694 packet{};QuatT28 head{},hand{};
    kcd2vr::native_roomscale_d76::Origin roomOriginD76{};
    if(!kcd2vr::native_roomscale_d76::ReadCharacterOrigin(reinterpret_cast<std::uintptr_t>(character),roomOriginD76))return;
    if(!ReadQuat(getter,pose,rig.head,head)||!ReadQuat(getter,pose,rig.hand_r,hand)||!Read(static_cast<unsigned char*>(character)+0xA08,packet.world))return;
    double norm=0;for(int i=0;i<8;++i)if(!std::isfinite(packet.world[i]))return;
    for(int i=0;i<4;++i)norm+=double(packet.world[i])*packet.world[i];
    if(norm<.8||norm>1.2||packet.world[7]<.1f||packet.world[7]>10)return;
    // Only a handful of immutable anatomy reads; this does NOT run an arm solve.
    static void* cachedSkeleton=nullptr;static Q4 boneToAnatomy{};
    if(cachedSkeleton!=skeleton){
        QuatT28 h{},p[4]{};const int fingers[4]{415,414,413,412};
        if(!ReadCompanion(skeleton,rig.hand_r,h))return;
        V3 center{};for(int i=0;i<4;++i){if(!ReadCompanion(skeleton,fingers[i],p[i]))return;center=Add(center,FromT(p[i]));}
        center=Mul(center,.25f);const V3 d=Normalize(Sub(center,FromT(h))),side=Normalize(Sub(FromT(p[0]),FromT(p[3])));
        const V3 n=Normalize(Cross(d,side)),l=Normalize(Cross(n,d));if(Len(n)<.5f)return;
        boneToAnatomy=QMul(QConj(QFromQuatT(h)),QFromBasis(d,l,n));cachedSkeleton=skeleton;
    }
    Q4 frame{};V3 baseHead{},trackingHead{};
    if(g_cal_state.load(std::memory_order_acquire)==2){V3 unused{};(void)ControllerHeadAnchoredArmGoal(ts,head,false,unused);
        frame=QNormalize({g_cal_frame_qx.load(),g_cal_frame_qy.load(),g_cal_frame_qz.load(),g_cal_frame_qw.load()});
        baseHead={g_cal_native_head_x.load(),g_cal_native_head_y.load(),g_cal_native_head_z.load()};
        trackingHead={g_cal_head_x.load(),g_cal_head_y.load(),g_cal_head_z.load()};
    }else{
        // Weapon-only mode does not demand a T-pose. Establish a fixed head-level
        // origin/yaw once per actor/request. Current HMD pitch/roll is never reused.
        static void* originChar=nullptr;static std::uint64_t originEpoch=0;
        static V3 nh{},th{};static Q4 fq{};const auto e=g_weapon_request_epoch6694.load();
        if(originChar!=character||originEpoch!=e){
            const V3 forward=QRotateV(XrToCryQ(ts.hq),{0,1,0});
            const float angle=std::atan2(-forward.x,forward.y);fq={0,0,std::sin(-angle*.5f),std::cos(-angle*.5f)};
            nh=FromT(head);th=Sub(ts.head,{roomOriginD76.consumedTracking.x,roomOriginD76.consumedTracking.y,roomOriginD76.consumedTracking.z});originChar=character;originEpoch=e;
        }frame=fq;baseHead=nh;trackingHead=th;
    }
    if(kcd2vr::roomscale_d76::recenterPending.load(std::memory_order_acquire)||
       (g_cal_state.load()==2&&roomOriginD76.owner.epoch&&roomOriginD76.owner.epoch!=g_cal_room_origin_d76.load(std::memory_order_acquire)))return;
    trackingHead=Add(trackingHead,{roomOriginD76.consumedTracking.x,roomOriginD76.consumedTracking.y,roomOriginD76.consumedTracking.z});
    packet.nativeHand=hand;packet.requestedHand=hand;
    ToT(packet.requestedHand,Add(baseHead,QRotateV(frame,XrToCryV(Sub(ts.right,trackingHead)))));
    QToQuatT(packet.requestedHand,AnatomicalGrip6694(frame,ts.rq,boneToAnatomy));
    packet.character=character;packet.skeleton=skeleton;packet.tick=ts.tick;packet.sample=ts.sequence;
    packet.epoch=g_pose_epoch6689.load();packet.valid=true;
    if(!kcd2vr::native_roomscale_d76::SameSerial(kcd2vr::native_roomscale_d76::Surface::Character,
        reinterpret_cast<std::uintptr_t>(character),roomOriginD76.serial))return;
    (void)g_weapon_frames6695.publish(packet);
}
static bool ReadWeaponTracking6694(WeaponTracking6694& out) noexcept {
    if(!g_weapon_frames6695.read(out)||!g_gameplay_allowed.load()||!g_head_valid.load()||!g_right_valid.load())return false;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    return out.valid&&out.sample&&out.tick&&now>=out.tick&&now-out.tick<=100&&out.epoch==g_pose_epoch6689.load();
}
static QuatT28 HandWorld6694(const QuatT28& model,const float* world) noexcept {
    const Q4 q=QNormalize({world[0],world[1],world[2],world[3]});QuatT28 result=model;
    QToQuatT(result,QMul(q,QFromQuatT(model)));
    ToT(result,Add({world[4],world[5],world[6]},QRotateV(q,Mul(FromT(model),world[7]))));return result;
}

struct AttachmentState6689 {
    void* self=nullptr;void* attachment=nullptr;void* entity=nullptr;std::uint32_t id=0;
    std::uint64_t generation=0;bool changed=false;std::uint64_t requestEpoch=0,lastLog=0;
    bool gripValid=false,usedCommitted=false;QuatT28 grip{};
    kcd2vr::pose_core::Stamp lastSource{};float lastWanted[8]{};int lastMode=0;
};
static AttachmentState6689 g_attachment6689;
static std::atomic_flag g_attachment_lock6689=ATOMIC_FLAG_INIT;
static std::atomic<void*> g_attachment_pending_self6695{nullptr},g_attachment_pending_object6695{nullptr};
struct AttachmentLock6689 { bool owned=!g_attachment_lock6689.test_and_set(std::memory_order_acquire);~AttachmentLock6689(){if(owned)g_attachment_lock6689.clear(std::memory_order_release);} };
static void MarkWeaponPending6695(void* self,void* attachment) noexcept {
    g_attachment_pending_object6695.store(attachment,std::memory_order_relaxed);
    g_attachment_pending_self6695.store(self,std::memory_order_release);
    diagnostics::lab::weaponRestorePending.store(self!=nullptr,std::memory_order_release);
}
static bool ReadAttachmentQs6689(void* attachment,float (&qs)[8]) noexcept {
    void* vft=nullptr;AttachmentWorldQuatTsFn fn=nullptr;
    if(!Read(attachment,vft)||!vft||!Read(static_cast<unsigned char*>(vft)+0x70,fn)||!fn)return false;
    __try {fn(attachment,qs);}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
    double qn=0;for(int i=0;i<8;++i)if(!std::isfinite(qs[i]))return false;
    for(int i=0;i<4;++i)qn+=double(qs[i])*qs[i];
    return qn>.8&&qn<1.2&&qs[7]>.001f&&qs[7]<100;
}
static void* CharacterForEntity6689(void* ent) noexcept {
    void* vf=nullptr;void* fn=nullptr;if(!Read(ent,vf)||!vf||!Read(static_cast<unsigned char*>(vf)+0x328,fn)||!fn)return nullptr;
    __try{return reinterpret_cast<void*(__fastcall*)(void*,int)>(fn)(ent,0);}__except(EXCEPTION_EXECUTE_HANDLER){return nullptr;}
}
// V66.128: capture the held bow's character. The entity vtable slot must be the exact
// retail CEntity::GetCharacter and the result must be a validated CCharInstance.
static void BowCaptureCharacter128(void*) noexcept {} // host native leaf
static bool PlaceBothConsumers6689(void* self,void* ent,const float (&qs)[8],bool& hasCharacter) noexcept {
    void* vf=nullptr;EntitySetPosRotScaleFn prs=nullptr;float scale[3]{};
    if(!Read(ent,vf)||!vf||!Read(static_cast<unsigned char*>(vf)+0x168,prs)||!prs)return false;
    for(int i=0;i<3;++i){if(!Read(static_cast<unsigned char*>(self)+0x0C+i*4,scale[i])||!std::isfinite(scale[i]))return false;scale[i]*=qs[7];}
    // Exceptions are isolated in this leaf; no RAII object crosses an MSVC SEH block.
    __try {prs(ent,qs+4,qs,scale,0x10u);}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
    void* child=CharacterForEntity6689(ent);hasCharacter=child!=nullptr;
    if(child){
        void* cv=nullptr;void* update=nullptr;
        if(!Read(child,cv)||!cv||!Read(static_cast<unsigned char*>(cv)+0x190,update)||update!=reinterpret_cast<void*>(WhBase()+0x423C30ull))return false;
        __try{reinterpret_cast<void(__fastcall*)(void*,const float*)>(update)(child,qs);}__except(EXCEPTION_EXECUTE_HANDLER){return false;}
    }
    return true;
}
static bool PlacementReadback6689(void* self,void* ent,std::uint32_t id,const float (&qs)[8],bool& characterChecked) noexcept {
    Matrix34V6550 tm{};if(!GetEntityWorldTmV6552(ent,id,tm))return false;
    const V3 delta{tm.m[3]-qs[4],tm.m[7]-qs[5],tm.m[11]-qs[6]};if(Len(delta)>.008f)return false;
    // Verify the native per-axis scale as well as position and quaternion.
    for(int c=0;c<3;++c){float scale=0;if(!Read(static_cast<unsigned char*>(self)+0x0C+c*4,scale))return false;
        const float expected=std::fabs(scale*qs[7]);const float actual=Len(V3{tm.m[c],tm.m[4+c],tm.m[8+c]});
        if(!std::isfinite(actual)||expected<.0001f||std::fabs(actual-expected)>.001f*std::max(1.f,expected))return false;}
    // Check rotation independently of scale; never call position readback full-pose proof.
    const V3 x=Normalize({tm.m[0],tm.m[4],tm.m[8]}),y=Normalize({tm.m[1],tm.m[5],tm.m[9]}),z=Normalize({tm.m[2],tm.m[6],tm.m[10]});
    const Q4 q=QFromBasis(x,y,z);const double dot=std::fabs(q.x*qs[0]+q.y*qs[1]+q.z*qs[2]+q.w*qs[3]);if(dot<.999)return false;
    void* child=CharacterForEntity6689(ent);characterChecked=child!=nullptr;
    if(child){float actual[8]{};if(!Read(static_cast<unsigned char*>(child)+0xA08,actual))return false;
        for(int i=0;i<8;++i)if(!std::isfinite(actual[i])||std::fabs(actual[i]-qs[i])>.001f)return false;}
    return true;
}
static bool ControllerWeaponTransform6694(AttachmentState6689& state,int mode,const float (&original)[8],float (&out)[8]) noexcept {
    QuatT28 nativeWorld{},requested{};kcd2vr::pose_core::Stamp source{};
    state.usedCommitted=mode==3;
    if(mode==3){
        QuatT28 native{},allowed{};float world[8]{};
        if(!ReadCommittedHandFrame6689(native,allowed,world,&source))return false;
        nativeWorld=HandWorld6694(native,world);requested=HandWorld6694(allowed,world);
    }else{
        WeaponTracking6694 raw{};if(!ReadWeaponTracking6694(raw))return false;
        nativeWorld=HandWorld6694(raw.nativeHand,raw.world);requested=HandWorld6694(raw.requestedHand,raw.world);
        source={raw.epoch,raw.sample,raw.tick,reinterpret_cast<std::uintptr_t>(raw.character)};
    }
    if(!state.gripValid){QuatT28 weapon{};std::copy_n(original,4,weapon.q);std::copy_n(original+4,3,weapon.t);
        state.grip=RelativeFromAbsolute(nativeWorld,weapon);state.gripValid=true;}
    const QuatT28 wanted=AbsoluteFromRelative(requested,state.grip);
    std::copy_n(original,8,out);std::copy_n(wanted.q,4,out);std::copy_n(wanted.t,3,out+4);
    for(float v:out)if(!std::isfinite(v))return false;
    state.lastSource=source;state.lastMode=mode;std::copy_n(out,8,state.lastWanted);return true;
}
// V66.132: atomic 3 -> physical takeover. While the new physical owner is
// WAITING for its coherent frame/grip, the already-qualified controller writer
// may remain the sole writer for the SAME right-hand attachment. The instant
// PhysicalAttachmentUpdate101 returns Placed, this bridge is no longer called.
static bool BridgeControllerToPhysical132(void* self,void* attachment,int mode) noexcept {
    AttachmentLock6689 guard;if(!guard.owned)return false;auto& state=g_attachment6689;
    const bool same=state.self==self&&state.attachment==attachment;
    if(!kcd2vr::physical_handoff132::allow_bridge(mode,same,state.gripValid,g_gameplay_allowed.load(),
        TrackingHeadUsable111(GetTickCount64()),TrackingHandUsable111(0,GetTickCount64()),double(g_right_grip.load())))return false;
    std::uint32_t bound=0,id=0;if(!Read(static_cast<unsigned char*>(self)+8,bound)||!bound||
       !NativeHeldItemId6694(0,id)||id!=bound)return false;
    void* entity=EntityById6694(id);if(!entity)return false;
    float original[8]{},wanted[8]{};if(!ReadAttachmentQs6689(attachment,original)||!ControllerWeaponTransform6694(state,3,original,wanted))return false;
    const auto now=static_cast<std::uint64_t>(GetTickCount64());const auto request=g_weapon_request_epoch6694.load();
    if(mode!=g_weapon_control_mode.load()||!g_gameplay_allowed.load()||!TrackingHeadUsable111(now)||!TrackingHandUsable111(0,now)||
       (FullWeaponMode120(mode)&&g_right_grip.load()<.25f))return false;
    std::uint32_t live=0;if(!NativeHeldItemId6694(0,live)||live!=id)return false;
    bool child=false;if(!PlaceBothConsumers6689(self,entity,wanted,child))return false;
    const bool newRequest=state.requestEpoch!=request;state.changed=true;state.requestEpoch=request;MarkWeaponPending6695(self,attachment);
    if(newRequest){char m[256]{};std::snprintf(m,sizeof(m),"V66.132 PHYSICAL_HANDOFF BRIDGE key=%016llX entity=%u mode=%d soleWriter=CONTROLLER until=PHYSICAL_PLACED grip=%.3f",
        (unsigned long long)g_native_side_wuid0[0].load(),id,mode,double(g_right_grip.load()));diagnostic_trace::Write(m);}
    return true;
}

namespace {static PhysicalAttachmentResult101 NativeHeldAttachmentT4(void*,void*) noexcept;}
namespace {static void StationWeaponAfterAttachmentD56(void*,void*) noexcept;}
static void __fastcall EntityAttachmentUpdateDetourV6688(void* self,void* attachment) noexcept {kcd2vr::hookcost178r22::Scope hc22{kcd2vr::hookcost178r22::AttachmentUpdate};
    struct StationVisibilityD56 {void* self;void* attachment;
        ~StationVisibilityD56(){StationWeaponAfterAttachmentD56(self,attachment);}} stationVisibilityD56{self,attachment};
    const AttachmentObservation15 observation{self,attachment};
    const auto native=[&](){InvalidateReadCache101R2();if(g_entity_attachment_update_original){kcd2vr::hookcost178r22::Original og{kcd2vr::hookcost178r22::AttachmentUpdate};g_entity_attachment_update_original(self,attachment);}InvalidateReadCache101R2();};
    if(!self||!attachment){native();return;}
    // A retired/unknown solver body can still collide. No native/controller,
    // activity, grace or tool writer resumes before exact collider retirement.
    WeaponDrawBeforeAttachmentD75(self,attachment);
    const auto heldT4=NativeHeldAttachmentT4(self,attachment);
    if(heldT4==PhysicalAttachmentResult101::Placed||heldT4==PhysicalAttachmentResult101::Busy)return;
    if(CrossbowAttachmentXR34(self,attachment))return;
    // C46R1: semi-passenger tools use their real native attachment transform.
#include "vrik/native_activity_passthrough46.inl"
    // Exact native-held tools have a separate F5 owner; weapons still take their
    // existing physical path. OFF/reference is a cheap no-op in this function.
    if(ManualArrowAttachment64(self,attachment))return;
    if(ToolAttachment59c10(self,attachment))return;
    const int mode=g_weapon_control_mode.load(std::memory_order_acquire);
    const bool pendingCandidate=g_attachment_pending_self6695.load(std::memory_order_acquire)==self &&
        g_attachment_pending_object6695.load(std::memory_order_relaxed)==attachment;
    if(mode==0&&!pendingCandidate&&!PhysicalAttachmentPending6698()){native();return;}
    ReadScope101R2 pages;
    // Placement is an explicit result, not the old ambiguous "handled" bool.
    if(mode>=4||PhysicalAttachmentPending6698()){
        const auto result=PhysicalAttachmentUpdate101(self,attachment);
        if(result==PhysicalAttachmentResult101::Placed){
            AttachmentLock6689 old;if(old.owned&&g_attachment6689.self==self&&g_attachment6689.attachment==attachment){
                // Keep the qualified backup grip, but no second active writer.
                g_attachment6689.changed=false;MarkWeaponPending6695(nullptr,nullptr);
            }
            return;
        }
        if(result==PhysicalAttachmentResult101::Busy)return;
        if(result==PhysicalAttachmentResult101::Restored||result==PhysicalAttachmentResult101::Fault){
            AttachmentLock6689 old;if(old.owned&&g_attachment6689.self==self&&g_attachment6689.attachment==attachment){g_attachment6689={};MarkWeaponPending6695(nullptr,nullptr);}return;
        }
        if(mode>=4){
            // V66.157: keep ONE authoritative full-physical writer.  The retired
            // controller bridge is deliberately NOT revived here because it does
            // not carry the physical contact/reach result.  Count this native
            // WAITING fallback so headset logs tell us exactly how often the
            // physical producer failed to publish before we change ownership.
            if(result==PhysicalAttachmentResult101::Waiting)g_phys_waiting_native157.fetch_add(1,std::memory_order_relaxed);
            native();
            AttachmentLock6689 old;if(old.owned&&g_attachment6689.self==self&&g_attachment6689.attachment==attachment){
                g_attachment6689.changed=false;MarkWeaponPending6695(nullptr,nullptr);
            }
            return;
        }
        // A lower rung may resume its own writer after physical pending work.
    }
    const int controllerMode=mode>=3?3:mode;
    // Filter unrelated traffic BEFORE entering the exact-target transaction.
    // Only the same currently-owned/pending attachment may skip duplicate
    // placement when another callback already owns that transaction.
    std::uint32_t bound=0,id=0;
    // Candidate lookup is NOT authorization. Unrelated world attachments must
    // not each walk Henry's manager/player/hand chain (each Read uses VirtualQuery).
    // Exact current hand membership is still required here AND before placement.
    std::uint32_t candidateId=0;
    const bool match=Read(static_cast<unsigned char*>(self)+8,bound)&&bound&&
        NativeCandidateItemId6699(0,candidateId)&&bound==candidateId&&
        NativeHeldItemId6694(0,id)&&id&&bound==id;
    if(!match&&!pendingCandidate){native();return;}
    AttachmentLock6689 guard;
    if(!guard.owned){g_weapon_contended6695.fetch_add(1,std::memory_order_relaxed);return;}
    auto& state=g_attachment6689;
    const auto restore=[&](){
        native();if(state.changed)g_weapon_restored6695.fetch_add(1,std::memory_order_relaxed);
        state.changed=false;state.lastSource={};MarkWeaponPending6695(nullptr,nullptr);
    };
    if(!match){
        const bool relinquished=state.changed;restore();state={};
        g_weapon_control_entity_id[0].store(0);g_weapon_control_entity[0].store(nullptr);
        g_weapon_reason6694.store(2);
        if(relinquished)diagnostic_trace::Write("V66.97-F12 HAND_ROLE_REVOKED gripCleared=1 destination=CURRENT_NATIVE noHistoricalHandRestore=1");
        return;
    }
    if(mode<2||diagnostics::lab::NativeReference()||NativePassengerCurrentA34(GetTickCount64())||g_weapon_fault6694.load(std::memory_order_acquire)){
        const bool changed=state.changed;restore();state.gripValid=false;
        g_weapon_reason6694.store(changed?8:diagnostics::lab::NativeReference()?5:g_weapon_fault6694.load()?6:3);
        if(mode==1){const auto now=static_cast<std::uint64_t>(GetTickCount64());if(now-state.lastLog>=2000){state.lastLog=now;char m[384]{};
            std::snprintf(m,sizeof(m),"V66.95-F12 BOUND item=%p id=%u attachment=%p key=%016llX route=C_ITEM_LINKED_ID_NATIVE_ENTITY_SYSTEM writes=NONE",g_native_side_item[0].load(),id,attachment,(unsigned long long)g_native_side_wuid0[0].load());diagnostic_trace::Write(m);}}
        return;
    }
    void* entity=EntityById6694(id);if(!entity){restore();state={};g_weapon_reason6694.store(3);return;}
    g_weapon_control_entity_id[0].store(id,std::memory_order_release);g_weapon_control_entity[0].store(entity,std::memory_order_release);
    const auto generation=g_weapon_control_generation[0].load(std::memory_order_acquire);
    const auto request=g_weapon_request_epoch6694.load(std::memory_order_acquire);
    if(state.self!=self||state.attachment!=attachment||state.generation!=generation||state.id!=id){
        restore();state={};state.self=self;state.attachment=attachment;state.entity=entity;state.id=id;state.generation=generation;state.requestEpoch=request;
    }else if(state.requestEpoch!=request){
        // Preserve only this exact item's already-qualified combined grip across
        // 3->physical->3 transitions. Never carry raw/committed bases interchangeably.
        if(controllerMode!=state.lastMode)state.gripValid=false;
        state.requestEpoch=request;
    }
    float original[8]{},wanted[8]{};
    if(!ReadAttachmentQs6689(attachment,original)){restore();state.gripValid=false;g_weapon_reason6694.store(3);return;}
    const auto now=static_cast<std::uint64_t>(GetTickCount64());
    if(!ControllerWeaponTransform6694(state,controllerMode,original,wanted)){
        // A new combined hand is unavailable: do not substitute unconstrained
        // controller coordinates. Retain this SAME source only until its original
        // capture deadline, never extending its age by using it again.
        const bool usable=state.changed&&state.lastMode==controllerMode&&g_gameplay_allowed.load()&&TrackingHeadUsable111(now)&&TrackingHandUsable111(0,now)&&
            kcd2vr::pose_core::fresh(state.lastSource,g_pose_epoch6689.load(),controllerMode==3?reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()):state.lastSource.actor,now)&&
            (controllerMode!=3||g_committed_pose_valid6689.load());
        if(!usable){if(state.changed)g_weapon_lost6695.fetch_add(1);restore();g_weapon_reason6694.store(controllerMode==3?9:4);return;}
        std::copy_n(state.lastWanted,8,wanted);
    }
    if(generation!=g_weapon_control_generation[0].load()||request!=g_weapon_request_epoch6694.load()||
       mode!=g_weapon_control_mode.load()||diagnostics::lab::NativeReference()||NativePassengerCurrentA34(now)||
       !g_gameplay_allowed.load()||!TrackingHeadUsable111(now)||!TrackingHandUsable111(0,now)) {restore();return;}
    std::uint32_t finalHandId=0;
    if(!NativeHeldItemId6694(0,finalHandId)||finalHandId!=id){restore();state={};g_weapon_reason6694.store(2);return;}
    bool child=false;if(!PlaceBothConsumers6689(self,entity,wanted,child)){
        restore();state.gripValid=false;g_weapon_fault6694.store(true);g_weapon_reason6694.store(6);
        diagnostic_trace::Write("V66.95-F12 PLACEMENT_FAULT nativeRestorationRequested=1 faultLatched=1");return;
    }
    state.changed=true;MarkWeaponPending6695(self,attachment);g_weapon_reason6694.store(7);
    const auto n=g_weapon_applied6694.fetch_add(1)+1;g_f12_attachment_proof_writes.fetch_add(1);
    if(n==1){bool childCheck=false;const bool readback=PlacementReadback6689(self,entity,id,wanted,childCheck);
        char m[448]{};std::snprintf(m,sizeof(m),"V66.95-F12 CONTROL_STARTED mode=%d id=%u nativeReadback=%d childConsumer=%d handSource=%s visibleProof=USER_REQUIRED timerProofGate=REMOVED",mode,id,readback,child,state.usedCommitted?"COMMITTED_HAND":"RAW_TRACKING");diagnostic_trace::Write(m);}
}

