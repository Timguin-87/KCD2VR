// Includes the ACTUAL producer and native lifecycle; only engine leaves are mocked.
#include "../src/subsystems/physics/runtime/native_grab_producer60.h"
#include "../src/subsystems/physics/runtime/native_grab_producer60.inl"
#include <cstdio>
#include <cstdlib>
namespace g=kcd2vr::physics::grab_producer60;
namespace cs=kcd2vr::constraint60;
static int failures=0;
#define CK(x) do{if(!(x)){std::printf("FAIL line %d: %s\n",__LINE__,#x);++failures;}}while(0)
struct Native {
    g::Owner owner{};g::Stamp clock{0xAABB,7,1};std::uint64_t now=1000,token=0,addSerial=0,removeSerial=0;
    unsigned prepares=0,adds=0,removes=0,drives=0,retires=0,refs=0;
    bool preparePending=false,addPending=false,addUncertain=false,holdPresent=false,holdAbsent=false,wrongOwner=false,wrongId=false;
    bool follow=true,wrongReply=false,retirePending=false,removeUncertain=false;g::Progress driveResult=g::Progress::Complete;
};
static bool stamp(void* p,g::Stamp& out) noexcept{out=static_cast<Native*>(p)->clock;return true;}
static g::Progress prepare(void* p,const g::Intent& in,std::uint64_t request,g::Owner& out) noexcept{
 auto& n=*static_cast<Native*>(p);++n.prepares;n.refs=2;n.owner=out;n.owner.world=n.clock.world;n.owner.worldGeneration=n.clock.generation;n.owner.proxyGeneration=99;
 out=n.owner;return n.preparePending?g::Progress::Pending:g::Progress::Complete;
}
static g::Progress drive(void* p,const g::Owner&,const g::Intent&) noexcept{auto& n=*static_cast<Native*>(p);++n.drives;return n.driveResult;}
static g::Result add(void* p,const g::Owner& owner,std::uint64_t token) noexcept{
 auto& n=*static_cast<Native*>(p);++n.adds;n.token=token;
 if(n.addPending)return {};
 n.addSerial=n.clock.serial;auto result=g::Result{owner,token,n.clock.serial,77,g::Progress::Complete};
 if(n.wrongReply)++result.owner.requestToken;
 if(n.addUncertain)result.progress=g::Progress::Uncertain;
 return result;
}
static g::Sample read(void* p,const g::Owner& owner,std::uint64_t token,std::uint32_t id) noexcept{
 auto& n=*static_cast<Native*>(p);g::Sample s{owner,token,n.clock.serial,id?id:77,cs::Status::Kind::Unknown,n.follow};
 if(n.wrongOwner)++s.owner.requestToken;if(n.wrongId)++s.id;
 if(n.removeSerial&&n.clock.serial>n.removeSerial&&!n.holdAbsent)s.kind=cs::Status::Kind::Absent;
 else if(n.addSerial&&n.clock.serial>n.addSerial&&!n.holdPresent)s.kind=cs::Status::Kind::Present;
 return s;
}
static g::Result remove(void* p,const g::Owner& owner,std::uint64_t token,std::uint32_t id) noexcept{
 auto& n=*static_cast<Native*>(p);++n.removes;n.removeSerial=n.clock.serial;return {owner,token,n.clock.serial,id,n.removeUncertain?g::Progress::Uncertain:g::Progress::Complete};
}
static g::Progress retire(void* p,const g::Owner&) noexcept{auto& n=*static_cast<Native*>(p);++n.retires;if(n.retirePending)return g::Progress::Pending;n.refs=0;return g::Progress::Complete;}
static g::Backend backend(){return {stamp,prepare,drive,add,read,remove,retire};}
static g::Intent intent(std::uint64_t press=1){g::Intent i{};i.player=0x1234;i.entity=100;i.expectedBody=0xB0D;i.physicsId=41;i.side=0;i.press=press;
 i.poseEpoch=3;i.trackingEpoch=4;i.sourceGeneration=5;i.allowed=i.held=true;return i;}
