#include "arrow_hold_d48.h"
// C63R2: place the SAME native-created hand ammunition, not a duplicate mesh.
// Serializing only this tiny owner keeps native remove/bind outside our placement.
// R96 xbow: the HAND ONLY crossbow bolt uses this same writer (bow pattern).
// Its serial is separate so the bow tick, which withdraws every frame while no
// bow is held, never takes the bolt away from the crossbow route.
static std::atomic<std::uint64_t> g_manual_arrow_xbow_serial_r96{0};
static bool CrossbowHandOnlyBoltR96() noexcept {return CrossbowReloadEnabledD65C()&&CrossbowHandOnlyR90();}
static void ManualArrowWithdraw64() noexcept {
    auto active=g_manual_arrow_active64.load(std::memory_order_acquire);
    if(!active||active==g_manual_arrow_xbow_serial_r96.load(std::memory_order_acquire))return; // none, or crossbow-owned
    if(!g_manual_arrow_active64.compare_exchange_strong(active,0,std::memory_order_acq_rel))return;
    (void)g_manual_arrow_lease64.publish({});
    (void)g_manual_arrow_achieved64.publish({});
}
static std::uint64_t ManualArrowBegin64() noexcept {
    const auto serial=g_manual_arrow_sequence64.fetch_add(1,std::memory_order_acq_rel)+1;
    g_manual_arrow_xbow_serial_r96.store(0,std::memory_order_release); // a bow arrow replaces any bolt lease
    g_manual_arrow_active64.store(serial,std::memory_order_release);return serial;
}
// R96 xbow: begin/end the crossbow bolt lease (native-frame thread only).
static std::uint64_t ManualArrowBeginCrossbowR96() noexcept {
    const auto serial=g_manual_arrow_sequence64.fetch_add(1,std::memory_order_acq_rel)+1;
    g_manual_arrow_xbow_serial_r96.store(serial,std::memory_order_release);
    g_manual_arrow_active64.store(serial,std::memory_order_release);return serial;
}
static void ManualArrowWithdrawCrossbowR96() noexcept {
    auto serial=g_manual_arrow_xbow_serial_r96.load(std::memory_order_acquire);
    if(!serial)return;
    if(g_manual_arrow_active64.compare_exchange_strong(serial,0,std::memory_order_acq_rel)){
        (void)g_manual_arrow_lease64.publish({});(void)g_manual_arrow_achieved64.publish({});}
    g_manual_arrow_xbow_serial_r96.store(0,std::memory_order_release);
}
// R98 xbow: crossbow bolt held forward like the game's own hand hold.
// Hand frame (right wrist joint 408, logged finger joints): -X = fingers,
// -Z = thumb/index side, Y = palm normal. +90 deg about Y maps -Z to -X.
// Rotation only; the translation is set later from the wrist and slt_0.
static QuatT28 CrossbowBoltForwardR98(const QuatT28& local) noexcept {
    QuatT28 turn{};turn.q[0]=0.f;turn.q[1]=0.70710678f;turn.q[2]=0.f;turn.q[3]=0.70710678f;
    turn.t[0]=turn.t[1]=turn.t[2]=0.f;
    auto out=AbsoluteFromRelative(turn,local);
    out.t[0]=local.t[0];out.t[1]=local.t[1];out.t[2]=local.t[2];
    return out;
}
static bool SameManualArrow64(const ManualArrowLease64& a,const ManualArrowLease64& b) noexcept {
    return a.serial&&a.serial==b.serial&&a.player==b.player&&a.model==b.model&&a.entity==b.entity&&a.id==b.id&&
        a.key==b.key&&a.ammo==b.ammo&&a.generation==b.generation&&a.epoch==b.epoch&&a.request==b.request;
}
static bool ManualArrowOwner64(const ManualArrowLease64& a,std::uint64_t now) noexcept {
    if(HandClaimBlockedT24(0,reinterpret_cast<std::uintptr_t>(g_committed_character6689.load()),a.epoch,a.request))return false;
    if(!a.serial||a.serial!=g_manual_arrow_active64.load(std::memory_order_acquire)||!a.id||!a.entity||
       !a.key||!a.ammo||!a.generation||!a.sample||!a.tick||now<a.tick||now-a.tick>80||
       a.epoch!=g_pose_epoch6689.load()||a.request!=g_weapon_request_epoch6694.load()||
       (a.serial==g_manual_arrow_xbow_serial_r96.load(std::memory_order_acquire)?!CrossbowHandOnlyBoltR96():
            (!StagedBowEnabled59()||g_manual_bow_fault596.load()))||NativePassengerCurrentA34(now)||!g_gameplay_allowed.load()||
       !TrackingHeadUsable111(now)||!TrackingHandUsable111(0,now)||!TrackingHandUsable111(1,now)||
       !ManualPlayer596(a.player)||EntityById6694(a.id)!=a.entity)return false;
    void* model=nullptr;void* owner=nullptr;std::uint32_t id=0;
    return Read(static_cast<unsigned char*>(a.player)+0x1B0,model)&&model==a.model&&model&&
        Read(static_cast<unsigned char*>(model)+0x20,owner)&&owner==a.player&&
        Read(static_cast<unsigned char*>(model)+0x2C,id)&&id==a.id;
}
static bool ManualArrowBinding64(const ManualArrowLease64& a,void* self,void* attachment,std::uint64_t now) noexcept {
    if(!ManualArrowOwner64(a,now)||!PhysVft6698(self,0x3A68F68)||
       !ManualBoundEntity19c4r4(attachment,a.id,a.entity))return false;
    void* object=nullptr;const char* name=nullptr;
    if(!Read(static_cast<unsigned char*>(attachment)+0x20,object)||object!=self||
       !Read(reinterpret_cast<void*>(WhBase()+0x40AC298),name)||!name)return false;
    return ManualFindHandAttachment19c4r4(a.model,name)==attachment&&ManualArrowOwner64(a,now);
}
static void ManualArrowPublish64(const ManualBow596& m,const PhysFrame6698& f,std::uint64_t tick,std::uint64_t sample) noexcept {
    if(!m.arrowSerial64||m.arrowSerial64!=g_manual_arrow_active64.load()||!ManualHandAttached19c4r4(m))return;
    ManualArrowLease64 a{};a.player=m.player;a.entity=m.entity;a.id=m.hand;a.serial=m.arrowSerial64;
    a.key=m.weapon;a.ammo=m.ammo;a.generation=m.generation;a.epoch=m.epoch;a.request=m.request;
    a.tick=std::min(tick,std::uint64_t(f.input.tick));a.sample=std::min(sample,std::uint64_t(f.input.sample));
    if(!Read(static_cast<unsigned char*>(m.player)+0x1B0,a.model)||!ManualArrowOwner64(a,GetTickCount64()))return;
    (void)g_manual_arrow_lease64.publish(a);
}
static bool SameManualArrowSocket64(const ManualArrowSocket64& a,const ManualArrowSocket64& b) noexcept {
    return SameManualArrow64(a.owner,b.owner)&&a.self==b.self&&a.attachment==b.attachment&&
        a.character==b.character&&a.joint==b.joint&&!std::memcmp(&a.relative,&b.relative,sizeof(a.relative));
}
static bool ManualArrowHelper64(void* entity,QuatT28& helper) noexcept {
    if(!g_attachment_helper_transform_original)return false;
    bool ok=false;__try{ok=g_attachment_helper_transform_original(entity,"slt_0",&helper,true);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    return ok&&ValidPose6689(helper)&&Len(FromT(helper))<3.f;
}
static void ManualArrowReceipt64(const ManualArrowLease64& a,const char* reason,std::uint64_t sample=0,double gap=100.) noexcept {
    const auto now=std::uint64_t(GetTickCount64());
    if(!diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,now))return;
    static thread_local std::uint64_t last=0,serial=0;
    static thread_local const char* previous=nullptr;
    if(serial==a.serial&&previous==reason&&now>=last&&now-last<500)return;
    serial=a.serial;previous=reason;last=now;char text[350]{};
    std::snprintf(text,sizeof(text),"C63R2 F11_STAGE HAND_ARROW_POSE serial=%llu entity=%u sourceSample=%llu sample=%llu reason=%s nockGapM=%.4f gripPoint=SLT0_AT_COMMITTED_WRIST nativeLifetime=UNCHANGED renderSubmission=UNOBSERVED",
        (unsigned long long)a.serial,a.id,(unsigned long long)a.sample,(unsigned long long)sample,reason,gap);
    diagnostic_trace::Write(text);
}
static bool ManualArrowAttachment64(void* self,void* attachment) noexcept {
    ManualArrowLease64 a{};std::uint32_t id=0;
    // Cheap exact entity prefilter before any character/pose/native helper walk.
    if(!g_manual_arrow_active64.load()||!g_manual_arrow_lease64.read(a)||!a.serial||
       !Read(static_cast<unsigned char*>(self)+8,id)||id!=a.id)return false;
    std::unique_lock<std::recursive_mutex> guard(g_manual_arrow_mutex64,std::try_to_lock);
    if(!guard.owns_lock())return ManualArrowBinding64(a,self,attachment,GetTickCount64()); // current exact ammo only; withdraw precedes native lifetime changes
    const auto now=std::uint64_t(GetTickCount64());
    if(!ManualArrowBinding64(a,self,attachment,now))return false;
    CommittedFrame6695 body{};
    if(!ReadCommittedPacket6695(body)||!body.valid[0]||body.stamp.epoch!=a.epoch||
       body.stamp.sample<a.sample||body.stamp.tick<a.tick)return false;
    ManualArrowSocket64 socket{};socket.owner=a;socket.self=self;socket.attachment=attachment;
    void* manager=nullptr;std::int16_t joint=-1;
    if(!PhysSocketContract101(WhBase())||!Read(static_cast<unsigned char*>(attachment)+0x28,manager)||
       !manager||manager!=g_henry_char_attachment_manager.load()||
       !Read(static_cast<unsigned char*>(manager)+0x70,socket.character)||
       reinterpret_cast<std::uintptr_t>(socket.character)!=body.stamp.actor||
       !Read(static_cast<unsigned char*>(attachment)+0x78,joint)||joint<0||
       !Read(static_cast<unsigned char*>(attachment)+0x9C,socket.relative)||!ValidPose6689(socket.relative))return false;
    socket.joint=joint;
    ManualArrowSocket64 previous{};
    if(g_manual_arrow_socket64.read(previous)&&SameManualArrow64(previous.owner,a)&&previous.helper){
        socket.nockHelper=previous.nockHelper;socket.helper=true;
    }else socket.helper=ManualArrowHelper64(a.entity,socket.nockHelper);
    if(!socket.helper){ManualArrowReceipt64(a,"NATIVE_NOCK_HELPER_UNAVAILABLE");return false;}
    if(!g_manual_arrow_socket64.publish(socket))return false;
    const auto& ready=body.arrow64;
    if(!ready.valid||!SameManualArrowSocket64(socket,ready.socket)){
        ManualArrowReceipt64(a,"WAIT_ORIGINAL_SOCKET_TRANSACTION",body.stamp.sample);return false;
    }
    ModelWorld6692 world{};float natural[8]{};
    if(!PhysAttachmentWorld164(attachment,body.stamp.actor,world)||!ReadAttachmentQs6689(attachment,natural))return false;
    auto target=PhysCurrentWorld164(PhysPose6698(ready.model),world);
    const auto wrist=PhysCurrentWorld164({{},{ready.wrist.x,ready.wrist.y,ready.wrist.z}},world);
    // The native animation can hold the shaft far ahead of slt_0. In VR the
    // closed right hand grips the actual nock endpoint. Preserve the correlated
    // native orientation, but derive translation from that endpoint, not the
    // animation's shaft offset. The entity's exact per-axis attachment scale
    // is the same scale consumed by PlaceBothConsumers6689 below.
    const auto helper=FromT(socket.nockHelper);
    float sx=0,sy=0,sz=0;
    if(!Read(static_cast<unsigned char*>(self)+0x0C,sx)||!Read(static_cast<unsigned char*>(self)+0x10,sy)||
       !Read(static_cast<unsigned char*>(self)+0x14,sz)||!std::isfinite(sx)||!std::isfinite(sy)||!std::isfinite(sz)||
       std::fabs(sx)<.0001f||std::fabs(sy)<.0001f||std::fabs(sz)<.0001f){
        (void)g_manual_arrow_achieved64.publish({});return false;
    }
    const pm::Vec3 scaledHelper{helper.x*sx*natural[7],helper.y*sy*natural[7],helper.z*sz*natural[7]};
    target.p=wrist.p-ph::point({target.q,{}},scaledHelper);
    if(!ph::finite(target)||!ph::finite(wrist)||!ManualArrowBinding64(a,self,attachment,GetTickCount64()))return false;
    float wanted[8]{float(target.q.x),float(target.q.y),float(target.q.z),float(target.q.w),
        float(target.p.x),float(target.p.y),float(target.p.z),natural[7]};
    // Once placement starts this consumer alone owns the callback, even if its
    // readback fails. Never append an original/native placement over a partial write.
    bool child=false,checked=false;
    if(!PlaceBothConsumers6689(self,a.entity,wanted,child)||
       !PlacementReadback6689(self,a.entity,a.id,wanted,checked)){
        (void)g_manual_arrow_achieved64.publish({});ManualArrowReceipt64(a,"PLACEMENT_READBACK_FAILED");return true;
    }
    ManualArrowAchieved64 achieved{};achieved.owner=a;achieved.sample=body.stamp.sample;achieved.tick=body.stamp.tick;
    Matrix34V6550 tm{};
    if(ready.bowValid&&GetEntityWorldTmV6552(a.entity,a.id,tm)&&ManualArrowBinding64(a,self,attachment,GetTickCount64())){
        // The achieved entity matrix already includes native per-axis scale.
        // slt_0 is the arrow's local NOCK helper, not a wrist/grip surrogate.
        const auto h=FromT(socket.nockHelper);
        achieved.arrowNock={tm.m[0]*h.x+tm.m[1]*h.y+tm.m[2]*h.z+tm.m[3],
            tm.m[4]*h.x+tm.m[5]*h.y+tm.m[6]*h.z+tm.m[7],tm.m[8]*h.x+tm.m[9]*h.y+tm.m[10]*h.z+tm.m[11]};
        const auto bow=PhysCurrentWorld164({{},{ready.bowNock.x,ready.bowNock.y,ready.bowNock.z}},world);
        achieved.bowNock={float(bow.p.x),float(bow.p.y),float(bow.p.z)};
        achieved.gap=Len(Sub(achieved.arrowNock,achieved.bowNock));achieved.valid=std::isfinite(achieved.gap);
    }
    (void)g_manual_arrow_achieved64.publish(achieved);
    ManualArrowReceipt64(a,achieved.valid?"ENTITY_AND_CHILD_READBACK":"PLACED_NO_CURRENT_BOW_TARGET",body.stamp.sample,achieved.gap);
    return true;
}
static void BuildManualArrowPoses64(CommittedFrame6695& packet,const QuatT28* original,std::size_t count,
    const ph::Input& input,const ph::Output& output,const ph::Profile& bow,
    const QuatT28* reference=nullptr,int handJoint=-1) noexcept {
    std::unique_lock<std::recursive_mutex> guard(g_manual_arrow_mutex64,std::try_to_lock);
    if(!guard.owns_lock())return;
    const auto now=std::uint64_t(GetTickCount64());ManualArrowSocket64 socket{};
    if(!g_manual_arrow_socket64.read(socket)||!packet.valid[0]||!packet.valid[1]||
       packet.stamp.epoch!=socket.owner.epoch||packet.stamp.sample<socket.owner.sample||packet.stamp.tick<socket.owner.tick||
       reinterpret_cast<std::uintptr_t>(socket.character)!=packet.stamp.actor||socket.joint<0||std::size_t(socket.joint)>=count||
       !ManualArrowBinding64(socket.owner,socket.self,socket.attachment,now))return;
    // Recheck CURRENT descriptor, not the destination left by a prior VR write.
    QuatT28 relative{};std::int16_t joint=-1;
    if(!Read(static_cast<unsigned char*>(socket.attachment)+0x78,joint)||joint!=socket.joint||
       !Read(static_cast<unsigned char*>(socket.attachment)+0x9C,relative)||
       std::memcmp(&relative,&socket.relative,sizeof(relative)))return;
    // D48: the current clip's hand-slot motion is not a controller-relative
    // grip. Use the anatomy reference already validated for THIS skeleton and
    // its explicit hand joint. No per-frame bone scan, new native getter, or
    // pointer-only cache can select a different joint or survive a rig change.
    const bool nativeTurn=kcd2vr::arrow_hold_d48::nativeTurn.load(std::memory_order_acquire);
    const bool stable=!nativeTurn&&reference&&handJoint>=0&&std::size_t(handJoint)<count&&
        ValidPose6689(reference[handJoint])&&ValidPose6689(reference[socket.joint]);
    if(!nativeTurn&&!stable){ManualArrowReceipt64(socket.owner,"REFERENCE_GRIP_UNAVAILABLE",packet.stamp.sample);return;}
    const auto arrow=AbsoluteFromRelative(stable?reference[socket.joint]:original[socket.joint],socket.relative);
    auto local=RelativeFromAbsolute(stable?reference[handJoint]:packet.native[0],arrow);
    // R98 xbow: a crossbow bolt has no native reload clip turning the hand slot
    // (the game plays crossbow_idle_loaded), so the slot gives the rig default
    // (logged localQ=-0.70711,0,0,0.70711 = -90 deg about hand X, native and
    // reference equal): bolt tip (slt_0 nock at -Y) along hand -Z = thumb side,
    // "turned 90 deg up". Native hold = tip along the fingers (hand -X, finger
    // joints sit at x=-0.08). One fixed +90 deg turn about hand +Y (palm normal).
    if(socket.owner.serial&&socket.owner.serial==g_manual_arrow_xbow_serial_r96.load(std::memory_order_acquire))
        local=CrossbowBoltForwardR98(local);
    if(!ValidPose6689(arrow)||!ValidPose6689(local)||Len(FromT(local))>1.5f)return;
    // D53: distinguish the selected animated grip from the stable anatomy grip
    // on the actual first held-arrow transaction. No new native reads/writes.
    if(diagnostics::capture168::active(diagnostics::capture168::Domain::Reload,now)){
        static thread_local std::uint64_t serial=0;static thread_local bool previousNative=false;
        if(serial!=socket.owner.serial||previousNative!=nativeTurn){
            serial=socket.owner.serial;previousNative=nativeTurn;
            const bool haveReference=reference&&handJoint>=0&&std::size_t(handJoint)<count&&
                ValidPose6689(reference[handJoint])&&ValidPose6689(reference[socket.joint]);
            QuatT28 referenceLocal{};
            if(haveReference)referenceLocal=RelativeFromAbsolute(reference[handJoint],AbsoluteFromRelative(reference[socket.joint],socket.relative));
            char note[420]{};std::snprintf(note,sizeof(note),"D53 F11_STAGE ARROW_BASIS serial=%llu entity=%u sample=%llu mode=%s socketJoint=%d handJoint=%d localQ=%.5f,%.5f,%.5f,%.5f referenceKnown=%d referenceQ=%.5f,%.5f,%.5f,%.5f nativeLifetime=UNCHANGED",
                (unsigned long long)socket.owner.serial,socket.owner.id,(unsigned long long)packet.stamp.sample,nativeTurn?"NATIVE_SLOT_TURN":"CONTROLLER_GRIP",socket.joint,handJoint,
                local.q[0],local.q[1],local.q[2],local.q[3],int(haveReference),referenceLocal.q[0],referenceLocal.q[1],referenceLocal.q[2],referenceLocal.q[3]);
            diagnostic_trace::Write(note);
        }
    }
    auto& out=packet.arrow64;out.socket=socket;out.model=AbsoluteFromRelative(packet.allowed[0],local);
    out.wrist=FromT(packet.allowed[0]);out.valid=ValidPose6689(out.model);
    out.bowValid=bow.bow()&&bow.key==socket.owner.key&&bow.generation==socket.owner.generation&&
        bow.bowNockQualified40&&bow.support&&output.active[1]&&ph::finite(output.weapons[1])&&
        input.allowed&&input.epoch==socket.owner.request&&input.bodyEpoch==socket.owner.epoch&&
        input.sample==packet.stamp.sample&&input.tick==packet.stamp.tick;
    if(out.bowValid){const auto p=ph::point(output.weapons[1],bow.grip[0].p);out.bowNock={float(p.x),float(p.y),float(p.z)};}
}
static double ManualArrowNockGap64(const ManualBow596& m,const PhysFrame6698& f) noexcept {
    ManualArrowAchieved64 a{};const auto now=std::uint64_t(GetTickCount64());
    if(!g_manual_arrow_achieved64.read(a)||!a.valid||a.owner.serial!=m.arrowSerial64||
       a.owner.player!=m.player||a.owner.entity!=m.entity||a.owner.id!=m.hand||a.owner.key!=m.weapon||a.owner.ammo!=m.ammo||
       a.owner.epoch!=m.epoch||a.owner.request!=m.request||a.owner.generation!=m.generation||
       !a.sample||a.sample>f.input.sample||!a.tick||f.input.tick<a.tick||f.input.tick-a.tick>80||
       !ManualArrowOwner64(a.owner,now)||!std::isfinite(a.gap))return 100.;
    return a.gap;
}

static void ManualArrowCue64(const PhysFrame6698& f) noexcept {
    namespace hp=kcd2vr::haptics101;
    (void)hp::publish_event(0,f.profiles[1].key,.30f,25,f.input.tick);
}
