// Same-T4 native backend. The source is an EXISTING prop, not a body we destroy.
// Exact-native frame provenance: flags1 local path AC1443..AC156D rotates and
// adds body position; ctor world_frames2. 0x2000 angular branch AC1FA6.
#include "vrik/visible_failure.h" // DS-F70 (re-added on Codex's backend): the ONE visible line on the exact failed step
#include "../throw_policy60.h"
namespace kcd2vr::physics::native_grab_backend60 {
namespace gp=kcd2vr::physics::grab_producer60;namespace phase=native_physics_phase_t4;
namespace hp=hand_proxy60;namespace hpt4=kcd2vr::native_hand_proxy_t4;namespace pkt=constraint_packet60;namespace pm=pose_core;
static phase::Lease g_propLease[2];
struct HeldMeta {
    gp::Owner owner{};bool held=false,proxyIssued=false,ready=false,uncertainAdd=false,throwConsumedH21=false,sourceAnchorReadyD40=false;
    std::uint64_t constraintToken=0;std::uint32_t id=0;
    pm::Vec3 sourceAnchor{},proxyAnchor{},target{};pm::Quat relative{},targetQ{};
};
static HeldMeta g_meta[2]{};
static phase::Context* ctx(void* p) noexcept{return static_cast<phase::Context*>(p);}
static bool sideOk(int s) noexcept{return s>=0&&s<2;}
static bool equal(const gp::Owner& a,const gp::Owner& b) noexcept {
    return a.requestToken&&a.requestToken==b.requestToken&&a.player==b.player&&a.world==b.world&&a.worldGeneration==b.worldGeneration&&
        a.sourceGeneration==b.sourceGeneration&&a.proxyGeneration==b.proxyGeneration&&a.poseEpoch==b.poseEpoch&&a.trackingEpoch==b.trackingEpoch&&
        a.press==b.press&&a.entity==b.entity&&a.side==b.side;
}
static bool current(const gp::Owner& o,phase::Context* c) noexcept {
    return c&&c->Current()&&(c->Kind()==phase::Phase::Step||c->Kind()==phase::Phase::Shutdown)&&sideOk(o.side)&&equal(o,g_meta[o.side].owner)&&g_meta[o.side].ready&&
        o.world==reinterpret_cast<std::uintptr_t>(c->World())&&o.worldGeneration==c->Generation();
}
static bool source(const hpt4::Achieved& a,const gp::Owner& o) noexcept {
    return !a.source.physicalPresenceH24&&a.source.player==o.player&&a.source.side==o.side&&a.source.trackingEpoch==o.trackingEpoch&&
        a.source.sourceGeneration==o.sourceGeneration&&a.source.cancelGeneration==o.press;
}
static bool matchingPress(const gp::Owner& o,const gp::Intent& i,std::uint64_t token) noexcept {
    return token&&o.requestToken==token&&o.player==i.player&&o.entity==i.entity&&o.side==i.side&&o.press==i.press&&
        o.poseEpoch==i.poseEpoch&&o.trackingEpoch==i.trackingEpoch&&o.sourceGeneration==i.sourceGeneration;
}
static pm::Quat rotation(const phase::RigidSnapshot& p) noexcept{return {p.orientation.x,p.orientation.y,p.orientation.z,p.orientation.w};}
static pm::Vec3 position(const phase::RigidSnapshot& p) noexcept{return {p.position.x,p.position.y,p.position.z};}
static bool submit(phase::Context& c,const gp::Intent& in) noexcept {
    if(!gp::SourceCurrentD41(in))return false;
    hpt4::Input p{};p.tick=in.tick;p.source={in.trackingEpoch,in.sourceGeneration,in.press,in.poseTick,in.player,in.side};
    p.target={{in.q[0],in.q[1],in.q[2],in.q[3]},{in.p[0],in.p[1],in.p[2]}};return hp::SubmitHandOwned(c,in.side,p);
}
static bool Stamp(void* p,gp::Stamp& out) noexcept {
    auto* c=ctx(p);if(!c||!c->Current()||(c->Kind()!=phase::Phase::Step&&c->Kind()!=phase::Phase::Shutdown))return false;
    out={reinterpret_cast<std::uintptr_t>(c->World()),c->Generation(),c->Serial()};return true;
}
static gp::Progress Prepare(void* p,const gp::Intent& in,std::uint64_t token,gp::Owner& out) noexcept {
    auto* c=ctx(p);if(!c||!c->Current()||c->Kind()!=phase::Phase::Step)return gp::Progress::Pending;
    if(!sideOk(in.side)||in.nativeAttached||!in.entity||!in.player||!in.press||!token||in.physicsId<0||!in.expectedBody){
        if(in.nativeAttached)kcd2vr::visible_failure::publish("Grab refused: prop already attached",std::uint64_t(GetTickCount64()));
        return gp::Progress::Refused;
    }
    auto& m=g_meta[in.side];
    // A second independent constraint must not fight the first hand over one
    // prop. Two-hand support needs an explicit shared-object owner instead.
    if(g_meta[1-in.side].held&&g_meta[1-in.side].owner.entity==in.entity)return gp::Progress::Refused;
    if(m.owner.requestToken&&!matchingPress(m.owner,in,token))return gp::Progress::Refused;
    if(!m.held){
        if(!phase::AcquireRigid(*c,in.physicsId,reinterpret_cast<void*>(in.expectedBody),g_propLease[in.side]))return gp::Progress::Pending;
        m.held=true;m.owner={in.player,reinterpret_cast<std::uintptr_t>(c->World()),token,c->Generation(),in.sourceGeneration,0,in.poseEpoch,in.trackingEpoch,in.press,in.entity,in.side};
    }
    if(m.owner.world!=reinterpret_cast<std::uintptr_t>(c->World())||m.owner.worldGeneration!=c->Generation())return gp::Progress::Uncertain;
    // Capture the local contact when this source lease is first readable, not
    // several callbacks later when a cold proxy finally becomes ready. Moving
    // the controller during preparation must not slide the grip over the prop.
    if(!m.sourceAnchorReadyD40){phase::RigidSnapshot original{};
        if(!phase::ReadRigid(*c,g_propLease[in.side],original))return gp::Progress::Pending;
        m.sourceAnchor=pm::rotate(pm::conjugate(rotation(original)),pm::Vec3{in.p[0],in.p[1],in.p[2]}-position(original));
        if(!pm::finite(m.sourceAnchor))return gp::Progress::Refused;
        m.sourceAnchorReadyD40=true;
    }
    if(!submit(*c,in))return gp::Progress::Pending;m.proxyIssued=true;
    m.target={in.p[0],in.p[1],in.p[2]};m.targetQ={in.q[0],in.q[1],in.q[2],in.q[3]};
    hpt4::Achieved a{};if(!hp::ReadAchieved(in.side,a)||!a.valid||!a.incarnation||!source(a,m.owner))return gp::Progress::Pending;
    auto* buddy=hp::CurrentProxyLease(*c,in.side,a.incarnation);if(!buddy)return gp::Progress::Pending;
    phase::RigidSnapshot prop{},proxy{};
    if(!phase::ReadRigid(*c,g_propLease[in.side],prop)||!phase::ReadRigid(*c,*buddy,proxy))return gp::Progress::Pending;
    if(!m.ready){
        const pm::Vec3 grip{in.p[0],in.p[1],in.p[2]};
        m.proxyAnchor=pm::rotate(pm::conjugate(rotation(proxy)),grip-position(proxy));
        m.relative=pm::multiply(pm::conjugate(rotation(proxy)),rotation(prop));
        if(!pm::finite(m.sourceAnchor)||!pm::finite(m.proxyAnchor))return gp::Progress::Refused;
        m.owner.proxyGeneration=a.incarnation;m.ready=true;
    }
    out=m.owner;return gp::Progress::Complete;
}
static gp::Progress Drive(void* p,const gp::Owner& own,const gp::Intent& in) noexcept {
    auto* c=ctx(p);if(!current(own,c)||!matchingPress(own,in,own.requestToken))return gp::Progress::Refused;
    if(!submit(*c,in))return gp::Progress::Pending;
    g_meta[own.side].target={in.p[0],in.p[1],in.p[2]};g_meta[own.side].targetQ={in.q[0],in.q[1],in.q[2],in.q[3]};hpt4::Achieved a{};phase::RigidSnapshot actual{};auto* buddy=hp::CurrentProxyLease(*c,own.side,own.proxyGeneration);
    if(!buddy||!hp::ReadAchieved(own.side,a)||!source(a,own)||a.incarnation!=own.proxyGeneration||!a.valid||!phase::ReadRigid(*c,*buddy,actual))return gp::Progress::Pending;
    return gp::Progress::Complete;
}
static gp::Result Add(void* p,const gp::Owner& own,std::uint64_t token) noexcept {
    gp::Result out{};out.owner=own;out.constraintToken=token;auto* c=ctx(p);
    if(!current(own,c)||!token){out.progress=gp::Progress::Refused;return out;}
    auto& m=g_meta[own.side];out.serial=c->Serial();
    if(m.constraintToken&&m.constraintToken!=token){out.progress=gp::Progress::Refused;return out;}
    if(m.uncertainAdd){out.progress=gp::Progress::Uncertain;return out;}
    if(m.id){out.id=m.id;out.progress=gp::Progress::Complete;return out;}
    auto* buddy=hp::CurrentProxyLease(*c,own.side,own.proxyGeneration);hpt4::Achieved a{};
    if(!buddy||!hp::ReadAchieved(own.side,a)||!a.valid||!source(a,own)||a.incarnation!=own.proxyGeneration)return out;
    phase::RigidSnapshot prop{},proxy{};
    if(!phase::ReadRigid(*c,g_propLease[own.side],prop)||!phase::ReadRigid(*c,*buddy,proxy))return out;
    const auto point0=position(prop)+pm::rotate(rotation(prop),m.sourceAnchor);
    const auto point1=position(proxy)+pm::rotate(rotation(proxy),m.proxyAnchor);
    auto packet=pkt::defaults();auto* dst=phase::NativeHandle(*c,*buddy);if(!dst)return out;
    pkt::setBuddy(packet,reinterpret_cast<std::uintptr_t>(dst));
    // Explicit world frames(2), ignore buddy(200), rigid relative orientation(2000).
    if(!pkt::setFlags(packet,0x2202)){out.progress=gp::Progress::Refused;return out;}
    pkt::setPoint(packet,0,float(point0.x),float(point0.y),float(point0.z));pkt::setPoint(packet,1,float(point1.x),float(point1.y),float(point1.z));
    if(!hp::PinConstraint(*c,own.side,own.proxyGeneration,true))return out;
    m.constraintToken=token;const auto result=phase::AddConstraint(*c,g_propLease[own.side],*buddy,packet);
    if(result.kind==phase::ConstraintResult::Kind::Uncertain){m.uncertainAdd=true;out.progress=gp::Progress::Uncertain;return out;}
    if(result.kind!=phase::ConstraintResult::Kind::Submitted||!result.id){
        (void)hp::PinConstraint(*c,own.side,own.proxyGeneration,false);
        kcd2vr::visible_failure::publish("Grab refused by physics",std::uint64_t(GetTickCount64()));
        out.progress=gp::Progress::Refused;return out;
    }
    m.id=result.id;out.id=m.id;out.progress=gp::Progress::Complete;return out;
}
static gp::Sample Read(void* p,const gp::Owner& own,std::uint64_t token,std::uint32_t id) noexcept {
    gp::Sample out{};out.owner=own;out.constraintToken=token;auto* c=ctx(p);if(!current(own,c))return out;
    auto& m=g_meta[own.side];if(!token||token!=m.constraintToken||!m.id||(id&&id!=m.id))return out;
    out.id=m.id;phase::ConstraintSnapshot cs{};if(!phase::ReadConstraint(*c,g_propLease[own.side],int(m.id),cs))return out;
    out.serial=c->Serial();if(!cs.exists){(void)hp::PinConstraint(*c,own.side,own.proxyGeneration,false);out.kind=constraint60::Status::Kind::Absent;return out;}
    auto* buddy=hp::CleanupProxyLease(*c,own.side,own.proxyGeneration);if(!buddy)return out;
    if(cs.buddy!=phase::NativeHandle(*c,*buddy))return out;
    hpt4::Achieved a{};phase::RigidSnapshot prop{},proxy{};
    out.kind=constraint60::Status::Kind::Present;
    if((cs.flags&0x100u)||!hp::ReadAchieved(own.side,a)||!a.valid||!source(a,own)||a.incarnation!=own.proxyGeneration||
       !phase::ReadRigid(*c,g_propLease[own.side],prop)||!phase::ReadRigid(*c,*buddy,proxy))return out;
    const auto point0=position(prop)+pm::rotate(rotation(prop),m.sourceAnchor);
    const auto point1=position(proxy)+pm::rotate(rotation(proxy),m.proxyAnchor);
    const double distance=pm::length(point0-point1);
    const double angle=pm::angle(pm::multiply(pm::conjugate(rotation(proxy)),rotation(prop)),m.relative);
    const double controlDistance=pm::length(point1-m.target);
    const double controlAngle=pm::angle(rotation(proxy),m.targetQ);
    out.following=std::isfinite(distance)&&distance<=.05&&std::isfinite(angle)&&angle<=.35&&
        std::isfinite(controlDistance)&&controlDistance<=.10&&std::isfinite(controlAngle)&&controlAngle<=.5;
    return out;
}
static gp::Result Remove(void* p,const gp::Owner& own,std::uint64_t token,std::uint32_t id) noexcept {
    gp::Result out{};out.owner=own;out.constraintToken=token;out.id=id;auto* c=ctx(p);
    if(!current(own,c)){out.progress=gp::Progress::Refused;return out;}
    auto& m=g_meta[own.side];if(!id||id!=m.id||token!=m.constraintToken){out.progress=gp::Progress::Refused;return out;}
    phase::ConstraintSnapshot snapshot{};auto* buddy=hp::CleanupProxyLease(*c,own.side,own.proxyGeneration);
    if(!buddy||!phase::ReadConstraint(*c,g_propLease[own.side],int(id),snapshot)||!snapshot.exists||
       snapshot.buddy!=phase::NativeHandle(*c,*buddy)){out.progress=gp::Progress::Refused;return out;}
    const auto result=phase::RemoveConstraint(*c,g_propLease[own.side],id);out.serial=c->Serial();
    out.progress=result.kind==phase::ConstraintResult::Kind::Submitted?gp::Progress::Complete:gp::Progress::Uncertain;
    return out;
}
static gp::Progress Retire(void* p,const gp::Owner& own) noexcept {
    if(!sideOk(own.side))return gp::Progress::Refused;auto* c=ctx(p);auto& m=g_meta[own.side];
    if(!m.held&&!m.proxyIssued)return gp::Progress::Complete;
    if(!own.requestToken||m.owner.requestToken!=own.requestToken||m.owner.player!=own.player||m.owner.entity!=own.entity||m.owner.press!=own.press)return gp::Progress::Refused;
    if(!c||!c->Current()||m.owner.world!=reinterpret_cast<std::uintptr_t>(c->World())||m.owner.worldGeneration!=c->Generation())return gp::Progress::Uncertain;
    if(m.held){
        throw60::ThrowRequest tr{};const auto now=std::uint64_t(GetTickCount64());
        if(m.id&&!m.throwConsumedH21&&c->Kind()==phase::Phase::Step&&throw60::read(own.side,tr)&&
           collab_h21::throwing.load(std::memory_order_acquire)&&collab_h21::current(tr.physicsEpoch)&&
           tr.throwEpoch==collab_h21::throwEpoch.load(std::memory_order_acquire)&&tr.tick&&now>=tr.tick&&now-tr.tick<=80&&
           tr.side==own.side&&tr.press==own.press&&tr.entity==own.entity&&tr.poseEpoch==own.poseEpoch&&tr.trackingEpoch==own.trackingEpoch&&
           tr.world==own.world&&tr.worldGeneration==own.worldGeneration&&tr.body==reinterpret_cast<std::uintptr_t>(phase::NativeHandle(*c,g_propLease[own.side]))&&
           tr.entityPointer&&phase::RigidEntityIdentity(*c,g_propLease[own.side],reinterpret_cast<void*>(tr.entityPointer),tr.entity)){
            m.throwConsumedH21=true; // read is NON-consuming; never retry an entered native velocity action
            phase::RigidSnapshot dynamics{};const auto speed=throw60::speed(tr.v);
            if(speed>0&&speed<=throw60::kMaxThrowSpeed&&phase::ReadRigid(*c,g_propLease[own.side],dynamics))
                (void)phase::SetVelocity(*c,g_propLease[own.side],{tr.v[0],tr.v[1],tr.v[2]},dynamics.angularVelocity);
        }
    }
    // The source is already unconstrained here. Apply its once-only release
    // velocity before waiting several callbacks for proxy destruction; that
    // retirement delay must not age a fresh release proof out of existence.
    // The producer permits retirement only before Add or after exact native Absent.
    if(m.proxyIssued){if(!hp::RetireOwned(*c,own.side))return gp::Progress::Uncertain;hpt4::Achieved a{};
        if(!hp::ReadAchieved(own.side,a)||!a.retired||a.state!=hpt4::State::Off)return gp::Progress::Pending;
        m.proxyIssued=false;
    }
    if(m.held){
        if(!phase::Release(*c,g_propLease[own.side]))return gp::Progress::Uncertain;m.held=false;
    }
    m=HeldMeta{};return gp::Progress::Complete;
}
static gp::Progress Reset(void* p,const gp::Owner& own) noexcept {
    auto* c=ctx(p);if(!c||!c->Current()||c->Kind()!=phase::Phase::WorldReset||!sideOk(own.side))return gp::Progress::Uncertain;
    auto& m=g_meta[own.side];if(!m.held&&!m.proxyIssued)return gp::Progress::Complete;
    if(!own.requestToken||m.owner.requestToken!=own.requestToken||m.owner.player!=own.player||m.owner.entity!=own.entity||m.owner.press!=own.press||
       m.owner.world!=reinterpret_cast<std::uintptr_t>(c->World())||m.owner.worldGeneration!=c->Generation())return gp::Progress::Uncertain;
    if(m.held&&!phase::Release(*c,g_propLease[own.side]))return gp::Progress::Uncertain;
    (void)hp::PinConstraint(*c,own.side,own.proxyGeneration,false);
    m=HeldMeta{};return gp::Progress::Complete; // authenticated native reset itself retires constraints; proxy Reset owns its own body
}
static bool Refresh(void* p,const gp::Owner& own,const gp::Intent& in) noexcept {
    auto* c=ctx(p);return current(own,c)&&c->Kind()==phase::Phase::Step&&matchingPress(own,in,own.requestToken)&&submit(*c,in);
}
static bool Withdraw(void* p,const gp::Owner& own) noexcept {
    auto* c=ctx(p);if(!c||!c->Current()||c->Kind()!=phase::Phase::Step||!sideOk(own.side))return false;
    const auto& m=g_meta[own.side];
    if(!m.proxyIssued)return true;
    if(!own.requestToken||own.requestToken!=m.owner.requestToken||own.player!=m.owner.player||
       own.entity!=m.owner.entity||own.press!=m.owner.press||own.poseEpoch!=m.owner.poseEpoch||
       own.trackingEpoch!=m.owner.trackingEpoch||own.sourceGeneration!=m.owner.sourceGeneration||
       (own.proxyGeneration&&own.proxyGeneration!=m.owner.proxyGeneration)||
       m.owner.world!=reinterpret_cast<std::uintptr_t>(c->World())||m.owner.worldGeneration!=c->Generation())return false;
    // Empty input revokes the motor; it does not destroy the pinned buddy or
    // release the source. CleanupProxyLease remains available for Remove/Absent.
    return hp::SubmitHandOwned(*c,own.side,hpt4::Input{});
}
void FillBackend(gp::Backend& out) noexcept{out={&Stamp,&Prepare,&Drive,&Add,&Read,&Remove,&Retire,&Reset,&Refresh,&Withdraw};}
}