static void reset(){for(auto& s:g::g_session)s={};g::SetEnabled(true);}
static void frame(Native& n,g::Intent& i,bool refresh=true){n.now+=10;++n.clock.serial;if(refresh)i.tick=i.poseTick=n.now;CK(g::PublishGrip(i));g::PhysicsStep(&n,n.now);}
static g::Status status(){g::Status s{};CK(g::ReadStatus(0,s));return s;}
int main(){
 kcd2vr::collab_h21::setPhysicsMode(2);
 CK(g::SetSourcePredicateD41(+[](const g::Intent&) noexcept {return true;}));
 CK(g::InstallBackend(backend()));CK(!g::InstallBackend(backend()));
 // Actual producer wake policy: feature OFF is idle only after all resources
 // and transfer tickets have retired. No copied/old intent wakes it by itself.
 {g::SetEnabled(false);CK(!g::NeedsPhysicsStepD80());
  g::g_session[0].state=g::State::Retiring;CK(g::NeedsPhysicsStepD80());g::g_session[0]={};
  g::g_session[1].owns=true;CK(g::NeedsPhysicsStepD80());g::g_session[1]={};
  g::g_session[0].addPending=true;CK(g::NeedsPhysicsStepD80());g::g_session[0]={};
  g::g_transferTicket[1]=9;CK(g::NeedsPhysicsStepD80());g::g_transferTicket[1]=0;
  CK(!g::NeedsPhysicsStepD80());g::SetEnabled(true);CK(g::NeedsPhysicsStepD80());
  kcd2vr::collab_h21::setPhysicsMode(0);CK(!g::NeedsPhysicsStepD80());
  kcd2vr::collab_h21::setPhysicsMode(2);}

 // Exact source path: prepare -> actual native add result -> later Present + measured follow -> remove -> later Absent -> retire.
 {reset();Native n{};auto i=intent();frame(n,i);CK(n.adds==1&&n.refs==2&&status().state==g::State::WaitingPresent);auto token=status().constraintToken;
 frame(n,i);CK(status().state==g::State::Held&&n.drives==1);n.now+=20000;frame(n,i);CK(status().state==g::State::Held&&status().constraintToken==token);
 g::Cancel(0);n.now+=10;++n.clock.serial;g::PhysicsStep(&n,n.now);CK(n.removes==1&&n.retires==0&&n.refs==2);
 n.holdAbsent=true;n.now+=10;++n.clock.serial;g::PhysicsStep(&n,n.now);CK(n.removes==1&&n.retires==0&&n.refs==2);
 n.holdAbsent=false;n.now+=10;++n.clock.serial;g::PhysicsStep(&n,n.now);CK(status().state==g::State::Idle&&n.refs==0&&n.retires==1);
 // No reacquire from the stale same grip publication.
 n.now+=10;++n.clock.serial;g::PhysicsStep(&n,n.now);CK(n.adds==1);}
 // Present alone never advertises Held. A successful measured drive is necessary.
 {reset();Native n{};n.follow=false;auto i=intent(2);frame(n,i);frame(n,i);CK(status().state==g::State::WaitingPresent);n.follow=true;frame(n,i);CK(status().state==g::State::Held);}
 // Disabling stops admission but continues native Remove/Absent and multi-step resource retirement.
 {reset();Native n{};auto i=intent(3);frame(n,i);frame(n,i);g::SetEnabled(false);frame(n,i);CK(n.removes==1&&n.refs==2);
 n.retirePending=true;frame(n,i);CK(n.refs==2&&status().state==g::State::Retiring);n.retirePending=false;frame(n,i);CK(n.refs==0&&status().state==g::State::Idle);}
 // Tracking/cancel while preparing cannot create an add; partial resources are retired.
 {reset();Native n{};n.preparePending=true;auto i=intent(4);frame(n,i);CK(n.refs==2&&n.adds==0);g::Cancel(0);n.now+=10;++n.clock.serial;g::PhysicsStep(&n,n.now);CK(n.refs==0&&n.adds==0);}
 // Busy native Add means no submitted command. Cancel safely retires without Remove/false Absent.
 {reset();Native n{};n.addPending=true;auto i=intent(5);frame(n,i);CK(n.adds==1&&n.refs==2);g::SetEnabled(false);frame(n,i);CK(n.refs==0&&n.removes==0);}
 // Future/ancient/unnormalized/attached evidence never reaches native Prepare.
 {reset();Native n{};auto i=intent(6);i.tick=1010;i.poseTick=1;CK(g::PublishGrip(i));n.now=1010;++n.clock.serial;g::PhysicsStep(&n,n.now);CK(n.prepares==0);
 i.poseTick=i.tick=1020;i.q[3]=0;CK(g::PublishGrip(i));n.now=1020;++n.clock.serial;g::PhysicsStep(&n,n.now);CK(n.prepares==0);
 i.q[3]=1;i.nativeAttached=true;frame(n,i);CK(n.prepares==0);}
 // Wrong lifecycle/ID status cannot be Held, and no resources are silently freed.
 {reset();Native n{};auto i=intent(7);frame(n,i);n.wrongOwner=true;frame(n,i);CK(status().state!=g::State::Held&&n.refs==2);n.wrongOwner=false;n.wrongId=true;frame(n,i);CK(status().state!=g::State::Held);}
 // Uncertain add retains BOTH leases, then the exact late ID is drained without resubmission.
 {reset();Native n{};n.addUncertain=true;auto i=intent(8);frame(n,i);CK(status().state==g::State::Quarantined&&n.refs==2&&n.adds==1);
 frame(n,i);CK(status().state==g::State::Releasing&&status().id==77);frame(n,i);CK(n.removes==1&&n.adds==1&&n.refs==2);frame(n,i);CK(n.refs==0);}
 // Malformed successful native reply cannot be treated as clean refusal.
 {reset();Native n{};n.wrongReply=true;auto i=intent(9);frame(n,i);CK(status().state==g::State::Quarantined&&n.refs==2&&n.retires==0);}
 // A fault during held Drive drains the exact still-current native constraint; it never silently abandons it.
 {reset();Native n{};auto i=intent(11);frame(n,i);frame(n,i);n.driveResult=g::Progress::Uncertain;frame(n,i);
 CK(status().state==g::State::Quarantined&&n.refs==2);frame(n,i);CK(n.removes==1&&n.refs==2);frame(n,i);CK(n.refs==0);}
 // New grip on the same prop cannot consume an old native ACK from the previous request token.
 {reset();Native n{};auto i=intent(12);frame(n,i);const auto oldOwner=n.owner;const auto oldToken=n.token;frame(n,i);
 i.held=false;frame(n,i);frame(n,i);CK(n.refs==0);i=intent(13);n.addSerial=n.removeSerial=0;frame(n,i);
 CK(status().owner.requestToken!=oldOwner.requestToken&&status().constraintToken!=oldToken);n.wrongOwner=true;frame(n,i);CK(status().state!=g::State::Held);}
 // Same address in a new world generation is still a replacement; no native mutation through old ownership.
 {reset();Native n{};auto i=intent(10);frame(n,i);frame(n,i);++n.clock.generation;frame(n,i);CK(status().state==g::State::Quarantined&&n.refs==2&&n.removes==0&&n.retires==0);}
 {reset();Native n{};auto i=intent(22);frame(n,i);frame(n,i);n.removeUncertain=true;n.holdAbsent=true;
  g::Cancel(0);++n.clock.serial;n.now+=10;g::PhysicsStep(&n,n.now);CK(n.removes==1&&status().state==g::State::Quarantined);
  for(int repeat=0;repeat<4;++repeat){++n.clock.serial;n.now+=10;g::PhysicsStep(&n,n.now);}
  CK(n.removes==1&&n.refs==2);n.holdAbsent=false;++n.clock.serial;n.now+=10;g::PhysicsStep(&n,n.now);CK(n.removes==1&&n.refs==0);}
 if(failures)return 1;std::puts("PASS actual producer + native Service: complete ownership/ACK/release lifecycle and adversarial cases");return 0;
}
