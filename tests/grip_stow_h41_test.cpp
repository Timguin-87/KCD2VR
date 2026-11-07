// Includes the ACTUAL producer and native lifecycle; only engine leaves are mocked.
#include "../src/subsystems/physics/runtime/native_grab_producer60.h"
#include "../src/subsystems/physics/runtime/native_grab_producer60.inl"
#include <cstdio>
#include <cstdlib>
namespace g=kcd2vr::physics::grab_producer60;
namespace cs=kcd2vr::constraint60;
static int failures=0,checks=0;
#define CK(x) do{++checks;if(!(x)){std::printf("FAIL line %d: %s\n",__LINE__,#x);++failures;}}while(0)
struct Native {
    g::Owner owner{};g::Stamp clock{0xAABB,7,1};std::uint64_t now=1000,token=0,addSerial=0,removeSerial=0;
    unsigned prepares=0,adds=0,removes=0,drives=0,retires=0,refs=0;std::uint64_t addBySide[2]{},removeBySide[2]{};
    bool preparePending=false,addPending=false,addUncertain=false,holdPresent=false,holdAbsent=false,wrongOwner=false,wrongId=false;
    bool stampFail=false;unsigned resets=0;bool follow=true,wrongReply=false,retirePending=false,removeUncertain=false;g::Progress driveResult=g::Progress::Complete;
};
static bool stamp(void* p,g::Stamp& out) noexcept{auto&n=*static_cast<Native*>(p);out=n.clock;return !n.stampFail;}
static g::Progress prepare(void* p,const g::Intent& in,std::uint64_t request,g::Owner& out) noexcept{
 auto& n=*static_cast<Native*>(p);++n.prepares;n.refs=2;n.owner=out;n.owner.world=n.clock.world;n.owner.worldGeneration=n.clock.generation;n.owner.proxyGeneration=99;
 out=n.owner;return n.preparePending?g::Progress::Pending:g::Progress::Complete;
}
static g::Progress drive(void* p,const g::Owner&,const g::Intent&) noexcept{auto& n=*static_cast<Native*>(p);++n.drives;return n.driveResult;}
static g::Result add(void* p,const g::Owner& owner,std::uint64_t token) noexcept{
 auto& n=*static_cast<Native*>(p);++n.adds;n.token=token;
 if(n.addPending)return {};
 n.addBySide[owner.side]=n.addSerial=n.clock.serial;auto result=g::Result{owner,token,n.clock.serial,77,g::Progress::Complete};
 if(n.wrongReply)++result.owner.requestToken;
 if(n.addUncertain)result.progress=g::Progress::Uncertain;
 return result;
}
static g::Sample read(void* p,const g::Owner& owner,std::uint64_t token,std::uint32_t id) noexcept{
 auto& n=*static_cast<Native*>(p);g::Sample s{owner,token,n.clock.serial,id?id:77,cs::Status::Kind::Unknown,n.follow};
 if(n.wrongOwner)++s.owner.requestToken;if(n.wrongId)++s.id;
 if(n.removeBySide[owner.side]>n.addBySide[owner.side]&&n.clock.serial>n.removeBySide[owner.side]&&!n.holdAbsent)s.kind=cs::Status::Kind::Absent;
 else if(n.addBySide[owner.side]&&n.clock.serial>n.addBySide[owner.side]&&!n.holdPresent)s.kind=cs::Status::Kind::Present;
 return s;
}
static g::Result remove(void* p,const g::Owner& owner,std::uint64_t token,std::uint32_t id) noexcept{
 auto& n=*static_cast<Native*>(p);++n.removes;n.removeBySide[owner.side]=n.removeSerial=n.clock.serial;return {owner,token,n.clock.serial,id,n.removeUncertain?g::Progress::Uncertain:g::Progress::Complete};
}
static g::Progress retire(void* p,const g::Owner&) noexcept{auto& n=*static_cast<Native*>(p);++n.retires;if(n.retirePending)return g::Progress::Pending;n.refs=0;return g::Progress::Complete;}
static g::Progress nativeReset(void*p,const g::Owner&)noexcept{auto&n=*static_cast<Native*>(p);++n.resets;n.refs=0;return g::Progress::Complete;}
static g::Backend backend(){return {stamp,prepare,drive,add,read,remove,retire,nativeReset};}
static g::Intent intent(std::uint64_t press=1){g::Intent i{};i.player=0x1234;i.entity=100;i.expectedBody=0xB0D;i.physicsId=41;i.side=0;i.press=press;
 i.poseEpoch=3;i.trackingEpoch=4;i.sourceGeneration=5;i.allowed=i.held=true;return i;}
