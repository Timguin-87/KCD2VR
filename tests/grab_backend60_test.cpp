// ACTUAL producer + backend + native phase dispatch/leases/actions. Only retail
// memory/native engine functions and the proxy geometry owner are substituted.
#define KCD2VR_PHYSICS_PHASE_T4_TEST 1
#include "../src/native_physics_phase_t4.cpp"
#include "../src/subsystems/physics/runtime/native_grab_backend60.h"
#include "../src/subsystems/physics/runtime/native_grab_producer60.inl"
#include "../src/subsystems/physics/runtime/committed_hand_push_h21.h"
#include <windows.h> // DS-F70: GetTickCount64 for the visible-failure publish
#include <map>
#include <cassert>
#include <iostream>
namespace pp=kcd2vr::native_physics_phase_t4;namespace gp=kcd2vr::physics::grab_producer60;
namespace hp=kcd2vr::physics::hand_proxy60;namespace be=kcd2vr::physics::native_grab_backend60;
namespace pt=kcd2vr::constraint_packet60;namespace pm=kcd2vr::pose_core;namespace ht=kcd2vr::native_hand_proxy_t4;
namespace {
std::array<unsigned char,0x6100> world{};std::array<unsigned char,0x500> prop{},proxy{},entity{};
std::map<std::uintptr_t,std::uintptr_t> slots;constexpr std::uintptr_t base=0x10000000;
pp::Lease proxyLease;pp::Context* currentContext=nullptr;ht::Achieved achieved{};
int refs=0,adds=0,removes=0,retireCalls=0,velocities=0,retireDelayD40=0,withdrawalsD41=0;bool withdrawalProbeD41=false;bool ready=false,exists=false,wrongBuddy=false,proxyRegistered=true,adversarial=false;
std::uint64_t now=1000;pm::Vec3 propPos{1,2,3},proxyPos{1.2,2.1,3};
float captured0[3]{},captured1[3]{};unsigned capturedFlags=0;
pp::V3 capturedVelocity{},capturedAngular{};
pp::V3 propVelocity{};
template<class T>void set(void* p,std::size_t n,T v){std::memcpy(static_cast<unsigned char*>(p)+n,&v,sizeof(v));}
template<class T>T get(const void* p,std::size_t n){T v{};std::memcpy(&v,static_cast<const unsigned char*>(p)+n,sizeof(v));return v;}
bool memory(const void* p,void* out,std::size_t n){const auto a=reinterpret_cast<std::uintptr_t>(p);
 if(a==base+0x492D830&&n==sizeof(void*)){void* w=world.data();std::memcpy(out,&w,n);return true;}
 if(auto it=slots.find(a);it!=slots.end()&&n==sizeof(std::uintptr_t)){std::memcpy(out,&it->second,n);return true;}
 for(auto range:{std::pair{world.data(),world.size()},std::pair{prop.data(),prop.size()},std::pair{proxy.data(),proxy.size()},std::pair{entity.data(),entity.size()}}){
 const auto begin=reinterpret_cast<std::uintptr_t>(range.first);if(a>=begin&&a-begin<=range.second&&n<=range.second-(a-begin)){std::memcpy(out,p,n);return true;}}
 return false;}
void* lookup(void*,int id){return id==17?prop.data():id==18&&proxyRegistered?proxy.data():nullptr;}
int idOf(void*,void* b){return b==prop.data()?17:18;}
int addRef(void*){return ++refs;}int release(void*){assert(refs>0);return --refs;}
int status(void* b,void* packet){const int type=get<int>(packet,0);const auto pos=b==prop.data()?propPos:proxyPos;
 if(type==1){set(packet,0x18,pp::V3{float(pos.x),float(pos.y),float(pos.z)});set(packet,0x3C,pp::Q{});set(packet,0x4C,1.f);set(packet,0x50,2);return 1;}
 if(type==8){set(packet,0x4C,1.f);set(packet,0x18,pp::V3{0,0,3});set(packet,0xC,b==prop.data()?propVelocity:pp::V3{});return 1;}
 assert(type==26&&b==prop.data());if(get<int>(packet,4)!=77||!exists)return 0;
 set(packet,0xC,0x2202u);set(packet,0x38,wrongBuddy?static_cast<void*>(prop.data()):static_cast<void*>(proxy.data()));return 1;}
int action(void* b,void* packet,int immediate){assert(b==prop.data()&&immediate==1);const int type=get<int>(packet,0);
 if(type==10){propVelocity=capturedVelocity=get<pp::V3>(packet,12);capturedAngular=get<pp::V3>(packet,24);++velocities;return 1;}
 if(type==5){++adds;capturedFlags=get<unsigned>(packet,0x60);for(unsigned i=0;i<3;++i){captured0[i]=get<float>(packet,0x10+4*i);captured1[i]=get<float>(packet,0x1C+4*i);}exists=true;return 77;}
 assert(type==6&&get<unsigned>(packet,4)==77&&get<int>(packet,0x10)==1);++removes;exists=false;return 1;}
}
namespace kcd2vr::physics::hand_proxy60 {
void SubmitHand(int,const ht::Input& i) noexcept{if(!i.tick&&!i.source.player)++withdrawalsD41;achieved.source=i.source;achieved.incarnation=5;achieved.valid=ready;achieved.retired=false;achieved.state=ht::State::Active;achieved.tick=i.tick;}
bool SubmitHandOwned(pp::Context& c,int side,const ht::Input& i) noexcept{assert(c.Current());SubmitHand(side,i);return true;}
pp::Lease* CurrentProxyLease(pp::Context& c,int side,std::uint64_t incarnation) noexcept{assert(c.Current());return side==0&&incarnation==5&&ready&&proxyLease.Held()?&proxyLease:nullptr;}
bool RetireOwned(pp::Context& c,int side) noexcept;
bool ReadAchieved(int,ht::Achieved& out) noexcept{out=achieved;return true;}
pp::Lease* CleanupProxyLease(pp::Context& c,int side,std::uint64_t incarnation) noexcept {return CurrentProxyLease(c,side,incarnation);}
bool PinConstraint(pp::Context& c,int side,std::uint64_t incarnation,bool) noexcept {return CleanupProxyLease(c,side,incarnation)!=nullptr;}
pp::Lease* CurrentProxyLease(int side) noexcept{return side==0&&ready&&proxyLease.Held()?&proxyLease:nullptr;}
void Retire(int) noexcept{++retireCalls;if(retireDelayD40>0){--retireDelayD40;return;}if(proxyLease.Held()){assert(currentContext);assert(pp::Release(*currentContext,proxyLease));}proxyRegistered=false;achieved={};achieved.retired=true;achieved.state=ht::State::Off;}
bool RetireOwned(pp::Context& c,int side) noexcept{assert(c.Current());Retire(side);return true;}
}
// Explicit clock leaf permits delayed proxy retirement without sleeping.
static std::uint64_t releaseClockD40=0;
static std::uint64_t BackendClockD40() noexcept{return releaseClockD40?releaseClockD40:GetTickCount64();}
#define GetTickCount64 BackendClockD40
#include "../src/subsystems/physics/runtime/native_grab_backend60.inl"
#undef GetTickCount64
namespace {
void tick(pp::Context& c,void*) noexcept{currentContext=&c;kcd2vr::physics::committed_push_h21::Step(c,now);if(ready&&!proxyLease.Held()&&proxyRegistered)assert(pp::AcquireRigid(c,18,proxy.data(),proxyLease));gp::PhysicsStep(&c,now);
 if(withdrawalProbeD41){gp::Status s{};assert(gp::ReadStatus(0,s));const int targetWrites=withdrawalsD41;
 const int nativeWrites=adds+removes+velocities+retireCalls;auto altered=s.owner;++altered.trackingEpoch;
 assert(!be::Withdraw(&c,altered)&&withdrawalsD41==targetWrites);
 assert(be::Withdraw(&c,s.owner)&&withdrawalsD41==targetWrites+1);
 assert(!achieved.source.player&&!achieved.tick&&refs==2&&proxyLease.Held());
 assert(adds+removes+velocities+retireCalls==nativeWrites); // withdraw only invalidates owned input
 assert(be::Refresh(&c,s.owner,gp::g_session[0].intent));withdrawalProbeD41=false;}
 if(adversarial){gp::Status s{};assert(gp::ReadStatus(0,s));auto altered=s.owner;++altered.trackingEpoch;int before=adds+removes;
 assert(be::Add(&c,altered,s.constraintToken).progress==gp::Progress::Refused);
 assert(be::Read(&c,s.owner,s.constraintToken+1,s.id).kind==kcd2vr::constraint60::Status::Kind::Unknown);
 assert(be::Remove(&c,s.owner,s.constraintToken,s.id+1).progress==gp::Progress::Refused);
 wrongBuddy=true;assert(be::Remove(&c,s.owner,s.constraintToken,s.id).progress==gp::Progress::Refused);wrongBuddy=false;
 assert(adds+removes==before);adversarial=false;}
 currentContext=nullptr;}
void reset(pp::Context& c,void*) noexcept{currentContext=&c;gp::PhysicsReset(&c,now);currentContext=nullptr;}
void body(void* b){set(b,0,base+0x3A48DC0);set(b,0x68,static_cast<void*>(world.data()));set(b,0x50,0);set(b,0x47,std::int8_t(2));set(b,0x84,0u);set(b,0x194,std::uint8_t(1));}
gp::Intent input(){gp::Intent i{};i.player=0x123;i.expectedBody=reinterpret_cast<std::uintptr_t>(prop.data());i.press=1;i.poseEpoch=2;i.trackingEpoch=3;i.sourceGeneration=4;i.entity=100;i.side=0;i.physicsId=17;i.p[0]=1.2f;i.p[1]=2.1f;i.p[2]=3;i.held=i.allowed=true;return i;}
void frame(gp::Intent& i){now+=10;i.tick=i.poseTick=now;assert(gp::PublishGrip(i));pp::Access::Dispatch(world.data(),pp::Phase::Step);}
gp::Status result(){gp::Status s{};assert(gp::ReadStatus(0,s));return s;}
}
int main(){
 kcd2vr::collab_h21::setPhysicsMode(2);
 assert(gp::SetSourcePredicateD41(+[](const gp::Intent&) noexcept {return true;}));
 pp::base=base;pp::testRead=&memory;pp::api={lookup,idOf,addRef,release,status,action};
 for(auto pin:std::initializer_list<std::array<std::uintptr_t,3>>{{0x407FC80,0xB0,0x667418},{0x407FC80,0xA8,0x52950C},{0x3A48DC0,0x10,0x5311C4},{0x3A48DC0,0x18,0x531F84},{0x3A48DC0,0x30,0x8A6C90},{0x3A48DC0,0x38,0xAC0D18}})slots[base+pin[0]+pin[1]]=base+pin[2];
 set(world.data(),0,base+0x407FC80);set(world.data(),0x5FDC,0x10000u);set(world.data(),0x5FF4,0x10000u);body(prop.data());body(proxy.data());
 assert(pp::RegisterClient({tick,reset,nullptr}));pp::installed=true;gp::Backend backend{};be::FillBackend(backend);assert(gp::InstallBackend(backend));gp::SetEnabled(true);
 auto i=input();frame(i);assert(adds==0&&refs==1&&result().state==gp::State::Preparing); // source ref alone, no fabricated proxy identity
 i.p[0]+=.02f; // D40: move the hand during cold proxy preparation
 ready=true;frame(i);assert(adds==1&&refs==2&&result().id==77&&result().state==gp::State::WaitingPresent);
 assert(capturedFlags==0x2202);for(unsigned k=0;k<3;++k){assert(std::abs(captured0[k]-(i.p[k]-(k==0?.02f:0.f)))<1e-5f);assert(std::abs(captured1[k]-i.p[k])<1e-5f);}
 frame(i);assert(result().state==gp::State::Held); // same non-origin grip points, no origin snap
 auto owner=result().owner;const auto token=result().constraintToken;adversarial=true;withdrawalProbeD41=true;frame(i);
 // Direct native backend refuses wrong owners/tokens/IDs without another native call.
 currentContext=nullptr; // backend only runs from the actual current T4 callback in the next tests
 wrongBuddy=true;frame(i);assert(result().state!=gp::State::Held&&adds==1);wrongBuddy=false;frame(i);assert(result().state==gp::State::Held);
 propPos.x+=.2;frame(i);assert(result().state==gp::State::WaitingPresent);propPos.x-=.2;frame(i);assert(result().state==gp::State::Held);
 now+=20000;frame(i);assert(result().state==gp::State::Held&&result().constraintToken==token);
 gp::SetEnabled(false);frame(i);assert(removes==1&&refs==2&&result().state==gp::State::Releasing);frame(i);
 assert(refs==0&&retireCalls==1&&result().state==gp::State::Idle&&lookup(world.data(),17)==prop.data()); // prop remains registered, only refs/proxy retire
 frame(i);assert(adds==1&&removes==1);
 // Real committed sample -> real typed native rigid lease/action. Cached,
 // stale, wrong-world, foreign-owner and F9-OFF samples never write velocity.
 namespace push=kcd2vr::physics::committed_push_h21;namespace pc=kcd2vr::passive169r1;
 set(entity.data(),0,base+0x4072C78);set(entity.data(),0xC,std::uint32_t(100));
 set(prop.data(),0x20,static_cast<void*>(entity.data()));set(prop.data(),0x28,std::int16_t(2));
 push::Target target{};target.contact.body=kcd2vr::npc_contact172::Body::Rigid;target.contact.ownerRead=true;
 target.contact.owner=reinterpret_cast<std::uintptr_t>(entity.data());target.contact.collider=reinterpret_cast<std::uintptr_t>(prop.data());target.contact.entityId=100;
 target.physicsId=17;target.world=reinterpret_cast<std::uintptr_t>(world.data());
 pc::Sample x{};x.source=pc::Source::HandWorld;x.side=0;x.actor=1;x.key=2;x.epoch=3;x.request=4;x.sequence=1;x.tick=now;
 x.eligible=x.worldCoherent=true;x.status=pc::Status::Clear;x.normal={1,0,0};x.feature=0;x.requested={1,0,0};x.allowed={1,0,0};
 push::PublishCommitted(x,target);now+=10;x.tick=now;x.sequence++;x.status=pc::Status::Contact;x.requested.x-=.01;
 push::PublishCommitted(x,target);pp::Access::Dispatch(world.data(),pp::Phase::Step);assert(velocities==1&&refs==0);
 now+=10;pp::Access::Dispatch(world.data(),pp::Phase::Step);assert(velocities==1); // no repeating cached contact
 x.tick=now;x.sequence++;x.requested.x-=.01;push::PublishCommitted(x,target);pp::Access::Dispatch(world.data(),pp::Phase::Step);assert(velocities==1); // new contact sample cannot accelerate an already matching prop again
 x.tick=now;x.sequence++;x.requested.x-=.01;push::PublishCommitted(x,target);now+=81;pp::Access::Dispatch(world.data(),pp::Phase::Step);assert(velocities==1);
 now+=10;x.tick=now;x.sequence++;x.requested.x-=.01;push::PublishCommitted(x,target); // refresh motion after discontinuity
 now+=10;x.tick=now;x.sequence++;x.requested.x-=.01;push::PublishCommitted(x,target);set(prop.data(),0x20,static_cast<void*>(proxy.data()));
 pp::Access::Dispatch(world.data(),pp::Phase::Step);assert(velocities==1&&refs==0);set(prop.data(),0x20,static_cast<void*>(entity.data()));
 now+=10;x.tick=now;x.sequence++;x.requested.x-=.01;push::PublishCommitted(x,target);kcd2vr::collab_h21::setPhysicsMode(0);
 pp::Access::Dispatch(world.data(),pp::Phase::Step);assert(velocities==1&&refs==0);kcd2vr::collab_h21::setPhysicsMode(2);
 // Throw runs only after real constraint Absent, on this exact rigid source.
 // The mailbox is non-consuming: a later same-source cleanup cannot replay it.
 gp::SetEnabled(true);proxyRegistered=true;achieved={};i.press=2;frame(i);frame(i);frame(i);assert(result().state==gp::State::Held&&refs==2);
 kcd2vr::collab_h21::setThrowing(true);kcd2vr::throw60::ThrowRequest tr{};const auto throwOwner=result().owner;
 tr.press=i.press;tr.side=0;tr.entity=100;tr.body=reinterpret_cast<std::uintptr_t>(prop.data());tr.entityPointer=reinterpret_cast<std::uintptr_t>(entity.data());
 tr.world=throwOwner.world;tr.worldGeneration=throwOwner.worldGeneration;tr.poseEpoch=throwOwner.poseEpoch;tr.trackingEpoch=throwOwner.trackingEpoch;
 tr.physicsEpoch=kcd2vr::collab_h21::epoch.load();tr.throwEpoch=kcd2vr::collab_h21::throwEpoch.load();tr.tick=GetTickCount64();tr.v[0]=2;
 releaseClockD40=tr.tick;retireDelayD40=3;
 assert(kcd2vr::throw60::publish(tr));gp::SetEnabled(false);frame(i);assert(velocities==1);frame(i);
 assert(velocities==2&&refs==2&&result().state==gp::State::Retiring&&capturedVelocity.x==2&&capturedAngular.z==3);
 releaseClockD40+=200; // release proof is now old; original once-only mutation already happened after Absent
 frame(i);frame(i);frame(i);assert(velocities==2&&refs==0);releaseClockD40=0;
 // A previous press's unconsumed mailbox cannot throw the next grabbed prop.
 gp::SetEnabled(true);proxyRegistered=true;achieved={};i.press=3;frame(i);frame(i);frame(i);assert(result().state==gp::State::Held);
 gp::SetEnabled(false);frame(i);frame(i);assert(velocities==2&&refs==0);
 // Same-world shutdown drains the existing native constraint once, observes
 // actual Absent on a later callback, then retires proxy/source references.
 gp::SetEnabled(true);proxyRegistered=true;achieved={};i.press=4;frame(i);frame(i);frame(i);assert(result().state==gp::State::Held&&refs==2);
 pp::RequestShutdown();now+=10;pp::Access::Dispatch(world.data(),pp::Phase::Step);assert(removes==4&&refs==2);
 now+=10;pp::Access::Dispatch(world.data(),pp::Phase::Step);assert(refs==0&&result().state==gp::State::Idle);
 std::cout<<"PASS actual producer/backend/native T4: delayed readiness, world grip anchors, immediate typed Add, buddy+measured follow, long hold, Remove/Absent/retire; prop survives\n";
}
