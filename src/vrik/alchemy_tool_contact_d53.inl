#include "alchemy_tool_contact_d55.h"
// D53: mode3 held alchemy props gain the same bounded WORLD contact proxy as
// smith tools. The same pinned native bounds reader reads the held entity;
// selection distance alone is not a collision. Three sparse bounds probes
// cannot certify mesh contact, mortar interiors, or tool-to-tool collision.
// The last COMMITTED tool pose/result is the entire history: rejected work never
// advances a cache. Current-plane correction is capped/permissive; no native effect.
static void AlchemyToolContactD53(void* character,std::uint64_t epoch,int side,std::uint64_t sample,std::uint64_t tracking,
    V3& goal,Q4& orientation,AlchemyContactResultD80& staged) noexcept {
    staged={};
    namespace ab=kcd2vr::subsystems::alchemy::bridge67;
    namespace al=kcd2vr::alchemy_tool18;
    namespace prox=kcd2vr::subsystems::alchemy::near67;
    namespace ws=kcd2vr::station_ws_j27;
    namespace mh=kcd2vr::minigame_hands45;
    const auto now=std::uint64_t(GetTickCount64());
    if(!sample||!tracking||g_phys_fault6698.load(std::memory_order_acquire)||!kcd2vr::alchemy_tool_contact_d55::enabled.load(std::memory_order_acquire)||side<0||side>1||mh::modeFor(mh::Alchemy)!=mh::SixDofVrik||ToolExperimentReason59c10(now)||
       !std::isfinite(goal.x)||!std::isfinite(goal.y)||!std::isfinite(goal.z)||
       !std::isfinite(orientation.x)||!std::isfinite(orientation.y)||!std::isfinite(orientation.z)||!std::isfinite(orientation.w))return;
    ToolSocket59c10 socket{};al::Binding current{};int presentationSideR93=-1;
    if(!g_tool_socket59c10[side].read(socket)||!socket.alchemy18||socket.nativeGrasp13||socket.smith36||
       socket.side!=side||socket.character!=character||socket.epoch!=epoch||!socket.gripReady67||
       !prox::fresh(socket.tick,now)||!socket.entity||
       socket.request!=kcd2vr::workstation178r20c1::experimentEpoch36.load(std::memory_order_acquire)||
       socket.modeEpoch67!=mh::modeEpoch67.load(std::memory_order_acquire)||
       !ReadAlchemyTool18(socket.attachment,socket.id,reinterpret_cast<std::uintptr_t>(character),current)||
       !al::same(current,socket.alchemy18)||!ab::presentationOwnsCurrentR95(current,epoch,[]() noexcept {return std::uint64_t(GetTickCount64());},nullptr,&presentationSideR93)||presentationSideR93!=side)return;
    CommittedFrame6695 prev{};
    if(!g_committed_frame6695.read(prev)||prev.stamp.actor!=reinterpret_cast<std::uintptr_t>(character)||
       prev.stamp.epoch!=epoch||prev.doorTrackingEpoch66!=tracking||!prox::fresh(prev.stamp.tick,now)||!prev.valid[side]||!prev.tools59c10[side].valid)return;
    const auto& old=prev.tools59c10[side];
    if(!old.socket.gripReady67||old.socket.request!=socket.request||old.socket.modeEpoch67!=socket.modeEpoch67||
       old.socket.id!=socket.id||old.socket.attachment!=socket.attachment||!al::same(old.socket.alchemy18,current)||
       !ValidPose6689(prev.allowed[side])||!ValidPose6689(old.model)||Len(Sub(goal,FromT(prev.allowed[side])))>.5f)return;
    ModelWorld6692 world{};if(!ReadModelWorld6692(character,world))return;
    AlchemyContactResultD80 result{};result.binding=current;result.sample=sample;result.tracking=tracking;
    result.epoch=epoch;result.request=socket.request;result.mode=socket.modeEpoch67;
    const float input[7]{float(goal.x),float(goal.y),float(goal.z),float(orientation.x),float(orientation.y),float(orientation.z),float(orientation.w)};
    const float basis[8]{float(world.q.x),float(world.q.y),float(world.q.z),float(world.q.w),float(world.t.x),float(world.t.y),float(world.t.z),world.scale};
    std::copy_n(input,7,result.input);std::copy_n(input,7,result.output);std::copy_n(basis,8,result.world);
    const auto& cached=prev.alchemyContactD80[side];
    const auto sameNumbers=[](const float* a,const float* b,unsigned count){
        for(unsigned i=0;i<count;++i)if(!std::isfinite(a[i])||!std::isfinite(b[i])||std::fabs(a[i]-b[i])>1.e-6f)return false;
        return true;
    };
    // Repeated skin/attachment callbacks consume only a result published by a
    // successful pose transaction of THIS source sample and exact tool/root.
    // Never borrow a previous sample's plane or replenish its query budget.
    if(cached.valid&&cached.sample==sample&&cached.tracking==tracking&&cached.epoch==epoch&&
       cached.request==result.request&&cached.mode==result.mode&&al::same(cached.binding,current)&&
       sameNumbers(cached.input,input,7)&&sameNumbers(cached.world,basis,8)){
        goal={cached.output[0],cached.output[1],cached.output[2]};
        orientation={cached.output[3],cached.output[4],cached.output[5],cached.output[6]};staged=cached;return;
    }
    // Missing selection does not revoke the actual held entity's native bounds.
    // These three probes share the existing world-ray budget; no extra rays.
    float bounds[6]{};al::Binding geometryOwner{};
    if(g_phys_fault6698.load(std::memory_order_acquire)||!PhysEntityIdentity135(socket.entity,current.entity)||
       !PhysBounds6698(socket.entity,bounds)||!PhysEntityIdentity135(socket.entity,current.entity)||
       !ReadAlchemyTool18(socket.attachment,socket.id,reinterpret_cast<std::uintptr_t>(character),geometryOwner)||
       !al::same(current,geometryOwner)||!ab::presentationOwnsCurrentR95(geometryOwner,epoch,[]() noexcept {return std::uint64_t(GetTickCount64());},nullptr,&presentationSideR93)||presentationSideR93!=side)return;
    const auto probes=ws::probes(bounds);if(!probes.valid)return;
    const auto grip=RelativeFromAbsolute(prev.allowed[side],old.model);
    QuatT28 wrist{};QToQuatT(wrist,orientation);ToT(wrist,goal);
    const auto requested=AbsoluteFromRelative(wrist,grip);
    if(!ValidPose6689(grip)||!ValidPose6689(requested))return;
    const Q4 fromQ=QFromQuatT(old.model),toQ=QFromQuatT(requested);
    struct PlaneD80 {V3 normal{};float penetration=0;};PlaneD80 planes[3]{};
    unsigned queries=0,planeCount=0;bool coverage=true;
    for(int i=0;i<3;++i){
        const V3 local{float(probes.p[i][0]),float(probes.p[i][1]),float(probes.p[i][2])};
        const V3 from=ModelToWorld6692(world,Add(FromT(old.model),QRotateV(fromQ,local)));
        const V3 to=ModelToWorld6692(world,Add(FromT(requested),QRotateV(toQ,local)));
        const V3 sweep=Sub(to,from);const float length=float(Len(sweep));
        if(!(length>1.e-4f))continue;
        if(length>1.5f)return;
        const auto hit=WorldRayDistance6692(from,sweep);++queries;
        if(hit.state!=wc::Result::Clear&&hit.state!=wc::Result::Hit){coverage=false;break;}
        if(hit.state!=wc::Result::Hit||!hit.plane||!std::isfinite(hit.distance)||hit.distance<=.002f)continue;
        // Preserve the unflipped native front face: a ray-facing normal also
        // points inward on an inside-to-outside exit and would oppose release.
        // Initial overlap, back face, tangency or inconsistent point is permissive.
        // This current front-face witness does not classify solid mesh interiors.
        const auto dot=[](V3 a,V3 b){return a.x*b.x+a.y*b.y+a.z*b.z;};
        const V3 normal=hit.nativeNormalD63;
        const float nn=float(Len(normal));
        const V3 expected=Add(from,Mul(sweep,hit.distance/length));
        if(!std::isfinite(nn)||nn<.99f||nn>1.01f||Len(Sub(hit.point,expected))>.002f||
           dot(Sub(from,hit.point),normal)<=.0005f||dot(sweep,normal)>=-.0001f)continue;
        // Sparse bounds approximation: a small radius allowance and 2mm skin.
        // Desired rotated endpoints participate; correction is translation only,
        // so a single hit never freezes unrelated wrist rotation or tangent.
        const float margin=float(std::min(.01,probes.radius*world.scale)+.002);
        const float penetration=margin-dot(Sub(to,hit.point),normal);
        if(penetration>0&&std::isfinite(penetration))planes[planeCount++]={normal,penetration};
    }
    V3 correction{};
    if(coverage){
        // Fixed 2 passes over at most 3 currently observed planes, no new query.
        for(int pass=0;pass<2;++pass)for(unsigned i=0;i<planeCount;++i){
            const auto& p=planes[i];const float projected=correction.x*p.normal.x+correction.y*p.normal.y+correction.z*p.normal.z;
            if(p.penetration>projected)correction=Add(correction,Mul(p.normal,p.penetration-projected));
        }
        const float length=float(Len(correction));
        if(!std::isfinite(length))return;
        if(length>.02f)correction=Mul(correction,.02f/length); // deliberate permissive 20mm cap; not exact mesh collision
    }
    // The query is an external boundary; a changed native attachment/owner
    // cannot constrain this wrist after returning from it.
    al::Binding after{};
    if(!kcd2vr::alchemy_tool_contact_d55::enabled.load(std::memory_order_acquire)||mh::modeFor(mh::Alchemy)!=mh::SixDofVrik||socket.modeEpoch67!=mh::modeEpoch67.load(std::memory_order_acquire)||
       !ReadAlchemyTool18(socket.attachment,socket.id,reinterpret_cast<std::uintptr_t>(character),after)||
       !al::same(current,after)||!PhysEntityIdentity135(socket.entity,current.entity)||!ab::presentationOwnsCurrentR95(after,epoch,[]() noexcept {return std::uint64_t(GetTickCount64());},nullptr,&presentationSideR93)||presentationSideR93!=side)return;
    if(coverage&&planeCount)goal=WorldToModel6692(world,Add(ModelToWorld6692(world,goal),correction));
    result.output[0]=goal.x;result.output[1]=goal.y;result.output[2]=goal.z;
    result.valid=true;staged=result; // commit owns publication; rejected solves publish nothing
    if(ws::captureD47(now)){
        static std::uint64_t next[2]{};if(now>=next[side]){next[side]=now+1000;char line[400]{};
            std::snprintf(line,sizeof(line),"D80 ALCHEMY_TOOL_CONTACT entity=%u type=%d side=%d previousSample=%llu sourceSample=%llu queries=%u planes=%u coverage=%d correctionMm=%.3f capMm=20 orientation=CONTROLLER geometry=NATIVE_LOCAL_BOUNDS_3_PROBES meshContact=UNPROVEN nativeEffects=UNCHANGED",
                current.entity,current.type,side,(unsigned long long)prev.stamp.sample,(unsigned long long)sample,queries,planeCount,coverage,1000.*Len(correction));diagnostic_trace::Write(line);}
    }
}