static void reset(){for(auto& s:g::g_session)s={};for(auto& r:g::g_transferLocal)r={};for(auto& r:g::g_transferTicket)r.store(0);g::SetEnabled(true);}
static void frame(Native& n,g::Intent& i,bool refresh=true){n.now+=10;++n.clock.serial;if(refresh)i.tick=i.poseTick=n.now;CK(g::PublishGrip(i));g::PhysicsStep(&n,n.now);}
static g::Status status(){g::Status s{};CK(g::ReadStatus(0,s));return s;}

#include "../src/subsystems/throw_drop/runtime/grip_stow_cb1.cpp"
namespace st=kcd2vr::grip_stow_cb1;
static unsigned preparesScript=0,commits=0,cancels=0;static Native* engine=nullptr;static bool currentIdentity=true,missingScript=false,reenter=false,prepared=false,resetDuringPrepare=false,toggleDuringCurrent=false;
static st::Identity identity{10,0x1234,0xDEF,0xAAA,0xBBB,3,4,9,2};
static bool current(const g::Owner&,std::uint64_t,st::Identity&out)noexcept{out=identity;if(toggleDuringCurrent){toggleDuringCurrent=false;st::SetEnabled(false);st::SetEnabled(true);}return currentIdentity;}
namespace kcd2vr::diagnostic_trace {void Write(const char*)noexcept{}}
namespace kcd2vr::native_script_t2 {
bool Register(Client,Wanted)noexcept{return true;}bool Install()noexcept{return true;}
bool Context::CallPacket(const char*,const char*in,char*out,std::size_t cap)const noexcept{
 unsigned long long serial;char action[16]{};unsigned pl,entity;int side;
 CK(std::sscanf(in,"%llu|%15[^|]|%u|%u|%d",&serial,action,&pl,&entity,&side)==5);CK(pl==identity.player&&entity==100);
 if(missingScript)return false;
 const char*status="CANCELLED";
 if(!std::strcmp(action,"PREPARE")){
  ++preparesScript;CK(engine->refs==2);
  if(reenter){const auto count=engine->drives;++engine->clock.serial;g::PhysicsStep(engine,engine->now);CK(engine->drives>count);}if(resetDuringPrepare){engine->stampFail=true;g::PhysicsReset(engine,engine->now);engine->stampFail=false;CK(engine->resets==1&&engine->refs==0);}prepared=true;status="PREPARED";
 }else if(!std::strcmp(action,"COMMIT")){++commits;CK(prepared&&engine->refs==0&&g::ReadTransfer(st::active.owner,st::active.serial)==g::TransferProgress::Retired);status="NATIVE_RETURNED";}
 else{++cancels;prepared=false;}
 std::snprintf(out,cap,"OK|%llu|%s|%s",serial,action,status);return true;
}}
static void service(Native&n){kcd2vr::native_script_t2::Context c{};c.now=n.now;c.generation=11;st::Service(c);}
static void drain(Native&n){++n.clock.serial;n.now+=10;g::PhysicsStep(&n,n.now);}
static void held(Native&n,g::Intent&i){reset();engine=&n;currentIdentity=true;missingScript=false;prepared=false;reenter=false;resetDuringPrepare=false;identity={10,0x1234,0xDEF,0xAAA,0xBBB,3,4,9,2};frame(n,i);frame(n,i);CK(status().state==g::State::Held);}
int main(){
 CK(g::SetSourcePredicateD41(+[](const g::Intent&) noexcept {return true;}));
 kcd2vr::collab_h21::setPhysicsMode(2);CK(g::InstallBackend(backend()));st::SetCurrentReader(current);CK(st::Install());CK(!st::Enabled());CK(st::Button()==0);st::SetButton(2);CK(st::Button()==2);st::SetEnabled(true);
 // Actual physical hold -> read-only original WUID prepare under metadata lease
 // -> native remove/Absent/retire -> once-only script transfer.
 {Native n{};auto i=intent(100);held(n,i);CK(st::Request(0,n.now));CK(!st::Request(0,n.now));reenter=true;service(n);CK(preparesScript==1&&commits==0);CK(g::ReadTransfer(st::active.owner,st::active.serial)!=g::TransferProgress::Retired);
 service(n);CK(commits==0);drain(n);CK(n.removes==1&&n.refs==2);n.retirePending=true;drain(n);service(n);CK(commits==0&&n.refs==2);n.retirePending=false;drain(n);CK(n.refs==0);service(n);CK(commits==1&&!st::Wanted());service(n);CK(commits==1);
 // Same physical press cannot reacquire; new original press works after transfer.
 frame(n,i);CK(n.adds==1);i=intent(101);n.removeSerial=n.addSerial=0;frame(n,i);CK(n.adds==2);}
 // Prepared body/request replacement or feature OFF/ON cancels only script
 // transfer; original physical references still have to retire normally.
 {Native n{};auto i=intent(102);held(n,i);CK(st::Request(0,n.now));service(n);const auto before=commits;identity.request++;drain(n);drain(n);service(n);CK(commits==before&&!st::Wanted());}
 {Native n{};auto i=intent(103);held(n,i);CK(st::Request(0,n.now));service(n);const auto before=commits;st::SetEnabled(false);st::SetEnabled(true);drain(n);drain(n);service(n);CK(commits==before&&!st::Wanted());}
 // A lookup cannot relabel the original press with a new OFF/ON generation.
 {Native n{};auto i=intent(111);held(n,i);toggleDuringCurrent=true;CK(!st::Request(0,n.now)&&!st::Wanted());}
 // Stale Held publication must not consume a normal face-button press.
 {Native n{};auto i=intent(109);held(n,i);n.now+=81;CK(!st::Request(0,n.now)&&!st::Wanted());}
 // OFF before the first Lua service cannot cancel a newly installed hold or
 // revive a stale request on the later ON transition.
 {Native n{};auto i=intent(110);held(n,i);CK(st::Request(0,n.now));const auto before=g::g_cancel[0].load();st::SetEnabled(false);service(n);st::SetEnabled(true);CK(!st::Wanted()&&n.refs==2&&g::g_cancel[0].load()==before);}
 // Missing script/unknown reply never cancels physical owner or invokes AddItem.
 {Native n{};auto i=intent(104);held(n,i);CK(st::Request(0,n.now));missingScript=true;const auto before=commits;service(n);CK(commits==before&&!st::Wanted()&&n.refs==2&&status().state==g::State::Held);}
 // Changed actual owner cannot use stale published Held status to cancel it.
 {Native n{};auto i=intent(105);held(n,i);CK(st::Request(0,n.now));const auto before=g::g_cancel[0].load();g::g_session[0].owner.requestToken++;service(n);drain(n);service(n);CK(g::g_cancel[0].load()==before&&!st::Wanted());}
 // A genuine native reset during the read-only capture is never blocked.
 // The later T4 consumer rejects the now-ended original request, not re-stamp it.
 {Native n{};auto i=intent(107);held(n,i);CK(st::Request(0,n.now));const auto before=commits;resetDuringPrepare=true;service(n);CK(n.refs==0&&n.resets==1);drain(n);service(n);CK(commits==before&&!st::Wanted());}
 // Busy actual metadata means unknown, not retirement; opposite side stays free.
 {Native n{};auto i=intent(106);held(n,i);const auto own=status().owner;CK(g::PrepareTransfer(own,777,n.now,[](void*)noexcept{return true;},nullptr));CK(g::ReadTransfer(own,777)!=g::TransferProgress::Retired);
 auto opposite=i;opposite.side=1;opposite.press++;frame(n,opposite);frame(n,opposite);g::Status os{};CK(g::ReadStatus(1,os)&&os.state==g::State::Held);CK(g::EndTransfer(own,777));}
 std::printf("H41 actual GP+stow service checks=%d failures=%d\n",checks,failures);return failures?1:0;
}
