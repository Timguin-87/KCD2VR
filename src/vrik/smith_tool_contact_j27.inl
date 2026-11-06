// J27 SMITHING TOOL CONTACT (Miki 26 Oct: "the iron and the hammer have no collisions").
// the hand holding the controller-placed smith tool (hammer / tongs / workpiece) was never swept: a held item turns the
// free-hand contact off for that hand (contactYield bit 1) and nothing else checked the tool. now the tool's own native
// local bounds give 3 probe points (station_ws_j27.h probes), they move with the wrist from the last allowed pose to the
// requested one. D80 constrains only current measured inward contact, with a 20mm correction cap;
// tangential motion and requested rotation remain free (same world query and budget).
// hand and tool stop together because the placed tool is allowed wrist * grip. the native forge stroke is driven by
// the controller motion path, not by this visual wrist, so a hammer resting on the anvil still counts its blow.
// a busy/unknown query never freezes the hand: that frame just follows the controller.
#include "smith_contact_history_r38.h"
static std::atomic<std::uint64_t> g_smith_tool_blocks_j27{0};
// Defined by the adjacent anvil adapter; exact current native workpiece identity.
static bool SmithIronA2(const kcd2vr::smith_tool36::Binding&,std::uint32_t&,float (&)[12]) noexcept;
static bool SmithHeldIronA2(const kcd2vr::smith_tool36::Binding&,std::uint32_t&,float (&)[12]) noexcept;
static void SmithToolContactJ27(void* character,std::uint64_t epoch,int side,Q4 orientation,V3& goal,Q4* allowedOrientation=nullptr,
    SmithToolContactJ27State* history=nullptr) noexcept {
    namespace ws=kcd2vr::station_ws_j27;
    if(side<0||side>1)return;
    auto& st=history?history[side]:g_smith_tool_contact_j27[side];
    const auto now=std::uint64_t(GetTickCount64());
    // Fresh matching copied packets cannot extend a revoked native owner.
    // The A2-off fallback reaches this adapter before A2's lifecycle gate.
    using namespace kcd2vr::activity_input163;
    const auto life=witness175.lifetime175.load(std::memory_order_acquire);
    const auto instance=witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire);
    const bool lifecycle=witness175.ready.load(std::memory_order_acquire)&&
        !witness175.transitionDepth175.load(std::memory_order_acquire)&&instance;
    ToolSocket59c10 socket{};
    const bool socketLive=lifecycle&&g_tool_socket59c10[side].read(socket)&&bool(socket.smith36)&&socket.side==side&&
        socket.character==character&&socket.epoch==epoch&&socket.tick&&now>=socket.tick&&now-socket.tick<=80&&socket.entity&&
        socket.smith36.instance==instance&&socket.smith36.lifetime==life&&
        socket.request==kcd2vr::workstation178r20c1::smithHoldEpoch36.load(std::memory_order_acquire)&&
        socket.modeEpoch67==kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire);
    CommittedFrame6695 prev{};
    const bool sameTool=socketLive&&g_committed_frame6695.read(prev)&&prev.valid[side]&&prev.tools59c10[side].valid&&
        prev.tools59c10[side].socket.id==socket.id&&prev.stamp.actor==reinterpret_cast<std::uintptr_t>(character)&&
        prev.stamp.epoch==epoch&&prev.stamp.tick&&now>=prev.stamp.tick&&now-prev.stamp.tick<=80&&
        prev.tools59c10[side].socket.request==socket.request&&prev.tools59c10[side].socket.modeEpoch67==socket.modeEpoch67&&
        kcd2vr::smith_tool36::sameGripIdentity67(prev.tools59c10[side].socket.smith36,socket.smith36);
    const bool wristValid=std::isfinite(goal.x)&&std::isfinite(goal.y)&&std::isfinite(goal.z)&&
        std::isfinite(orientation.x)&&std::isfinite(orientation.y)&&std::isfinite(orientation.z)&&std::isfinite(orientation.w);
    if(!ws::admitTool(ws::toolCollision.load(std::memory_order_relaxed),SmithAutoHoldReason36(now)==nullptr,socketLive,sameTool,wristValid)){st={};return;}
    const bool sameOwner=st.id==socket.id&&st.epoch==epoch&&st.owner.character==character&&
        st.owner.request==socket.request&&st.owner.modeEpoch67==socket.modeEpoch67&&
        st.lastTick&&now>=st.lastTick&&now-st.lastTick<=80&&
        kcd2vr::smith_tool36::sameGripIdentity67(st.owner.smith36,socket.smith36);
    if(!sameOwner){st={};st.id=socket.id;st.epoch=epoch;}
    st.owner=socket;st.lastTick=now;
    // D60: native bounds/world queries can cross a station/attachment handover.
    // A fresh pre-query packet is not authority to clamp after that boundary.
    // Only copied lifecycle/socket joins run on clear frames; a native binding
    // re-read is reserved for an actual proposed collision correction below.
    const auto ownerCurrentD60=[&]() noexcept {
        const auto currentTick=std::uint64_t(GetTickCount64());ToolSocket59c10 current{};
        return ws::toolCollision.load(std::memory_order_acquire)&&!SmithAutoHoldReason36(currentTick)&&
            witness175.ready.load(std::memory_order_acquire)&&!witness175.transitionDepth175.load(std::memory_order_acquire)&&
            witness175.lifetime175.load(std::memory_order_acquire)==life&&
            witness175.active[unsigned(Kind175::Smithing)].load(std::memory_order_acquire)==instance&&
            socket.request==kcd2vr::workstation178r20c1::smithHoldEpoch36.load(std::memory_order_acquire)&&
            socket.modeEpoch67==kcd2vr::minigame_hands45::modeEpoch67.load(std::memory_order_acquire)&&
            g_tool_socket59c10[side].read(current)&&current.side==side&&current.character==character&&
            current.epoch==epoch&&current.entity==socket.entity&&current.id==socket.id&&
            current.request==socket.request&&current.modeEpoch67==socket.modeEpoch67&&
            current.tick&&currentTick>=current.tick&&currentTick-current.tick<=80&&
            socket.tick&&currentTick>=socket.tick&&currentTick-socket.tick<=80&&
            prev.stamp.tick&&currentTick>=prev.stamp.tick&&currentTick-prev.stamp.tick<=80&&
            kcd2vr::smith_tool36::sameGripIdentity67(current.smith36,socket.smith36);
    };
    auto& attempts=SmithAttemptsForOwnerR38(g_smith_world_attempts_r38[side],socket);
    // Native bounds can be unavailable on the first attachment frame. A miss
    // is not a permanent "no collision" decision for this entire tool lifetime.
    if((st.boundsId!=socket.id||!st.boundsOk)&&SmithBoundsRetryDueR38(socket.id,now,attempts.boundsId,attempts.boundsFailedAt)){
        st.boundsId=socket.id;st.boundsOk=PhysBounds6698(socket.entity,st.bounds);
        attempts.boundsId=socket.id;attempts.boundsFailedAt=st.boundsOk?0:now;}

    const bool tongs=socket.smith36.tool==int(kcd2vr::smith_tool36::Tool::Tongs);
    std::uint32_t ironId=0;float ironWorld[12]{};
    const bool iron=tongs&&SmithHeldIronA2(socket.smith36,ironId,ironWorld);
    if(tongs&&!iron){st={};return;} // native Detach revokes the held proxy, never borrow the forge world position
    // A failed replacement read may have happened in a rejected pose. Retire
    // the old committed iron geometry before consulting the attempt throttle.
    if(iron&&st.ironId!=ironId){st.ironId=ironId;st.ironOk=false;}
    if(iron&&!st.ironOk&&SmithBoundsRetryDueR38(ironId,now,attempts.ironId,attempts.ironFailedAt)){
        st.ironId=ironId;st.ironOk=PhysBounds6698(EntityById6694(ironId),st.ironBounds);
        attempts.ironId=ironId;attempts.ironFailedAt=st.ironOk?0:now;}
    if(!ownerCurrentD60()){st={};return;}
    const auto probe=tongs?(iron&&st.ironOk?ws::probes(st.ironBounds):ws::Probes{}):
        (st.boundsOk?ws::probes(st.bounds):ws::Probes{});
    if(!probe.valid){st.have=false;return;}
    if(!st.have||st.id!=socket.id||st.epoch!=epoch||Len(Sub(goal,st.lastGoal))>.5f){
        st.have=true;st.id=socket.id;st.epoch=epoch;st.lastGoal=goal;st.lastQ=orientation;return;}
    ModelWorld6692 x{};if(!ReadModelWorld6692(character,x)){st.lastGoal=goal;st.lastQ=orientation;return;}
    auto grip=RelativeFromAbsolute(prev.allowed[side],prev.tools59c10[side].model);
    if(tongs){
        const V3 ix{ironWorld[0],ironWorld[4],ironWorld[8]},iy{ironWorld[1],ironWorld[5],ironWorld[9]},iz{ironWorld[2],ironWorld[6],ironWorld[10]};
        QuatT28 ironModel{};QToQuatT(ironModel,QNormalize(QMul(QConj(x.q),QFromBasis(ix,iy,iz))));
        ToT(ironModel,WorldToModel6692(x,{ironWorld[3],ironWorld[7],ironWorld[11]}));
        grip=AbsoluteFromRelative(grip,RelativeFromAbsolute(prev.tools59c10[side].model,ironModel));
    }
    QuatT28 wristA{},wristB{};QToQuatT(wristA,st.lastQ);ToT(wristA,st.lastGoal);QToQuatT(wristB,orientation);ToT(wristB,goal);
    const auto toolA=AbsoluteFromRelative(wristA,grip),toolB=AbsoluteFromRelative(wristB,grip);
    const Q4 qa=QFromQuatT(toolA),qb=QFromQuatT(toolB);
    struct PlaneD80 {V3 normal{};float penetration=0;};PlaneD80 planes[3]{};
    unsigned planeCount=0;int surface=-1;
    const auto dotD80=[](V3 a,V3 b){return a.x*b.x+a.y*b.y+a.z*b.z;};
    for(int i=0;i<3;++i){
        const V3 local{float(probe.p[i][0]),float(probe.p[i][1]),float(probe.p[i][2])};
        const V3 a=ModelToWorld6692(x,Add(FromT(toolA),QRotateV(qa,local))),b=ModelToWorld6692(x,Add(FromT(toolB),QRotateV(qb,local)));
        const V3 seg=Sub(b,a);const float len=Len(seg);
        if(!(len>1e-4f))continue;
        if(len>1.5f){st.lastGoal=goal;st.lastQ=orientation;return;}
        const auto h=WorldRayDistance6692(a,seg);
        if(h.state!=wc::Result::Clear&&h.state!=wc::Result::Hit){
            if(!ownerCurrentD60())st={};else{st.lastGoal=goal;st.lastQ=orientation;}
            return; // unknown is not clear: follow, no freeze
        }
        if(h.state!=wc::Result::Hit||!h.plane||!std::isfinite(h.distance)||h.distance<=.002f)continue;
        // Use the unflipped native front face. The ray-facing normal alone
        // also faces inward on an inside-to-outside exit and would oppose release.
        // Initial overlap, back face, tangency or an unqualified plane remains
        // permissive. This is current front-face evidence, not a solid-volume test.
        const V3 normal=h.nativeNormalD63;
        const float nn=float(Len(normal));
        const V3 expected=Add(a,Mul(seg,h.distance/len));
        if(!std::isfinite(nn)||nn<.99f||nn>1.01f||Len(Sub(h.point,expected))>.002f||
           dotD80(Sub(a,h.point),normal)<=.0005f||dotD80(seg,normal)>=-.0001f)continue;
        const float margin=float(std::min(.01,probe.radius*x.scale)+.002);
        const float penetration=margin-dotD80(Sub(b,h.point),normal);
        if(penetration>0&&std::isfinite(penetration)){
            planes[planeCount++]={normal,penetration};surface=h.native172.surfaceIdx;
        }
    }
    if(!ownerCurrentD60()){st={};return;}
    if(planeCount){
        kcd2vr::smith_tool36::Binding live{};
        if(!ReadSmithTool36(reinterpret_cast<void*>(socket.smith36.attachment),socket.id,reinterpret_cast<std::uintptr_t>(character),live)||
           !kcd2vr::smith_tool36::sameGripIdentity67(socket.smith36,live)){st={};return;}
        if(tongs){
            std::uint32_t afterId=0;float afterWorld[12]{};
            if(!SmithHeldIronA2(live,afterId,afterWorld)||afterId!=ironId){st={};return;}
            for(unsigned i=0;i<12;++i)if(afterWorld[i]!=ironWorld[i]){st={};return;}
        }
        if(!ownerCurrentD60()){st={};return;}
        // Fixed two passes, at most three current planes, no further queries.
        // Keep controller rotation; the rotated endpoint probes already took
        // part in these constraints. A sparse proxy is not exact swept mesh.
        V3 correction{};
        for(int pass=0;pass<2;++pass)for(unsigned i=0;i<planeCount;++i){
            const auto& plane=planes[i];const float projected=dotD80(correction,plane.normal);
            if(plane.penetration>projected)correction=Add(correction,Mul(plane.normal,plane.penetration-projected));
        }
        const float length=float(Len(correction));
        if(!std::isfinite(length)){st={};return;}
        if(length>.02f)correction=Mul(correction,.02f/length);
        goal=WorldToModel6692(x,Add(ModelToWorld6692(x,goal),correction));
        if(allowedOrientation)*allowedOrientation=orientation;
        if(ws::captureD47(now)){
        g_smith_tool_blocks_j27.fetch_add(1,std::memory_order_relaxed);
        if(now>=attempts.lastLog&&now-attempts.lastLog>=1000){attempts.lastLog=now;char m[300]{};
            std::snprintf(m,sizeof(m),"D80 WS TOOL_CONTACT item=%s side=%d surface=%d planes=%u correctionMm=%.3f capMm=20 rotation=CONTROLLER sparseMesh=UNPROVEN blocks=%llu",
                ws::toolName(socket.smith36.tool),side,surface,planeCount,1000.*Len(correction),(unsigned long long)g_smith_tool_blocks_j27.load());
            diagnostic_trace::Write(m);}}
    }
    st.lastGoal=goal;st.lastQ=orientation;
}
