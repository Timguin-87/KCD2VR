// A2 SMITHING ANVIL CONTACT (Miki 27 Oct: "only the anvil needs to be collided with - iron and hammer").
// Optional approximate learned anvil. Existing J27 world contact remains unchanged when this experiment is off.
// The anvil itself is learned in subsystems/smithing/anvil_a2.h from the native StrokeHit points of THIS minigame
// visit and locked (stable). Here, per hand, on the pose owner:
//  - hammer hand: the hammer's own native local bounds (3 probes along its long axis);
//  - tongs hand: the workpiece (iron) in the tongs, its native local bounds placed by the CURRENT native iron
//    transform relative to the tongs (so the iron, not the tongs, meets the anvil).
// The probes are expressed in the WRIST frame (wrist * grip * tool), and the solve runs from the last allowed wrist
// to the current desired wrist, translation and rotation together (anvil_a2::solve). The result changes the goal
// (translation) and final model-space wrist quaternion (D52), so the arm solver places the wrist - and therefore
// the tool - at the allowed transform. Legacy callers may still request the old visual-XR conversion. Native forge strokes use the raw tracked controller (SemiSmithInput46 /
// WorkstationSample37c4), never this visual wrist, so a hammer resting on the anvil still counts its blow.
// Contact/recovery history is transaction-owned; native anvil observation is
// independent of whether a particular displayed wrist packet was published.
static kcd2vr::subsystems::smithing::anvil_a2::Anvil g_anvil_a2{};  // pose owner only
static std::uint64_t g_anvil_a2_waitSince=0,g_anvil_a2_noticeEpoch=0;
static std::atomic<std::uint64_t> g_smith_anvil_blocks_a2{0};
#include "smith_native_floor_d56.inl"
static Q4 CryToXrQA2(Q4 q) noexcept { const Q4 basis{0.70710678118f,0,0,0.70710678118f}; return QNormalize(QMul(QMul(QConj(basis),QNormalize(q)),basis)); }
namespace anvil_a2_adapter {
namespace av=kcd2vr::subsystems::smithing::anvil_a2;
inline av::D3 D(V3 v) noexcept {return {v.x,v.y,v.z};}
inline V3 F(av::D3 v) noexcept {return {float(v.x),float(v.y),float(v.z)};}
inline av::DQ DQ_(Q4 q) noexcept {return {q.x,q.y,q.z,q.w};}
inline Q4 FQ(av::DQ q) noexcept {return QNormalize(Q4{float(q.x),float(q.y),float(q.z),float(q.w)});}
}
// the iron in the tongs: current native workpiece of this smith model (model+2F8 holder, +10 entity id)
static bool SmithIronA2(const kcd2vr::smith_tool36::Binding& b,std::uint32_t& id,float (&world)[12]) noexcept {
    id=0;const auto base=WhBase();if(!b.model||!retail_build112::Ready(base))return false;
    std::uintptr_t holder=0,vft=0;std::uint32_t eid=0;
    if(!Read(reinterpret_cast<void*>(b.model+kcd2vr::smith_tool36::kWorkpieceSlot),holder)||!holder||
       !Read(reinterpret_cast<void*>(holder),vft)||vft!=base+kcd2vr::smith_tool36::kWorkpieceVft||
       !Read(reinterpret_cast<void*>(holder+0x10),eid)||!eid)return false;
    void* e=EntityById6694(eid);if(!e||!PhysVft6698(e,0x4072C78)||!Read(static_cast<unsigned char*>(e)+0x58,world))return false;
    for(float v:world)if(!std::isfinite(v)||std::fabs(v)>1e6f)return false;
    if(EntityById6694(eid)!=e)return false;id=eid;return true;
}
// D78R1: a forge workpiece may remain alive after native Detach. Only the
// exact currently attached nested Assetslot may supply held-tongs collision.
// Keep SmithIronA2 itself for the independent hammer/anvil thickness witness.
static bool SmithHeldIronA2(const kcd2vr::smith_tool36::Binding& b,std::uint32_t& id,float (&world)[12]) noexcept {
    namespace st=kcd2vr::smith_tool36;
    id=0;st::Binding live{},after{};SmithWorkpieceD56 piece{},again{};
    if(b.tool!=int(st::Tool::Tongs)||
       !ReadSmithTool36(reinterpret_cast<void*>(b.attachment),b.entity,b.character,live)||!st::sameGripIdentity67(b,live)||
       !SmithWorkpieceD56Read(live,piece))return false;
    std::uint32_t observed=0;
    if(!SmithIronA2(live,observed,world)||observed!=piece.entity||
       !ReadSmithTool36(reinterpret_cast<void*>(live.attachment),live.entity,live.character,after)||!st::same(live,after)||
       !SmithWorkpieceD56Read(after,again)||again.holder!=piece.holder||again.entity!=piece.entity||
       again.attachment!=piece.attachment||again.object!=piece.object||
       std::memcmp(&again.inTool,&piece.inTool,sizeof(piece.inTool)))return false;
    id=observed;return true;
}
template<class CoherentStateA2> static void SmithAnvilContactA2(void* character,std::uint64_t epoch,int side,const CoherentStateA2& s,Q4 frame,Q4& xrQ,V3& goal,
    SmithContactHistoryR38* history=nullptr,Q4* modelOrientationD52=nullptr) noexcept {
    namespace ws=kcd2vr::station_ws_j27;namespace av=kcd2vr::subsystems::smithing::anvil_a2;using namespace anvil_a2_adapter;
    if(side<0||side>1)return;
    auto& st=history?history->anvil[side]:g_smith_anvil_contact_a2[side];
    const int contactMode=av::mode.load(std::memory_order_acquire);
    const auto modeRevision=av::modeRevisionD39.load(std::memory_order_acquire);
    const bool experiment=contactMode!=int(av::Mode::Off)&&av::enabled.load(std::memory_order_acquire);
    const bool modeRetired=av::syncModeD39(g_anvil_a2,contactMode,modeRevision);
    const auto now=std::uint64_t(GetTickCount64());const bool capture=ws::captureD47(now);
    if(modeRetired){g_anvil_a2_waitSince=0;g_anvil_a2_noticeEpoch=0;
        if(capture)diagnostic_trace::Write("WS ANVIL_RETIRE reason=LIVE_SOURCE_MODE_CHANGED oldGeometry=REVOKED");}
    // D52: the live caller supplies the final requested wrist after native
    // fixture guidance. Keep the accepted model-space quaternion paired with
    // its contact position; do not feed it back into tracked controller input.
    const Q4 desiredQ=modelOrientationD52?*modelOrientationD52:DesiredHand6689(s,side,frame,xrQ);
    const auto applyRotation=[&](Q4 allowedModel){
        if(modelOrientationD52){*modelOrientationD52=allowedModel;return;}
        const Q4 delta=QNormalize(QMul(allowedModel,QConj(desiredQ)));
        const Q4 controller=QMul(frame,XrToCryQ(xrQ));
        xrQ=CryToXrQA2(QMul(QConj(frame),QMul(delta,controller)));
    };
    const auto worldFallback=[&](){Q4 allowed=desiredQ;
        SmithToolContactJ27(character,epoch,side,desiredQ,goal,&allowed,history?history->world:nullptr);applyRotation(allowed);};
    if(!experiment&&!st.corrected){st={};worldFallback();return;}
    // bounded return if a correction was active and this frame cannot solve (off, anvil lost, socket gone)
    const auto recoverOrForget=[&](){
        if(!st.corrected||!st.have){st.have=false;st.corrected=false;return;}
        bool done=false;const auto p=av::recover({D(st.lastGoal),DQ_(st.lastQ)},{D(goal),DQ_(desiredQ)},done);
        goal=F(p.p);const Q4 q=FQ(p.q);applyRotation(q);st.lastGoal=goal;st.lastQ=q;if(done){st.corrected=false;st.have=false;}
    };
    using namespace kcd2vr::activity_input163;
    const auto life=witness175.lifetime175.load(std::memory_order_acquire);
    const auto smithEpoch=witness175.smithEpochC1.load(std::memory_order_acquire);
    const auto instance=witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire);
    const bool lifecycle=witness175.ready.load(std::memory_order_acquire)&&!witness175.transitionDepth175.load(std::memory_order_acquire)&&instance;
    ToolSocket59c10 socket{};
    const bool socketLive=g_tool_socket59c10[side].read(socket)&&bool(socket.smith36)&&socket.side==side&&
        socket.character==character&&socket.epoch==epoch&&socket.tick&&now>=socket.tick&&now-socket.tick<=80&&socket.entity&&
        socket.smith36.instance==instance&&socket.smith36.lifetime==life&&
        socket.request==kcd2vr::workstation178r20c1::smithHoldEpoch36.load(std::memory_order_acquire)&&
        socket.modeEpoch67==kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire);
    const bool wristValid=std::isfinite(goal.x)&&std::isfinite(goal.y)&&std::isfinite(goal.z)&&
        std::isfinite(desiredQ.x)&&std::isfinite(desiredQ.y)&&std::isfinite(desiredQ.z)&&std::isfinite(desiredQ.w);
    if(!lifecycle||!socketLive||!wristValid||SmithAutoHoldReason36(now)!=nullptr){st={};return;} // owner loss yields immediately
    const bool sameOwner=st.have&&st.owner.character==character&&st.epoch==epoch&&st.owner.id==socket.id&&
        st.owner.request==socket.request&&st.owner.modeEpoch67==socket.modeEpoch67&&st.smithEpoch==smithEpoch&&
        st.lastTick&&now>=st.lastTick&&now-st.lastTick<=80&&
        kcd2vr::smith_tool36::sameGripIdentity67(st.owner.smith36,socket.smith36);
    if(st.have&&!sameOwner)st={}; // never recover an old item's or old body's wrist
    CommittedFrame6695 prev{};
    const bool sameTool=g_committed_frame6695.read(prev)&&prev.valid[side]&&prev.tools59c10[side].valid&&
        prev.stamp.actor==reinterpret_cast<std::uintptr_t>(character)&&prev.stamp.epoch==epoch&&
        prev.stamp.tick&&now>=prev.stamp.tick&&now-prev.stamp.tick<=80&&
        prev.tools59c10[side].socket.id==socket.id&&prev.tools59c10[side].socket.request==socket.request&&
        prev.tools59c10[side].socket.modeEpoch67==socket.modeEpoch67&&
        kcd2vr::smith_tool36::sameGripIdentity67(prev.tools59c10[side].socket.smith36,socket.smith36);
    if(!sameTool){st={};return;}
    st.lastTick=now;
    const bool tongs=socket.smith36.tool==int(kcd2vr::smith_tool36::Tool::Tongs);
    std::uint32_t ironId=0;float ironWorld[12]{};bool iron=false;
    if(tongs){
        iron=SmithHeldIronA2(socket.smith36,ironId,ironWorld);
        // Native Detach ends the proxy before any recovery of its old pose,
        // including a simultaneous switch of collision or A2 mode to OFF.
        if(!iron){st={};(history?history->world[side]:g_smith_tool_contact_j27[side])={};return;}
    }
    if(!experiment||!ws::toolCollision.load(std::memory_order_relaxed)){
        if(sameOwner&&st.corrected)recoverOrForget();else st={};
        if(!st.corrected)worldFallback();
        return;
    }
    std::uintptr_t henry=0;
    const av::Identity current{instance,(socket.smith36.player&&Read(reinterpret_cast<void*>(socket.smith36.player+0x38),henry))?henry:0,life,smithEpoch};
    float root[12]{};av::D3 rootPos{},rootFwd{};
    if(!bool(current)||!Read(reinterpret_cast<void*>(henry+0x58),root)||!av::rootFrom(root,rootPos,rootFwd)){st={};return;}
    st.owner=socket;st.smithEpoch=smithEpoch;
    auto& attempts=SmithAttemptsForOwnerR38(g_smith_anvil_attempts_r38[side],socket);
    // --- tool probes in the tool frame (hammer bounds, or the iron held by the tongs)
    if((st.boundsId!=socket.id||!st.boundsOk)&&SmithBoundsRetryDueR38(socket.id,now,attempts.boundsId,attempts.boundsFailedAt)){
        st.boundsId=socket.id;st.boundsOk=PhysBounds6698(socket.entity,st.bounds);
        attempts.boundsId=socket.id;attempts.boundsFailedAt=st.boundsOk?0:now;}
    // The hammer's independent native thickness witness remains available
    // whether or not that workpiece is currently nested in the tongs.
    if(!tongs)iron=SmithIronA2(socket.smith36,ironId,ironWorld);
    if(iron&&st.ironId!=ironId){st.ironId=ironId;st.ironOk=false;}
    if(iron&&!st.ironOk&&SmithBoundsRetryDueR38(ironId,now,attempts.ironId,attempts.ironFailedAt)){
        st.ironId=ironId;st.ironOk=PhysBounds6698(EntityById6694(ironId),st.ironBounds);
        attempts.ironId=ironId;attempts.ironFailedAt=st.ironOk?0:now;}
    double half=-1;
    if(iron&&st.ironOk){double e[3];for(int k=0;k<3;++k)e[k]=double(st.ironBounds[k+3])-st.ironBounds[k];half=.5*std::min(e[0],std::min(e[1],e[2]));}
    // --- learn / validate the anvil from the coherent native blow ring
    {
        const auto r1=av::validate(g_anvil_a2,current,epoch,rootPos,rootFwd);
        av::Ring ring{};const bool wasLocked=g_anvil_a2.locked;av::Retire r2=av::Retire::None;
        if(av::published.load(std::memory_order_acquire)>g_anvil_a2.lastSerial&&av::mailbox.read(ring))r2=av::integrate(g_anvil_a2,ring,current,epoch,half,contactMode==int(av::Mode::Real),modeRevision);
        const auto r3=av::validate(g_anvil_a2,current,epoch,rootPos,rootFwd); // queued old-root blows cannot earn current geometry
        const auto why=r1!=av::Retire::None?r1:r2!=av::Retire::None?r2:r3;
        if(capture&&why!=av::Retire::None){char m[160]{};std::snprintf(m,sizeof(m),"WS ANVIL_RETIRE reason=%s instance=%p",av::retireName(why),reinterpret_cast<void*>(instance));diagnostic_trace::Write(m);}
        if(capture&&g_anvil_a2.locked&&!wasLocked&&g_anvil_a2.box){char m[320]{};const auto& a=g_anvil_a2; // A3 REAL
            std::snprintf(m,sizeof(m),"WS ANVIL_LOCKED source=PHYSICS entity=%p instance=%p life=%llu epoch=%llu min(%.3f,%.3f,%.3f) max(%.3f,%.3f,%.3f)",
                reinterpret_cast<void*>(a.phys),reinterpret_cast<void*>(instance),(unsigned long long)a.id.lifetime,(unsigned long long)a.id.smithEpoch,
                a.boxMin.x,a.boxMin.y,a.boxMin.z,a.boxMax.x,a.boxMax.y,a.top);
            diagnostic_trace::Write(m);}
        else if(capture&&g_anvil_a2.locked&&!wasLocked){char m[320]{};const auto& a=g_anvil_a2;
            std::snprintf(m,sizeof(m),"WS ANVIL source=NATIVE_STROKEHIT_LOCKED instance=%p life=%llu epoch=%llu blows=%u box=center(%.3f,%.3f) top=%.3f radius=%.3f depth=%.2f ironHalf=%.3f",
                reinterpret_cast<void*>(instance),(unsigned long long)a.id.lifetime,(unsigned long long)a.id.smithEpoch,a.blows,a.center.x,a.center.y,a.top,a.radius,av::kDepth,a.half);
            diagnostic_trace::Write(m);}
        if(!g_anvil_a2.locked){
            if(g_anvil_a2_noticeEpoch!=current.smithEpoch){g_anvil_a2_noticeEpoch=current.smithEpoch;g_anvil_a2_waitSince=now;}
            else if(!av::physicsMode()&&g_anvil_a2_waitSince&&now-g_anvil_a2_waitSince>8000){g_anvil_a2_waitSince=0; // REAL: the publisher says if not found
                (void)kcd2vr::visible_failure::publish("smithing: anvil collision waits for 3 hammer blows on the anvil",now);}
            // Real anvil discovery starts at a native blow. Until its current
            // box exists, keep the bounded real world tool sweep available;
            // selecting REAL must not disable all contact before the first hit.
            recoverOrForget();if(!st.corrected)worldFallback();return;
        }
    }
    const auto probe=!tongs?(st.boundsOk?ws::probes(st.bounds):ws::Probes{}):(iron&&st.ironOk?ws::probes(st.ironBounds):ws::Probes{});
    if(!probe.valid){recoverOrForget();return;}
    if(!st.have||st.id!=socket.id||st.epoch!=epoch||Len(Sub(goal,st.lastGoal))>.5f){
        st.have=true;st.id=socket.id;st.epoch=epoch;st.lastGoal=goal;st.lastQ=desiredQ;st.corrected=false;return;}
    ModelWorld6692 x{};if(!ReadModelWorld6692(character,x)){st.lastGoal=goal;st.lastQ=desiredQ;return;}
    if(tongs){
        std::uint32_t afterId=0;float afterWorld[12]{};
        if(!SmithHeldIronA2(socket.smith36,afterId,afterWorld)||afterId!=ironId||
           std::memcmp(afterWorld,ironWorld,sizeof(ironWorld))){st={};return;}
    }
    // grip: tool relative to the allowed wrist of the last committed frame; iron relative to the tool (native, now)
    const auto grip=RelativeFromAbsolute(prev.allowed[side],prev.tools59c10[side].model);
    QuatT28 toolInWrist=grip;
    if(tongs){
        const V3 ix{ironWorld[0],ironWorld[4],ironWorld[8]},iy{ironWorld[1],ironWorld[5],ironWorld[9]},iz{ironWorld[2],ironWorld[6],ironWorld[10]};
        const Q4 ironQw=QFromBasis(ix,iy,iz);const V3 ironPw{ironWorld[3],ironWorld[7],ironWorld[11]};
        QuatT28 ironModel{};QToQuatT(ironModel,QNormalize(QMul(QConj(x.q),ironQw)));ToT(ironModel,WorldToModel6692(x,ironPw));
        const auto ironInTool=RelativeFromAbsolute(prev.tools59c10[side].model,ironModel);
        toolInWrist=AbsoluteFromRelative(grip,ironInTool);
    }
    const Q4 tq=QFromQuatT(toolInWrist);const V3 tp=FromT(toolInWrist);
    av::Probe probes[3]{};
    for(int i=0;i<3;++i){const V3 local{float(probe.p[i][0]),float(probe.p[i][1]),float(probe.p[i][2])};
        probes[i]={D(Mul(Add(tp,QRotateV(tq,local)),x.scale)),probe.radius*x.scale};}
    const av::Pose last{D(ModelToWorld6692(x,st.lastGoal)),DQ_(QNormalize(QMul(x.q,st.lastQ)))};
    const av::Pose want{D(ModelToWorld6692(x,goal)),DQ_(QNormalize(QMul(x.q,desiredQ)))};
    const auto r=av::solve(g_anvil_a2,last,want,probes,3);
    if(r.corrected){
        goal=WorldToModel6692(x,F(r.pose.p));const Q4 qm=QNormalize(QMul(QConj(x.q),FQ(r.pose.q)));applyRotation(qm);
        st.corrected=true;st.lastGoal=goal;st.lastQ=qm;
        if(capture){g_smith_anvil_blocks_a2.fetch_add(1,std::memory_order_relaxed);
        if(now>=attempts.lastLog&&now-attempts.lastLog>=1000){attempts.lastLog=now;char m[240]{};
            std::snprintf(m,sizeof(m),"WS TOOL_CONTACT item=%s side=%d surface=ANVIL depthCm=%.1f rotDeg=%.1f fraction=%.2f slid=%d blocks=%llu",
                tongs?"IRON":ws::toolName(socket.smith36.tool),side,r.depth*100.,av::qangle(r.pose.q,want.q)*57.2957795,r.fraction,r.slid?1:0,
                (unsigned long long)g_smith_anvil_blocks_a2.load());
            diagnostic_trace::Write(m);}}
        return;
    }
    if(st.corrected&&!r.startInside){ // leaving a correction: bounded, never through the anvil
        bool done=false;auto p=av::recover(last,want,done);
        if(av::penetration(g_anvil_a2,p,probes,3)>av::kClear){ // first move over the top at the current height
            av::Pose over=want;over.p.z=std::max(want.p.z,last.p.z);bool ignore=false;
            p=av::recover(last,over,ignore);done=false;
            if(av::penetration(g_anvil_a2,p,probes,3)>av::kClear)p=last;
        }
        goal=WorldToModel6692(x,F(p.p));const Q4 qm=QNormalize(QMul(QConj(x.q),FQ(p.q)));applyRotation(qm);
        st.lastGoal=goal;st.lastQ=qm;if(done)st.corrected=false;return;
    }
    st.corrected=false;st.lastGoal=goal;st.lastQ=desiredQ;
}
