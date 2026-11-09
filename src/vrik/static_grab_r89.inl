// R94: a small standalone scenery entity keeps an enabled native rigid body.
// The existing verified velocity servo drives it; no Entity transform or
// physics-disable write is made. This runs on the existing native game rhythm.
namespace static_grab_r89n {
namespace G=kcd2vr::static_grab_r89;
namespace tr=kcd2vr::touch_react_service60;
namespace pc=kcd2vr::passive169r1;
namespace nc=kcd2vr::npc_contact172;
namespace gp=kcd2vr::physics::grab_producer60;
namespace B=kcd2vr::brush_swap_r100; // R100 loose scenery props (swap)
using brush_swap_r100n::SwapReadBrushR100;using brush_swap_r100n::SwapHideR100;using brush_swap_r100n::SwapPhysicsR100;
using brush_swap_r100n::SwapPlaceR100;using brush_swap_r100n::SwapRestoreEntryR100;using brush_swap_r100n::SwapLoadEpisodeR100;
using brush_swap_r100n::SwapNoteReadyR100;
constexpr std::uintptr_t kStaticBodyRva=0x3A578C0,kRigidBodyRva=0x3A48DC0;
struct Held {
    std::uint32_t id=0;void* entity=nullptr;void* body=nullptr;void* proxy=nullptr;
    ph::Pose offset{};float scale[3]{},bounds[6]{};double mass=0,diameter=0;bool wasRigid=false;
    prop5916::Controller drive{};std::uintptr_t character=0;
    std::uint64_t claim=0,start=0,epoch=0,tracking=0,request=0,reportTick=0;
    char name[48]{},cls[32]{};
    bool swapR100=false; // R100: our loose copy of a brush, follows VrSceneryPropSwapR100
    prop5916::FirmHoldR116 firm{};bool firmActive=false; // R116 firm grip (VrPropFirmHoldR116)
};
struct Note {char text[96]{};std::uint64_t tick=0;bool pending=false;};
static Held g_held[2];static G::Edge g_edge[2];static G::Touch g_touch[2];static Note g_note[2];
static G::EntityClaims g_entities;
static pc::State g_contactMotionR94[2];
static kcd2vr::impact59c6::Seen g_impactSeenR94[2];
static kcd2vr::lab_prop58::Budget g_contactBudgetR94;
static std::uint64_t g_serial=0,g_lastShown=0;
static B::Table g_swapTable; // R100 (moved up in R124: Release() reads the prop material from it)
static bool Lua(const char* chunk) noexcept {
    auto exec=gore32n::g_lua_exec43.load(std::memory_order_acquire);return exec&&chunk&&exec(chunk);
}
static bool Capture(std::uint64_t now) noexcept {
    using namespace kcd2vr::diagnostics::capture168;
    return active(Domain::Physics,now)||active(Domain::Interaction,now)||active(Domain::Bug,now);
}
static void Row(const char* phase,int side,const Held& h,const char* reason,double extra=0,double extra2=0) noexcept {
    const auto now=std::uint64_t(GetTickCount64());if(!Capture(now))return;
    // R103: value2 = DRIVE: rotation error in degrees (target vs achieved); HELD: visible-hand vs controller gap in cm
    char row[480]{};std::snprintf(row,sizeof(row),
        "R94 STATIC_PROP phase=%s side=%d id=%u entity=%p body=%p class=%s diameterCm=%.0f massKg=%.3f wasRigid=%d claim=%llu reason=%s value=%.4f value2=%.2f drive=NATIVE_VELOCITY physics=ENABLED inventory=NATIVE_ONLY",
        phase,side,h.id,h.entity,h.body,h.cls,h.diameter*100,h.mass,h.wasRigid?1:0,
        (unsigned long long)h.claim,reason,extra,extra2);diagnostic_trace::Write(row);
}
static void Show(const char* message,std::uint64_t now) noexcept {
    if(!Capture(now))return;
    char row[200]{};std::snprintf(row,sizeof(row),"R94 STATIC_PROP refusal=\"%s\"",message);diagnostic_trace::Write(row);
    if(!kcd2vr::diagnostics::capture168::active(kcd2vr::diagnostics::capture168::Domain::Bug,now))return;
    if(!kcd2vr::visible_failure::safe(message)||(g_lastShown&&now>=g_lastShown&&now-g_lastShown<2000))return;
    char lua[200]{};std::snprintf(lua,sizeof(lua),"if Game and Game.SendInfoText then Game.SendInfoText(\"KCD2VR %s\") end",message);
    if(Lua(lua))g_lastShown=now;
}
static void Defer(int side,std::uint64_t now,const char* message) noexcept {
    auto& n=g_note[side];n={};std::snprintf(n.text,sizeof(n.text),"%s",message);n.tick=now;n.pending=true;
}
static void FlushNotes(std::uint64_t now) noexcept {
    for(int side=0;side<2;++side){auto& n=g_note[side];
        if(!n.pending||now<n.tick||now-n.tick<300)continue;n.pending=false;
        const auto use=G::pickupUseTick[side].load(std::memory_order_acquire);
        if(use&&use+G::kPickupYieldMs>=n.tick)continue;
        Show(n.text,now);
    }
}
static bool EntityClassR89(void* entity,char* out,std::size_t n) noexcept {
    if(!out||n<2)return false;out[0]=0;void* cls=nullptr;std::uintptr_t vf=0;const char* name=nullptr;
    if(!entity||!Read(static_cast<unsigned char*>(entity)+0x20,cls)||!cls||!Read(cls,vf)||vf!=WhBase()+0x3B282A0||
       !Read(static_cast<unsigned char*>(cls)+0x10,name)||!name)return false;
    std::size_t i=0;for(;i+1<n;++i){char c=0;if(!Read(name+i,c))return false;if(!c)break;out[i]=c;}
    out[i]=0;return i>0;
}
// Reuse the existing audited entity parent and physics-proxy contracts. A
// suspended/native-attached body is not ours even if GetPhysics still returns it.
static bool Body(void* entity,void*& body,void*& proxy,std::uintptr_t& rva) noexcept {
    body=proxy=nullptr;rva=0;std::uintptr_t link=0,parent=0,vf=0;std::uint32_t flags=0;
    if(!entity||!PhysVft6698(entity,kCEntityVftRva)||!Read(static_cast<unsigned char*>(entity)+0x98,link)||
       (link&&(!Read(reinterpret_cast<void*>(link+0x18),parent)||parent)))return false;
    static constexpr StageLeafPin596 pins[]{{0x5D70C4,{0x4C,0x8B,0x81,0xA8,0,0,0,0x45}}};
    if(!StageLeafPins596(pins)||!SmallPickupBodyR24(entity,body))return false;
    __try{proxy=reinterpret_cast<void*(__fastcall*)(void*,int)>(WhBase()+0x5D70C4)(entity,1);}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
    if(proxy){void* current=nullptr;
        if(!PhysVft6698(proxy,0x3AC3478)||!Read(static_cast<unsigned char*>(proxy)+0x28,flags)||(flags&0x40u)||
           !Read(static_cast<unsigned char*>(proxy)+0x38,current)||current!=body)return false;
    }else if(body)return false;
    if(!body)return true;
    void* foreign=nullptr;short type=0;
    if(!Read(body,vf)||vf<WhBase()||!Read(static_cast<unsigned char*>(body)+0x20,foreign)||foreign!=entity||
       !Read(static_cast<unsigned char*>(body)+0x28,type)||type!=2)return false;
    rva=vf-WhBase();return rva==kStaticBodyRva||rva==kRigidBodyRva;
}
static bool PoseOf(const Matrix34V6550& tm,ph::Pose& out,float (&scale)[3]) noexcept {
    const V3 x{tm.m[0],tm.m[4],tm.m[8]},y{tm.m[1],tm.m[5],tm.m[9]},z{tm.m[2],tm.m[6],tm.m[10]};
    scale[0]=Len(x);scale[1]=Len(y);scale[2]=Len(z);
    for(float v:scale)if(!std::isfinite(v)||v<.001f||v>100.f)return false;
    const Q4 q=QFromBasis(Normalize(x),Normalize(y),Normalize(z));
    out={{q.x,q.y,q.z,q.w},{tm.m[3],tm.m[7],tm.m[11]}};return ph::finite(out);
}
static bool FrameCurrent(const CommittedFrame6695& f,std::uint64_t now) noexcept {
    return (G::enabled.load(std::memory_order_acquire)||B::enabled.load(std::memory_order_acquire))&&PropInstantContextD78(now)&&
        g_stage_thread596.load(std::memory_order_acquire)==GetCurrentThreadId()&&
        f.stamp.actor&&f.stamp.actor==reinterpret_cast<std::uintptr_t>(g_committed_character6689.load(std::memory_order_acquire))&&
        f.stamp.epoch&&f.stamp.epoch==g_pose_epoch6689.load(std::memory_order_acquire)&&
        f.doorTrackingEpoch66&&f.doorTrackingEpoch66==tracking59c6::epoch.load(std::memory_order_acquire)&&
        f.stamp.sample&&G::recent(now,f.stamp.tick,80)&&TrackingHeadUsable111(now);
}
static bool ConstraintFree(int side,std::uint32_t id=0) noexcept {
    gp::Status s{};
    for(int h=0;h<2;++h)if(gp::ReadStatus(h,s)&&s.state!=gp::State::Idle&&
        (h==side||(id&&s.owner.entity==id)))return false;
    return true;
}
static bool HandFree(int side,std::uint64_t now) noexcept {
    namespace ma=kcd2vr::mutual_admission_t25;
    if(side<0||side>1||!PropHandFree596(side)||HeldInteractionVisual43()||
       action_time_owner59c11::NativePickupOutstandingD72()||!ConstraintFree(side))return false;
    if((kcd2vr::corpse_grip_cg1::claims.reserved()&(1u<<side))||ma::active[side].load(std::memory_order_acquire)||
       ma::doorClaimsH41[side].load(std::memory_order_acquire)||ma::pickupClaimsD72[side].load(std::memory_order_acquire))return false;
    {PropLock596 lock;if(!lock.owned||(g_prop596.id&&g_prop596.side==side))return false;}
    return !G::pickupYield(side,now);
}
static bool Inspect(Held& h,bool allowPickable,ph::Pose& object) noexcept {
    if(!h.id||G::ownsEntityR94(h.id)||!ConstraintFree(-1,h.id))return false;
    void* entity=EntityById6694(h.id);
    if(!entity||entity==g_last_player_entity.load(std::memory_order_acquire)||(h.entity&&h.entity!=entity))return false;
    h.entity=entity;char raw[64]{};
    if(!EntityClassR89(entity,raw,sizeof(raw)))return false;
    const bool pickable=!std::strcmp(raw,"PickableItem");
    if(G::classify(raw)!=G::ClassKind::Grab&&!(allowPickable&&pickable))return false;
    G::cleanName(raw,h.cls,sizeof(h.cls));(void)EntityNameV178r19(entity,h.id,raw,sizeof(raw));G::cleanName(raw,h.name,sizeof(h.name));
    Matrix34V6550 tm{};void* render=nullptr;
    if(!GetEntityWorldTmV6552(entity,h.id,tm)||!SmallPickupBoundsR24(entity,render,h.bounds))return false;
    const auto size=G::measure(h.bounds,tm.m);h.diameter=size.diameter;h.mass=G::massKg(size.volume);
    if(!size.ok||size.diameter>B::maxDiameter()||!PoseOf(tm,object,h.scale))return false; // R158: 65 cm, or 1 m with VrSceneryBigPropsR158
    std::uintptr_t rva=0;if(!Body(entity,h.body,h.proxy,rva))return false;
    h.wasRigid=rva==kRigidBodyRva;
    // PickableItem has one preparation owner, SmallContactAdmissionR24. This
    // consumer can push its existing rigid body but never recreates the item.
    if(pickable&&!h.wasRigid)return false;
    {PropLock596 lock;if(!lock.owned||(g_prop596.id==h.id)||(g_prop596.body&&g_prop596.body==h.body))return false;}
    return EntityById6694(h.id)==entity;
}
static bool Prepare(Held& h,ph::Pose& object) noexcept {
    if(!PropPhysicsPins5916C1())return false;
    const auto before=object;void* prior=h.body;
    char lua[384]{};
    const int n=std::snprintf(lua,sizeof(lua),
        "assert(KCD2VR_Lab and KCD2VR_Lab.PrepareSceneryR94 and KCD2VR_Lab.PrepareSceneryR94(%u,'%s',%.6f,%s),'KCD2VR scenery preparation refused')",
        h.id,h.cls,h.mass,h.wasRigid?"false":"true");
    if(n<=0||std::size_t(n)>=sizeof(lua)||!Lua(lua)){Row("REFUSED",-1,h,"SCRIPT_PREPARATION_REFUSED");return false;}
    // A nonthrowing void Physicalize is not success. Read the current exact
    // native rigid body, finite mass and unchanged render geometry afterwards.
    void* body=nullptr;void* proxy=nullptr;std::uintptr_t rva=0;prop5916::Dynamics d{};
    Matrix34V6550 tm{};void* render=nullptr;float bounds[6]{},scale[3]{};char cls[64]{};
    if(EntityById6694(h.id)!=h.entity||!EntityClassR89(h.entity,cls,sizeof(cls))||std::strcmp(cls,h.cls)||
       !Body(h.entity,body,proxy,rva)||rva!=kRigidBodyRva||(h.wasRigid&&body!=prior)||
       !PropDynamics5916C1(body,d)||d.mass>G::kMaxMass||!GetEntityWorldTmV6552(h.entity,h.id,tm)||
       !SmallPickupBoundsR24(h.entity,render,bounds)||std::memcmp(bounds,h.bounds,sizeof(bounds))||
       !PoseOf(tm,object,scale)||pm::length(object.p-before.p)>.01||pm::angle(object.q,before.q)>.02||
       EntityById6694(h.id)!=h.entity){Row("REFUSED",-1,h,"RIGID_MASS_OR_GEOMETRY_NOT_CONFIRMED");return false;}
    for(int i=0;i<3;++i)if(std::fabs(scale[i]-h.scale[i])>.001f)return false;
    h.body=body;h.proxy=proxy;h.mass=d.mass;return true;
}
static bool Current(const Held& h,ph::Pose& actual,prop5916::Dynamics& d) noexcept {
    void* body=nullptr;void* proxy=nullptr;std::uintptr_t rva=0;Matrix34V6550 tm{};float scale[3]{};
    if(!h.entity||!h.body||EntityById6694(h.id)!=h.entity||!Body(h.entity,body,proxy,rva)||
       body!=h.body||proxy!=h.proxy||rva!=kRigidBodyRva||!PropDynamics5916C1(body,d)||
       !GetEntityWorldTmV6552(h.entity,h.id,tm)||!PoseOf(tm,actual,scale))return false;
    for(int i=0;i<3;++i)if(std::fabs(scale[i]-h.scale[i])>.001f)return false;
    char cls[64]{};void* finalBody=nullptr;void* finalProxy=nullptr;std::uintptr_t finalRva=0;
    return EntityById6694(h.id)==h.entity&&EntityClassR89(h.entity,cls,sizeof(cls))&&!std::strcmp(cls,h.cls)&&
        Body(h.entity,finalBody,finalProxy,finalRva)&&finalBody==h.body&&finalProxy==h.proxy&&finalRva==kRigidBodyRva;
}
// R103: the hand the user sees (collision-solved "allowed" wrist of the same committed frame).
// Falls back to the controller hand when that slot is not finite.
static ph::Pose CarryHandR103(const CommittedFrame6695& f,int side) noexcept {
    const auto seen=PropWorldHand596(f,side);
    return f.valid[side]&&ph::finite(seen)?seen:PropRequestedWorld5916C1(f,side);
}
// R103 (props rotate in the hand): the rotation part of the servo was capped at 80 rad/s^2 per kg, so a
// 1.3-1.9 kg plate/jug could change its spin by only ~0.5 rad/s per frame and lagged/rotated when the
// wrist turned. Linear force 40 -> 80 N, angular 80 -> 300 rad/s^2 (per kg), 12 -> 20 rad/s, gain 12 -> 14.
// Leash (0.35 m) and throw speed (8 m/s) unchanged.
static prop5916::Limits CarryLimitsR103() noexcept {
    prop5916::Limits l{};l.force=80;l.angularAcceleration=300;l.angularSpeed=20;l.rotationGain=14;return l;
}
// R116: monotonic seconds for the firm hold (GetTickCount64 moves in 15.6 ms steps, see prop_drive5916c1.h)
static double NowSecondsR116() noexcept {
    return std::chrono::duration<double>(std::chrono::steady_clock::now().time_since_epoch()).count();
}
// R116 "no sound when it falls": the retail collision-event producer (BA7168) only logs a contact when the
// body flags have bit 25 = pef_log_collisions (pinned "bt esi,19h" at BA7191, native_weapon_contact_event_h24.h),
// and only logged contacts reach the game's material effects (cup on wood/stone sound+particles). Our loose
// copies are fresh Lua BasicEntity rigid bodies; this adds that one flag with the same pe_params_flags (type 15)
// packet and the same pinned rigid SetParams (6F1BE4) the hand proxy uses (native_hand_proxy_owner60.inl:252).
// Kept after release (the fall is what should sound). Nothing else on the body changes.
struct FlagsPacketR116 {int type=15;std::uint32_t flags=0x80000000u,flagsOR=0x02000000u,flagsAND=~0u;};
static_assert(sizeof(FlagsPacketR116)==16);
static bool SendFlagsR116(std::uintptr_t fn,std::uintptr_t body,const FlagsPacketR116* p) noexcept {
    __try {return reinterpret_cast<int(__fastcall*)(void*,const void*,int)>(fn)(reinterpret_cast<void*>(body),p,0)>0;}
    __except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
static void ImpactEventsR116(const Held& h) noexcept {
    const auto body=reinterpret_cast<std::uintptr_t>(h.body);std::uintptr_t fn=0;std::uint32_t before=0,after=0;
    const char* result="REFUSED_BODY_OR_SETPARAMS_PIN";
    if(body&&self_collision_r103n::Setter(body,kcd2vr::self_collision_r103::Kind::Prop,fn)&&Read(reinterpret_cast<void*>(body+0x84),before)){
        if(before&0x02000000u)result="ALREADY_LOGGED";
        else{const FlagsPacketR116 p{};
            result=!SendFlagsR116(fn,body,&p)?"SETPARAMS_REFUSED":
                (Read(reinterpret_cast<void*>(body+0x84),after)&&(after&0x02000000u))?"SET":"QUEUED_BY_PHYSICS_STEP";}
    }
    char row[220]{};std::snprintf(row,sizeof(row),"R116 PROP_IMPACT_EVENTS id=%u body=%p flagsBefore=0x%X flagsAfter=0x%X result=%s",
        h.id,h.body,before,after,result);
    diagnostic_trace::Write(row);
}
static void Publish(int side,const Held& h,const CommittedFrame6695& f,ph::Pose actual,std::uint64_t now) noexcept {
    prop5916::Presentation p{};p.character=h.character;p.body=reinterpret_cast<std::uintptr_t>(h.body);
    p.bodyEpoch=h.epoch;p.key=p.instantTokenD78=h.claim;p.observationTick=now;
    p.sourceTick=f.stamp.tick;p.sequence=f.stamp.sample;p.side=side;
    p.wristWorld=ph::compose(actual,ph::inverse(h.offset));(void)G::presentationR94[side].publish(p);
}
// R124 (R123 run: "clink sound when a prop falls: no sound"): our loose props play the game's own put-down sound
// when they hit something (prop_impact_sound_r124.h). Watched after a release or a knock, every native frame.
namespace PI=kcd2vr::prop_impact_sound_r124;
static PI::Watch g_impactR124;
static std::uint64_t g_impactRefusedRowR124=0;
static void WatchImpactR124(std::uint32_t id,PI::Kind kind,std::uint64_t now) noexcept {
    if(PI::enabled.load(std::memory_order_acquire))g_impactR124.add(id,kind,now);
}
// true = the audio system got the trigger (prop proxy first, Henry's proxy if the prop entity refuses)
static bool PlayImpactR124(const PI::Slot& s,double change,std::uint64_t now) noexcept {
    const char* name=PI::trigger(s.kind);char lua[520]{};const char* route="NONE";
    int n=PI::chunk(lua,sizeof(lua),s.id,name,false);
    if(n>0&&std::size_t(n)<sizeof(lua)&&Lua(lua))route="PROP";
    else{n=PI::chunk(lua,sizeof(lua),s.id,name,true);if(n>0&&std::size_t(n)<sizeof(lua)&&Lua(lua))route="HENRY";}
    const bool played=std::strcmp(route,"NONE")!=0;
    if(Capture(now)){char row[240]{};std::snprintf(row,sizeof(row),"R124 PROP_IMPACT_SOUND id=%u kind=%s trigger=%s speedChange=%.2f route=%s",
        s.id,PI::kindName(s.kind),name,change,route);diagnostic_trace::Write(row);}
    else if(!played&&(!g_impactRefusedRowR124||now<g_impactRefusedRowR124||now-g_impactRefusedRowR124>=30000)){
        g_impactRefusedRowR124=now; // the reason goes to KCD2VR.log (at most every 30 s); the next impact tries again
        char row[240]{};std::snprintf(row,sizeof(row),"R124 PROP_IMPACT_SOUND not played: Lua has no Sound.GetAudioTriggerID('%s') / ExecuteAudioTrigger on prop %u or Henry (script not ready?) - next impact tries again",
            name,s.id);diagnostic_trace::Write(row);
    }
    return played;
}
static void ImpactSoundsR124(std::uint64_t now) noexcept {
    if(!g_impactR124.any())return;
    if(!PI::enabled.load(std::memory_order_acquire)){g_impactR124.clear();return;} // OFF: silent props, nothing watched
    const double t=NowSecondsR116();bool soundThisFrame=false;
    for(auto& s:g_impactR124.s){
        if(!s.id)continue;
        if(PI::Watch::expired(s,now)||G::ownsEntityR94(s.id)){s=PI::Slot{};continue;} // held again: watched from its next release
        void* entity=EntityById6694(s.id);void* body=nullptr;void* proxy=nullptr;std::uintptr_t rva=0;prop5916::Dynamics d{};
        if(!entity){s=PI::Slot{};continue;}
        if(!Body(entity,body,proxy,rva)||rva!=kRigidBodyRva||!body||!PropDynamics5916C1(body,d)){s.primed=false;continue;}
        const double v[3]{d.linear.x,d.linear.y,d.linear.z};
        const double change=PI::Watch::step(s,v,t,now);
        if(!soundThisFrame&&PI::Watch::due(s,change,now)){s.lastSound=now;soundThisFrame=true;(void)PlayImpactR124(s,change,now);}
    }
}
static PI::Kind ImpactKindR124(const Held& h) noexcept {
    if(const auto* e=g_swapTable.byEntity(h.id);e&&e->soundKind<unsigned(PI::Kind::Count))return PI::Kind(e->soundKind);
    return PI::kindFor(h.name);
}
static void Release(int side,std::uint64_t now,const char* reason) noexcept {
    const auto h=g_held[side];if(!h.id)return;
    // Physics was never disabled and there is no constraint to remove. Retire
    // the motor/presentation without a new velocity write to any old body.
    g_held[side]={};G::clearPresentationR94(side);G::heldEntityR94[side].store(0,std::memory_order_release);
    G::heldBodyR103[side].store(nullptr,std::memory_order_release);G::heldEntityPtrR103[side].store(nullptr,std::memory_order_release);
    // R103: Henry collides with the prop again once it is clear of him (or after 4 s); momentum stays native
    self_collision_r103n::Release(kcd2vr::self_collision_r103::propOwner(side),now?now:std::uint64_t(GetTickCount64()));
    g_entities.retire(side,h.id);(void)kcd2vr::mutual_admission_t25::RetirePickupD72(side,h.claim);
    Row("RELEASE",side,h,reason);
    // R124: our loose copy (or a static prop we made rigid) now falls/lands: listen for its impacts
    if(h.swapR100||!h.wasRigid)WatchImpactR124(h.id,ImpactKindR124(h),now?now:std::uint64_t(GetTickCount64()));
}
// R96: the R94R1 run (KCD2VR2.log 17:53:15-17:53:24) had 10 grab refusals and
// all 103 hand contact rays hit a BASE static body with no entity owner (the
// table/shelf under the mug, or a brush). The contact ray alone can never name
// the mug. On a Grip close after such a contact, ask the physics world which
// bodies are around the palm (same retail GetEntitiesInBox the anvil uses, read
// only) and take the smallest one that contains the palm.
// R100: that mug WAS the brush (foreign id 1, CBrush owner, prefab Type="Brush").
// The same query now also returns static brush bodies; a brush <= 65 cm becomes a
// loose copy (SwapGrabR100). Entities still need the old switch, except our own
// loose copies, which follow VrSceneryPropSwapR100.
static std::uint32_t g_swapSerial=0;
static int BoxQueryR96(kcd2vr::subsystems::smithing::anvil_physics_a3::InBoxFn fn,void* world,const float* mn,const float* mx,void*** list) noexcept {
    __try {return fn(world,mn,mx,list,1|2|4,int(kcd2vr::subsystems::smithing::anvil_physics_a3::kMaxCandidates));} // static, sleeping rigid, rigid
    __except(EXCEPTION_EXECUTE_HANDLER){return -1;}
}
static B::Pick NearbyR100(int side,const CommittedFrame6695& f,G::Touch& out,B::Brush& brush,const char*& why) noexcept {
#ifdef KCD2VR_STATIC_GRAB_HOST_TEST_R96
    return HostNearbyR100(side,f,out,brush,why);
#endif
    namespace A=kcd2vr::subsystems::smithing::anvil_physics_a3;
    why="BOX_QUERY_CONTRACT";const auto base=WhBase();
    if(!base||A::faulted.load(std::memory_order_acquire))return B::Pick::None;
    const ph::Pose hand=PropRequestedWorld5916C1(f,side);if(!ph::finite(hand))return B::Pick::None;
    void* world=nullptr;std::uintptr_t vft=0,slot=0;unsigned char prologue[sizeof(A::kInBoxPrologue)]{};
    if(!A::readRaw(base+A::kWorldSlotRva,&world,sizeof(world))||!world||
       !A::readRaw(reinterpret_cast<std::uintptr_t>(world),&vft,sizeof(vft))||vft!=base+A::kWorldVtableRva||
       !A::readRaw(vft+A::kInBoxSlot,&slot,sizeof(slot))||slot!=base+A::kInBoxRva||
       !A::readRaw(slot,prologue,sizeof(prologue))||std::memcmp(prologue,A::kInBoxPrologue,sizeof(prologue)))return B::Pick::None;
    constexpr float r=.10f;
    const float mn[3]{float(hand.p.x)-r,float(hand.p.y)-r,float(hand.p.z)-r},mx[3]{float(hand.p.x)+r,float(hand.p.y)+r,float(hand.p.z)+r};
    void* own[A::kMaxCandidates]{};void** list=own;
    const int count=BoxQueryR96(reinterpret_cast<A::InBoxFn>(slot),world,mn,mx,&list);
    if(count<0){A::faulted.store(true,std::memory_order_release);why="BOX_QUERY_FAULT";return B::Pick::None;}
    why="NO_BODY_AROUND_PALM";if(!count||!list)return B::Pick::None;
    const unsigned n=std::min<unsigned>(unsigned(count),A::kMaxCandidates);
    void* henry=g_last_player_entity.load(std::memory_order_acquire);
    B::Candidate c[A::kMaxCandidates]{};unsigned m=0;
    for(unsigned i=0;i<n;++i){
        void* e=nullptr;B::Candidate x{};void* owner=nullptr;std::uintptr_t ownerVf=0;
        if(!A::readRaw(reinterpret_cast<std::uintptr_t>(list+i),&e,sizeof(e))||!e||
           !A::readRaw(reinterpret_cast<std::uintptr_t>(e)+0x08,x.bb,sizeof(x.bb))||
           !A::readRaw(reinterpret_cast<std::uintptr_t>(e)+0x20,&owner,sizeof(owner))||
           !A::readRaw(reinterpret_cast<std::uintptr_t>(e)+0x28,&x.foreign,sizeof(x.foreign))||!owner||owner==henry)continue;
        x.body=reinterpret_cast<std::uintptr_t>(e);x.owner=reinterpret_cast<std::uintptr_t>(owner);
        if(x.foreign==B::kForeignEntity){
            if(!Read(owner,ownerVf)||ownerVf!=base+kCEntityVftRva||!Read(static_cast<unsigned char*>(owner)+0xC,x.entity)||!x.entity)continue;
            x.swapped=g_swapTable.hasEntity(x.entity);
        }else if(x.foreign!=B::kForeignStatic)continue; // terrain (0) and anything else: never
        c[m++]=x;
    }
    const double palm[3]{hand.p.x,hand.p.y,hand.p.z};const char* brushWhy=nullptr;
    auto valid=[&](unsigned i,B::Pick kind) noexcept->bool{
        const auto& x=c[i];
        if(kind==B::Pick::Entity){
            Held h{};h.id=x.entity;h.entity=reinterpret_cast<void*>(x.owner);ph::Pose object{};
            if(!Inspect(h,false,object))return false;
            out=G::Touch{};out.entity=x.entity;out.owner=x.owner;
            out.collider=reinterpret_cast<std::uintptr_t>(h.body);out.rigid=h.wasRigid;
            out.actor=f.stamp.actor;out.epoch=f.stamp.epoch;out.request=g_contact_control_epoch6695.load(std::memory_order_acquire);
            out.tick=f.stamp.tick;return true;
        }
        B::Brush b{};const char* w=nullptr;char path[sizeof(b.path)]{};
        if(g_swapTable.byBrush(x.owner)||g_swapTable.byBody(x.body)){brushWhy="ALREADY_SWAPPED";return false;}
        if(!SwapReadBrushR100(x.body,b,w)){brushWhy=w;return false;}
        if(const char* rule=B::brushRule(b)){brushWhy=rule;return false;}
        if(!B::cleanPath(b.path,path,sizeof(path),w)){brushWhy=w;return false;}
        std::memcpy(b.path,path,sizeof(path));brush=b;return true;
    };
    const auto choice=B::choose(palm,c,m,G::enabled.load(std::memory_order_acquire),B::enabled.load(std::memory_order_acquire),valid);
    why=choice.kind==B::Pick::None&&brushWhy?brushWhy:choice.why;
    return choice.kind;
}
static void SwapResultR100(int side,const B::Brush& b,const char* result,const char* why,std::uint32_t id,double mass) noexcept {
    // one line per swap grab result, always (no on-screen text)
    char row[520]{};std::snprintf(row,sizeof(row),
        "R100 BRUSH_SWAP side=%d result=%s reason=%s model=%s geo=%s sub=%d parent=%d material=%s brush=%p body=%p entity=%u massKg=%.2f sizeCm=%.0f swapped=%u",
        side,result,why?why:"?",b.path[0]?b.path:"-",b.geo[0]?b.geo:"-",b.subCount,b.parent?1:0,b.material?"OVERRIDE":"FILE",
        reinterpret_cast<void*>(b.brush),reinterpret_cast<void*>(b.body),id,mass,B::boxOk(b.bb)?B::diameterOf(b.bb)*100:0.,g_swapTable.n);
    diagnostic_trace::Write(row);
}
static bool AcquireR100(int side,std::uint64_t now,const CommittedFrame6695& f,const G::Touch& t,bool swapped) noexcept;
// F5 detail: one row per swap step (only while an F5 capture runs)
static bool SwapStepR100(const char* step,bool ok,int side,const B::Brush& b,std::uint32_t serial,std::uint32_t id=0) noexcept {
    const auto now=std::uint64_t(GetTickCount64());if(!Capture(now))return ok;
    char row[300]{};std::snprintf(row,sizeof(row),"R100 BRUSH_SWAP_STEP step=%s ok=%d side=%d serial=%u brush=%p body=%p entity=%u model=%s",
        step,ok?1:0,side,serial,reinterpret_cast<void*>(b.brush),reinterpret_cast<void*>(b.body),id,b.path);
    diagnostic_trace::Write(row);return ok;
}
// Loose copy for one brush: hide, suspend physics, spawn NOT SAVED BasicEntity, place,
// grab. Order and rollback: brush_swap_r100::swap.
static void SwapGrabR100(int side,std::uint64_t now,const CommittedFrame6695& f,const B::Brush& b) noexcept {
    const double mass=B::massFor(b.bb);
    if(!B::enabled.load(std::memory_order_acquire))return;
    if(g_swapTable.byBrush(b.brush)||g_swapTable.byBody(b.body)){SwapResultR100(side,b,"REFUSED","ALREADY_SWAPPED",0,mass);return;}
    if(g_swapTable.n>=B::kTableMax){SwapResultR100(side,b,"REFUSED","SWAP_TABLE_FULL_UNTIL_NEXT_LOAD",0,mass);return;}
    Matrix34V6550 tm{};std::memcpy(tm.m,b.tm,sizeof(tm.m));ph::Pose pose{};float scale[3]{};
    if(!PoseOf(tm,pose,scale)){SwapResultR100(side,b,"REFUSED","BAD_TRANSFORM",0,mass);return;}
    if(!SwapNoteReadyR100()){SwapResultR100(side,b,"REFUSED","NOTE_CHANNEL_NOT_INSTALLED",0,mass);return;}
    if(!HandFree(side,now)){SwapResultR100(side,b,"REFUSED","HAND_NOT_FREE",0,mass);return;}
    struct Leaves {
        const B::Brush& b;int side;std::uint64_t now;const CommittedFrame6695& f;ph::Pose pose;float scale[3];double mass;std::uint32_t serial=0;
        bool hide(bool on) noexcept {return SwapStepR100(on?"HIDE":"SHOW",SwapHideR100(b.brush,on),side,b,serial);}
        bool suspend() noexcept {return SwapStepR100("PHYSICS_SUSPEND",SwapPhysicsR100(b.body,1),side,b,serial);}
        bool restore() noexcept {return SwapStepR100("PHYSICS_RESTORE",SwapPhysicsR100(b.body,2),side,b,serial);}
        bool spawn(std::uint32_t& id) noexcept {
            id=0;serial=(++g_swapSerial&0xFFFFFFu);if(!serial)serial=g_swapSerial=1;
            char lua[480]{};const int n=std::snprintf(lua,sizeof(lua),
                "assert(KCD2VR_Lab and KCD2VR_Lab.SwapSpawnR100 and KCD2VR_Lab.SwapSpawnR100(%u,'%s',%.6f,%.6f,%.6f,%.6f,%.6f,%.6f,%.4f),'KCD2VR scenery swap spawn refused')",
                serial,b.path,pose.p.x,pose.p.y,pose.p.z,double(scale[0]),double(scale[1]),double(scale[2]),mass);
            if(n<=0||std::size_t(n)>=sizeof(lua))return false;
            B::noteEntity.store(0,std::memory_order_release);B::notePending.store(serial,std::memory_order_release);
            const bool ran=Lua(lua);
            B::notePending.store(0,std::memory_order_release);
            id=B::noteEntity.exchange(0,std::memory_order_acq_rel);
            if(!ran&&id){(void)remove(id);id=0;}
            return SwapStepR100("SPAWN_NOT_SAVED_BASICENTITY",ran&&id&&EntityById6694(id),side,b,serial,id);
        }
        bool place(std::uint32_t id) noexcept {return SwapStepR100("PLACE_SAME_TRANSFORM",placed(id),side,b,serial,id);}
        bool placed(std::uint32_t id) noexcept {
            void* e=EntityById6694(id);if(!e||!SwapPlaceR100(e,pose,scale))return false;
            Matrix34V6550 back{};ph::Pose got{};float s[3]{};
            if(EntityById6694(id)!=e||!GetEntityWorldTmV6552(e,id,back)||!PoseOf(back,got,s))return false;
            for(int i=0;i<3;++i)if(std::fabs(s[i]-scale[i])>.01f*std::max(1.f,scale[i]))return false;
            return pm::length(got.p-pose.p)<=.01&&pm::angle(got.q,pose.q)<=.02;
        }
        bool grab(std::uint32_t id) noexcept {
            void* e=EntityById6694(id);if(!e)return false;
            Held h{};h.id=id;h.entity=e;ph::Pose object{};if(!Inspect(h,false,object))return false;
            G::Touch t{};t.entity=id;t.owner=reinterpret_cast<std::uintptr_t>(e);t.collider=reinterpret_cast<std::uintptr_t>(h.body);
            t.rigid=h.wasRigid;t.actor=f.stamp.actor;t.epoch=f.stamp.epoch;
            t.request=g_contact_control_epoch6695.load(std::memory_order_acquire);t.tick=f.stamp.tick;
            return SwapStepR100("GRAB_SMALL_PROP_CARRY",AcquireR100(side,now,f,t,true),side,b,serial,id);
        }
        bool remove(std::uint32_t id) noexcept {
            char lua[160]{};std::snprintf(lua,sizeof(lua),"assert(KCD2VR_Lab and KCD2VR_Lab.SwapRemoveR100 and KCD2VR_Lab.SwapRemoveR100(%u),'KCD2VR swap remove refused')",id);
            (void)Lua(lua);return SwapStepR100("REMOVE_LOOSE_COPY",EntityById6694(id)==nullptr,side,b,serial,id);
        }
    } leaves{b,side,now,f,pose,{scale[0],scale[1],scale[2]},mass};
    const auto o=B::swap(leaves);
    if(o.ok||o.keptLoose){B::Entry added{b.brush,b.body,b.stat,o.entity,leaves.serial};
        added.soundKind=static_cast<unsigned char>(PI::kindFor(b.path)); // R124: impact sound by the .cgf path
        (void)g_swapTable.add(added);}
    SwapResultR100(side,b,o.ok?"SWAPPED":o.keptLoose?"LOOSE_COPY_KEPT":o.rollbackClean?"REFUSED_ROLLED_BACK":"REFUSED_ROLLBACK_INCOMPLETE",o.why,o.entity,mass);
}
static void TryGrab(int side,std::uint64_t now,const CommittedFrame6695& f) noexcept {
    G::Touch t=g_touch[side];
    if(!HandFree(side,now)||!FrameCurrent(f,now)||!f.valid[side]||!f.propRequestedValid5916c1[side])return;
    if(!G::recent(now,t.tick,G::kTouchMemoryMs)){
        // Any Grip close on an empty free hand asks; the refusal line is only
        // written when the hand really touched something (no spam on air grips).
        const bool touchedWorld=G::recent(now,t.worldTick,G::kTouchMemoryMs);
        const char* why=nullptr;B::Brush brush{};
        const auto kind=NearbyR100(side,f,t,brush,why);
        if(kind==B::Pick::Brush){SwapGrabR100(side,now,f,brush);return;}
        if(kind!=B::Pick::Entity){
            if(touchedWorld){char msg[96]{};std::snprintf(msg,sizeof(msg),"grab: no movable object at the palm (%s)",why?why:"?");
                Defer(side,now,msg);
                if(B::enabled.load(std::memory_order_acquire)){B::Brush none{};SwapResultR100(side,none,"REFUSED",why,0,0);}}
            return;
        }
        Held probe{};probe.id=t.entity;Row("NEARBY_ENTITY_R96",side,probe,"PALM_INSIDE_ENTITY_BOX");
    }
    const bool swapped=g_swapTable.hasEntity(t.entity);
    if(!G::enabled.load(std::memory_order_acquire)&&!(swapped&&B::enabled.load(std::memory_order_acquire)))return;
    (void)AcquireR100(side,now,f,t,swapped);
}
static bool AcquireR100(int side,std::uint64_t now,const CommittedFrame6695& f,const G::Touch& t,bool swapped) noexcept {
    if(t.actor!=f.stamp.actor||t.epoch!=f.stamp.epoch||t.request!=g_contact_control_epoch6695.load(std::memory_order_acquire))return false;
    Held h{};h.id=t.entity;h.entity=reinterpret_cast<void*>(t.owner);ph::Pose object{};
    if(!Inspect(h,false,object)){Row("REFUSED",side,h,"CLASS_BODY_BOUNDS_OR_NATIVE_OWNER");return false;}
    if(reinterpret_cast<std::uintptr_t>(h.body)!=t.collider){Row("REFUSED",side,h,"TOUCHED_BODY_REPLACED");return false;}
    const ph::Pose hand=PropRequestedWorld5916C1(f,side);
    Matrix34V6550 tm{};if(!GetEntityWorldTmV6552(h.entity,h.id,tm))return false;
    const auto size=G::measure(h.bounds,tm.m);
    const pm::Vec3 centre{size.centre[0],size.centre[1],size.centre[2]};
    const double reach=pm::length(hand.p-centre);
    if(!ph::finite(hand)||!size.ok||reach>G::maxReachR158(size.diameter)){Row("REFUSED",side,h,"REACH",reach);return false;} // R158: 30 cm, more for a big prop
    h.claim=0x5294000000000000ull|(++g_serial&0xFFFFFFFFFFFFull);h.swapR100=swapped;
    if(!kcd2vr::mutual_admission_t25::RetainPickupD72(side,h.claim))return false;
    if(!g_entities.retain(side,h.id)){
        (void)kcd2vr::mutual_admission_t25::RetirePickupD72(side,h.claim);return false;
    }
    G::heldEntityR94[side].store(h.id,std::memory_order_release);
    // Publish exclusion before the script/native leaves. Failed preparation
    // leaves no paused-physics debt and never begins a transform-only carry.
    g_held[side]=h;
    if(!ConstraintFree(side,h.id)||!Prepare(h,object)||!FrameCurrent(f,std::uint64_t(GetTickCount64()))||
       !TrackingHandUsable111(side)||!PropHandFree596(side)||!ConstraintFree(side,h.id)||
       kcd2vr::mutual_admission_t25::pickupClaimsD72[side].load(std::memory_order_acquire)!=h.claim){
        Release(side,now,"ACQUIRE_NOT_CONFIRMED");return false;
    }
    // R103 (plate too misaligned): the hand he SEES at the grab moment is the collision-solved hand (allowed),
    // which stops on the table top while the controller is already a few cm inside it. The old offset was
    // taken from the controller hand, so once lifted the object sat off by that gap. Keep the exact
    // visible-palm-to-object offset of the grab moment and drive from the same visible hand (CarryHandR103).
    const ph::Pose seen=CarryHandR103(f,side);
    h.offset=ph::compose(ph::inverse(seen),object);h.start=now;h.character=f.stamp.actor;h.epoch=f.stamp.epoch;
    h.tracking=f.doorTrackingEpoch66;h.request=t.request;h.drive.seed(object,f.stamp.sample,f.stamp.tick);
    g_held[side]=h;Publish(side,h,f,object,std::uint64_t(GetTickCount64()));
    Row("HELD",side,h,swapped?"LOOSE_SCENERY_COPY_R100":"NATIVE_RIGID_MASS_CONFIRMED",reach,pm::length(seen.p-hand.p)*100);
    ImpactEventsR116(h); // R116: falls/knocks of this prop reach the game's material effects (clink)
    // R103: the held prop no longer collides with Henry, his hand proxies or his weapons (restored after release)
    self_collision_r103n::Exclude(kcd2vr::self_collision_r103::propOwner(side),kcd2vr::self_collision_r103::Kind::Prop,h.id,
        reinterpret_cast<std::uintptr_t>(h.entity),reinterpret_cast<std::uintptr_t>(h.body),-1,now);
    G::heldEntityPtrR103[side].store(h.entity,std::memory_order_release);G::heldBodyR103[side].store(h.body,std::memory_order_release);
    (void)kcd2vr::haptics101::publish_event(side,h.claim,.35f,35,now);
    return true;
}
// R100: a load (loading screen) brings the level's own brushes back. Restore any of our
// suspended brushes that still exist exactly, remove leftover loose copies, clear the table.
static void SwapLoadR100() noexcept {
    bool loading=false;const auto episode=SwapLoadEpisodeR100(loading);auto& T=g_swapTable;
    if(!T.known){T.known=true;T.episode=episode;return;}
    if(!loading&&episode==T.episode)return;
    T.episode=episode;if(!T.n)return;
    for(int side=0;side<2;++side)if(g_held[side].id&&g_held[side].swapR100)Release(side,std::uint64_t(GetTickCount64()),"LOAD_RESTORES_LEVEL_BRUSHES");
    unsigned restored=0;const unsigned n=T.n;
    for(unsigned i=0;i<n;++i)if(SwapRestoreEntryR100(T.e[i]))++restored;
    T.clear();g_impactR124.clear(); // R124: the loose copies are removed, nothing left to listen to
    (void)Lua("if KCD2VR_Lab and KCD2VR_Lab.SwapClearR100 then KCD2VR_Lab.SwapClearR100() end");
    char row[160]{};std::snprintf(row,sizeof(row),"R100 BRUSH_SWAP load: table cleared entries=%u brushesRestored=%u loading=%d",n,restored,loading?1:0);
    diagnostic_trace::Write(row);
}
static void Carry(int side,std::uint64_t now,const CommittedFrame6695* f,float grip) noexcept {
    auto& h=g_held[side];
    if(!(h.swapR100?B::enabled:G::enabled).load(std::memory_order_acquire)){Release(side,now,"SWITCHED_OFF_MOMENTUM_RETAINED");return;}
    if(G::pickupYield(side,now)&&now>=h.start&&now-h.start<=G::kPickupYieldMs){Release(side,now,"NATIVE_PICKUP_SAME_SQUEEZE");return;}
    if(!f||!FrameCurrent(*f,now)||!f->valid[side]||!f->propRequestedValid5916c1[side]||!TrackingHandUsable111(side)||
       f->stamp.actor!=h.character||f->stamp.epoch!=h.epoch||f->doorTrackingEpoch66!=h.tracking||
       h.request!=g_contact_control_epoch6695.load(std::memory_order_acquire)||!PropHandFree596(side)||!ConstraintFree(side,h.id)||
       kcd2vr::mutual_admission_t25::pickupClaimsD72[side].load(std::memory_order_acquire)!=h.claim){Release(side,now,"SOURCE_OR_OWNER_ENDED");return;}
    if(!std::isfinite(grip)||grip<G::kGripOpen){
        // R116: with the firm grip the prop leaves with the hand's own velocity (throw), not with a gap correction
        ph::Pose was{};prop5916::Dynamics wd{};pm::Vec3 lin{},ang{};
        if(h.firmActive&&h.firm.primed&&Current(h,was,wd)){h.firm.release(lin,ang);(void)PropSetVelocity5916C1(h.body,lin,ang);}
        Release(side,now,h.firmActive?"GRIP_OPEN_HAND_VELOCITY_R116":"GRIP_OPEN_NATIVE_MOMENTUM_RETAINED");return;
    }
    ph::Pose actual{};prop5916::Dynamics d{};
    if(!Current(h,actual,d)){Release(side,now,"NATIVE_BODY_REPLACED_OR_UNAVAILABLE");return;}
    const auto target=ph::compose(CarryHandR103(*f,side),h.offset);
    auto staged=h.drive;const auto at=std::uint64_t(GetTickCount64());
    // R116 firm grip: every game frame, hand velocity + half the gap, no force/mass budget (prop_drive5916c1.h).
    // OFF = the R103 servo below (commands only when the coarse tick advanced, 80 N / 300 rad/s^2 budgets).
    const bool firm=prop5916::firmHoldR116.load(std::memory_order_acquire);
    auto firmStaged=h.firm;prop5916::Command command{};
    if(firm){
        const double t=NowSecondsR116();
        const double poseTime=f->handTimeNsR118>0?double(f->handTimeNsR118)*1e-9:0.; // R118: the pose's own clock
        if(!h.firmActive||!firmStaged.primed)firmStaged.seed(target,f->stamp.sample,t,poseTime);
        command=firmStaged.update(target,actual,d,f->stamp.sample,t,poseTime);
    }else{
        if(h.firmActive){staged.seed(target,f->stamp.sample,f->stamp.tick);h.drive=staged;h.firmActive=false;} // switched OFF mid-hold
        command=staged.update(target,actual,d,f->stamp.sample,f->stamp.tick,at,CarryLimitsR103());
    }
    self_collision_r103n::Exclude(kcd2vr::self_collision_r103::propOwner(side),kcd2vr::self_collision_r103::Kind::Prop,h.id,
        reinterpret_cast<std::uintptr_t>(h.entity),reinterpret_cast<std::uintptr_t>(h.body),-1,at); // re-applies only if still missing (250 ms)
    if(command.decision==prop5916::Decision::End){Release(side,at,"DRIVE_CONTINUITY_OR_SEPARATION");return;}
    if(command.decision==prop5916::Decision::Drive){
        if(!PropSetVelocity5916C1(h.body,command.linear,command.angular)){Release(side,at,"NATIVE_VELOCITY_REFUSED");return;}
        if(firm){h.firm=firmStaged;h.firmActive=true;}else h.drive=staged;
    }
    Publish(side,h,*f,actual,at); // achieved pose, never the unsimulated target
    if(Capture(at)&&(!h.reportTick||at<h.reportTick||at-h.reportTick>=200)){
        Row("DRIVE",side,h,command.decision==prop5916::Decision::Duplicate?"DUPLICATE_SOURCE_NO_ACTION":firm?"FIRM_HOLD_R116":"ACTION_SUBMITTED",command.separation,
            pm::angle(target.q,actual.q)*57.29577951308232);
        h.reportTick=at;
    }
}
static bool HandContact(const tr::Evidence& e,const CommittedFrame6695& f,std::uint64_t now) noexcept {
    const auto& s=e.sample;const int side=s.side;
    return side>=0&&side<2&&FrameCurrent(f,now)&&s.source==pc::Source::HandWorld&&s.status==pc::Status::Contact&&
        s.eligible&&s.worldCoherent&&s.actor==f.stamp.actor&&s.epoch==f.stamp.epoch&&s.sequence==f.stamp.sample&&s.tick==f.stamp.tick&&
        s.request==g_contact_control_epoch6695.load(std::memory_order_acquire)&&e.publicationR24==f.publicationR24&&f.publicationR24&&
        f.valid[side]&&f.propRequestedValid5916c1[side]&&e.entity&&e.owner&&e.ownerRead&&e.collider&&
        (e.body==nc::Body::Physical||e.body==nc::Body::Rigid)&&pm::finite(s.point)&&bool(pm::unit(s.normal));
}
static void React(std::uint32_t id,std::uintptr_t owner,std::uintptr_t collider,int side,
                  pm::Vec3 point,pm::Vec3 normal,double speed,std::uint64_t now,const CommittedFrame6695& f) noexcept {
    if(!id||!collider||!FrameCurrent(f,now)||!std::isfinite(speed)||speed<.10||speed>20||
       !pm::finite(point)||!pm::unit(normal)||G::ownsEntityR94(id)||HeldInteractionVisual43()||
       g_contactBudgetR94.refuse(id,now))return;
    Held h{};h.id=id;h.entity=reinterpret_cast<void*>(owner);ph::Pose object{};
    if(!Inspect(h,true,object)||reinterpret_cast<std::uintptr_t>(h.body)!=collider||pm::length(point-object.p)>.65)return;
    const bool pickable=!std::strcmp(h.cls,"PickableItem");
    // F9's existing dynamic response stays the single writer while selected.
    // Static scenery still needs promotion; the old rigid-only lane refuses it.
    if(h.wasRigid&&kcd2vr::collab_h21::enabled.load(std::memory_order_acquire))return;
    if(!g_contactBudgetR94.admit(id,now))return;
    if(!pickable&&!Prepare(h,object))return;
    ph::Pose actual{};prop5916::Dynamics d{};
    if(!Current(h,actual,d)||d.mass>G::kMaxMass||!FrameCurrent(f,std::uint64_t(GetTickCount64())))return;
    const double impulse=G::contactImpulseR94(speed,d.mass);if(!(impulse>0))return;
    const auto direction=*pm::unit(normal)*-1.;
    char lua[640]{};const int n=std::snprintf(lua,sizeof(lua),
        "assert(KCD2VR_Lab and KCD2VR_Lab.PushPropR94 and KCD2VR_Lab.PushPropR94(%u,'%s',{x=%.9g,y=%.9g,z=%.9g},{x=%.9g,y=%.9g,z=%.9g},%.9g),'KCD2VR prop contact refused')",
        h.id,h.cls,point.x,point.y,point.z,direction.x,direction.y,direction.z,impulse);
    if(n<=0||std::size_t(n)>=sizeof(lua))return;
    const bool submitted=Lua(lua);
    Row("CONTACT",side,h,submitted?"NATIVE_POINT_IMPULSE_SUBMITTED":"POINT_IMPULSE_REFUSED",impulse);
    // R124: a knocked loose copy / made-rigid static prop sounds when it lands (rigid level props keep the game's own)
    if(submitted&&(g_swapTable.hasEntity(id)||!h.wasRigid))WatchImpactR124(id,ImpactKindR124(h),now);
    if(!pickable&&submitted){auto& t=g_touch[side];
        if(t.entity==id&&t.owner==reinterpret_cast<std::uintptr_t>(h.entity)){
            t.collider=reinterpret_cast<std::uintptr_t>(h.body);t.rigid=true;
        }
    }
}
static void WeaponContacts(std::uint64_t now,const CommittedFrame6695& f) noexcept {
    PhysFrame6698 pf{};if(!g_phys_frame6698.read(pf)||!pf.input.allowed||pf.characterT23!=f.stamp.actor||
       pf.input.bodyEpoch!=f.stamp.epoch||pf.trackingEpochT23!=f.doorTrackingEpoch66||
       pf.publicationT23!=f.publicationR24||pf.input.sample!=f.stamp.sample||
       pf.input.epoch!=g_contact_control_epoch6695.load(std::memory_order_acquire))return;
    for(int side=0;side<2;++side){const auto& r=pf.output.impactRecord178r30[side];const auto& p=pf.profiles[side];
        if(!r.valid||!r.entityId||!r.collider||!pf.input.tracked[side]||!pf.output.active[side]||
           !p.key||r.key!=p.key||r.generation!=p.generation||r.epoch!=f.stamp.epoch||r.request!=pf.input.epoch||
           !G::recent(now,r.tick,80)||(r.body!=int(nc::Body::Physical)&&r.body!=int(nc::Body::Rigid))||
           !g_impactSeenR94[side].take(r))continue;
        auto collider=r.collider;
        if(r.body==int(nc::Body::Physical)){
            // Only actual PickableItem preparation succeeds here. Scenery is
            // admitted by React/Prepare with its own unchanged-slot proof.
            void* entity=EntityById6694(r.entityId);char cls[64]{};
            if(entity&&EntityClassR89(entity,cls,sizeof(cls))&&!std::strcmp(cls,"PickableItem")){
                void* body=nullptr;if(!PrepareWeaponContactR94(pf,side,body))continue;
                collider=reinterpret_cast<std::uintptr_t>(body);
            }
        }
        React(r.entityId,0,collider,side,r.point,r.normal,r.speed,now,f);
    }
}
// R158 (instant_drop_r158.h): an instant-held pickup item let go WITHOUT the chest stow falls like a scenery prop.
// once per drop, on this native rhythm (game thread, where the scenery Lua calls run): detach it from the parent the
// native pickup gave it (only when one is read), wake its body, give it the hand velocity again. one log line.
namespace ID=kcd2vr::instant_drop_r158;
static bool EntityParentR158(void* entity,std::uintptr_t& parent) noexcept {
    // the audited GetParent 83A8F4 layout (Body() above, saved_corpse_drag_d72.inl SavedOwnerD72): +0x98 link, +0x18 parent
    parent=0;std::uintptr_t link=0;
    return entity&&Read(static_cast<unsigned char*>(entity)+0x98,link)&&(!link||Read(reinterpret_cast<void*>(link+0x18),parent));
}
static void InstantDropR158(std::uint64_t now) noexcept {
    ID::Request r{};bool due=false;
    {PropLock596 lock;if(!lock.owned)return; // busy: the next frame takes it (the request is age-bounded)
     due=g_instant_drop_r158.take(now,r);
     if(due&&g_prop596.id==r.id)due=false;} // squeezed again already: it is held, not dropped
    if(!due||!r.id)return;
    void* e=EntityById6694(r.id);const char* result="DONE";
    bool parentSeen=false,detached=false,posePut=false,awake=false,velocity=false;
    if(!e||reinterpret_cast<std::uintptr_t>(e)!=r.entity||!PhysVft6698(e,kCEntityVftRva)||G::ownsEntityR94(r.id))result="ENTITY_GONE_OR_HELD";
    else{
        std::uintptr_t parent=0;parentSeen=EntityParentR158(e,parent)&&parent;
        Matrix34V6550 tm{};ph::Pose before{};float scale[3]{};
        const bool poseBefore=GetEntityWorldTmV6552(e,r.id,tm)&&PoseOf(tm,before,scale);
        if(parentSeen){
            char lua[320]{};const int n=ID::detachChunk(lua,sizeof(lua),r.id);
            if(n>0&&std::size_t(n)<sizeof(lua))(void)Lua(lua);
            std::uintptr_t after=1;detached=EntityById6694(r.id)==e&&EntityParentR158(e,after)&&!after;
            if(!detached)result="DETACH_REFUSED_WAKE_ONLY";
            else if(poseBefore){ // keep it where he let go: no parent now, so the LOCAL setter is the world pose
                Matrix34V6550 t2{};ph::Pose got{};float s2[3]{};
                if(!(GetEntityWorldTmV6552(e,r.id,t2)&&PoseOf(t2,got,s2)&&kcd2vr::prop_parent_r153::Matches(got,before))){
                    std::uintptr_t vf=0,fn=0;
                    posePut=Read(e,vf)&&vf==WhBase()+kCEntityVftRva&&Read(reinterpret_cast<void*>(vf+0x168),fn)&&
                        fn==WhBase()+0x509810&&PropSetLocal596(fn,e,before,scale);
                }
            }
        }
        char lua[320]{};const int n=ID::wakeChunk(lua,sizeof(lua),r.id);
        awake=n>0&&std::size_t(n)<sizeof(lua)&&EntityById6694(r.id)==e&&Lua(lua);
        if(!awake&&!std::strcmp(result,"DONE"))result="WAKE_REFUSED";
        if(ID::rewriteVelocity(r)&&EntityById6694(r.id)==e)velocity=PropSetVelocity5916C1(PropBody596(e),{r.v[0],r.v[1],r.v[2]});
    }
    char row[300]{};std::snprintf(row,sizeof(row),
        "R158 INSTANT_DROP id=%u parent=%d detached=%d posePut=%d awake=%d handSpeed=%.2f velocity=%d result=%s",
        r.id,parentSeen?1:0,detached?1:0,posePut?1:0,awake?1:0,ID::speed(r),velocity?1:0,result);
    diagnostic_trace::Write(row);
}
static void Tick(std::uint64_t now) noexcept {
    self_collision_r103n::Tick(now); // R103: give Henry's collision back to released props/corpses
    SwapLoadR100(); // R100: after a load the level's brushes are back; clear the swap table
    ImpactSoundsR124(now); // R124: released/knocked props of ours play their put-down sound when they hit something
    InstantDropR158(now); // R158: a dropped (not stowed) instant pickup item falls (any switch state: the hold ran)
    const bool on=G::enabled.load(std::memory_order_acquire),swap=B::enabled.load(std::memory_order_acquire),any=on||swap;
    if(!any&&!g_held[0].id&&!g_held[1].id){g_note[0].pending=g_note[1].pending=false;g_contactMotionR94[0]={};g_contactMotionR94[1]={};return;}
    CommittedFrame6695 f{};const bool haveFrame=ReadCommittedPacket6695(f)&&FrameCurrent(f,now);
    for(int side=0;side<2;++side){
        const float grip=haveFrame&&f.mapFoldTrackedM39?f.mapGripM39[side]:std::numeric_limits<float>::quiet_NaN();
        const bool closed=g_edge[side].close(grip);
        if(g_held[side].id){Carry(side,now,haveFrame?&f:nullptr,grip);continue;}
        tr::Evidence e{};pc::Decision motion{};bool contact=false;
        if(any&&haveFrame&&tr::mailbox[side].read(e)){
            motion=pc::update(g_contactMotionR94[side],e.sample);contact=HandContact(e,f,now);
            // entity touches need the old switch; our own loose copies follow the swap switch
            if(contact&&!on&&!(swap&&g_swapTable.hasEntity(e.entity)))contact=false;
            if(contact){auto& t=g_touch[side];t.entity=e.entity;t.owner=e.owner;t.collider=e.collider;
                t.actor=e.sample.actor;t.epoch=e.sample.epoch;t.request=e.sample.request;t.tick=e.sample.tick;t.rigid=e.body==nc::Body::Rigid;
            }else if(e.sample.status==pc::Status::Contact&&!e.entity&&e.body==nc::Body::Physical&&G::recent(now,e.sample.tick,80))
                g_touch[side].worldTick=e.sample.tick;
        }
        if(any&&closed&&haveFrame&&f.valid[side]&&f.pickupTriggerD80[side]<=.55f&&TrackingHandUsable111(side))TryGrab(side,now,f);
        if(on&&contact&&!g_held[side].id&&HandFree(side,now)&&motion.motion.valid)
            React(e.entity,e.owner,e.collider,side,e.sample.point,e.sample.normal,motion.motion.incoming,now,f);
    }
    if(on&&haveFrame)WeaponContacts(now,f);
    FlushNotes(now);
}
} // namespace static_grab_r89n
