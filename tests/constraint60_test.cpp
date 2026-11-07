// DS-F18: typed add/remove-constraint service state machine v3 (Codex 153105 fixes).
#include "../src/vrik/constraint_service60.h"
#include <cstdio>
#include <cstring>
namespace cs=kcd2vr::constraint60;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static constexpr std::uintptr_t WA=0x777,WB=0x888;
static cs::Snap present(unsigned id,std::uintptr_t w=WA){cs::Snap s{};s.status={cs::Status::Kind::Present,id,w};s.world=w;return s;}
static cs::Snap absent(unsigned id,std::uintptr_t w=WA){cs::Snap s{};s.status={cs::Status::Kind::Absent,id,w};s.world=w;return s;}
static cs::Snap statusFault(unsigned id,std::uintptr_t w=WA){cs::Snap s{};s.status={cs::Status::Kind::Fault,id,w};s.world=w;return s;}
int main(){
    // happy lifecycle: create ONCE -> accepted id latched -> present -> drive -> remove -> absent -> release
    {cs::Service s{};cs::Snap snap{};snap.world=WA;
     CK(s.acquire(0xBAD,WA,1000));
     auto r=s.step(snap,1000);CK(r.action==cs::Action::Create&&std::strstr(r.reason,"ISSUING_CREATE"));
     r=s.step(snap,1010);CK(r.action==cs::Action::None&&std::strstr(r.reason,"WAITING_ACCEPTED_ID")); // no second create
     snap.acceptedIdValid=true;snap.acceptedId=77;snap.acceptedToken=s.acquireToken;snap.acceptedToken=s.acquireToken;
     r=s.step(snap,1020);CK(r.stage==cs::Stage::Created&&std::strstr(r.reason,"ID_ACCEPTED_WAITING_STATUS")&&s.id==77);
     r=s.step(snap,1030);CK(r.stage==cs::Stage::Created&&r.action==cs::Action::None); // unknown status: hold
     r=s.step(present(77),1040);CK(r.stage==cs::Stage::Present&&r.action==cs::Action::Drive);
     r=s.step(present(77),1050);CK(r.stage==cs::Stage::Driving&&r.action==cs::Action::Drive);
     CK(s.release(1060)&&s.stage==cs::Stage::Removing);
     r=s.step(present(77),1070);CK(r.stage==cs::Stage::Removing&&r.action==cs::Action::Remove);
     r=s.step(absent(77),1080);CK(r.stage==cs::Stage::Removed);
     r=s.step(absent(77),1090);CK(r.stage==cs::Stage::Idle&&std::strstr(r.reason,"RELEASED_TO_IDLE"));}
    // late create after Grip release: cancel in Requested -> accepted id drains to Removing
    {cs::Service s{};cs::Snap snap{};snap.world=WA;
     CK(s.acquire(0xBAD,WA,1000));(void)s.step(snap,1000);
     CK(s.release(1010));
     snap.acceptedIdValid=true;snap.acceptedId=77;snap.acceptedToken=s.acquireToken;snap.acceptedToken=s.acquireToken;
     auto r=s.step(snap,1020);CK(r.stage==cs::Stage::Removing&&r.action==cs::Action::Remove&&std::strstr(r.reason,"LATE_ACCEPTED_ID_DRAINING"));
     r=s.step(absent(77),1030);CK(r.stage==cs::Stage::Removed);}
    // v3: cancel BEFORE the first step retires cleanly, never emits Create
    {cs::Service s{};cs::Snap snap{};snap.world=WA;
     CK(s.acquire(0xBAD,WA,1000));CK(s.release(1000));
     auto r=s.step(snap,1010);CK(r.stage==cs::Stage::Fault&&std::strstr(r.reason,"CANCELLED_BEFORE_CREATE")&&r.action!=cs::Action::Create&&s.id==0);}
    // create timeout WITHOUT an issued create -> clean fault; reacquire legal
    {cs::Service s{};cs::Snap snap{};snap.world=WA;
     CK(s.acquire(0xBAD,WA,1000));
     auto r=s.step(snap,1501);CK(r.stage==cs::Stage::Fault&&std::strstr(r.reason,"CREATE_TIMEOUT_OR_CLOCK"));
     CK(s.acquire(0xBE,WA,1600));}
    // v3: create timeout AFTER the create was issued -> quarantine (no id),
    // then a late accepted id drains to Removing instead of leaking
    {cs::Service s{};cs::Snap snap{};snap.world=WA;
     CK(s.acquire(0xBAD,WA,1000));(void)s.step(snap,1000); // Create issued
     auto r=s.step(snap,1501);CK(r.stage==cs::Stage::Quarantined&&std::strstr(r.reason,"CREATE_ACCEPT_TIMEOUT_PENDING_NATIVE")&&s.id==0);
     CK(!s.acquire(0xBE,WA,1600)); // still quarantined
     snap.acceptedIdValid=true;snap.acceptedId=88;snap.acceptedToken=s.acquireToken;snap.acceptedToken=s.acquireToken;
     r=s.step(snap,1610);CK(r.stage==cs::Stage::Removing&&r.action==cs::Action::Remove&&std::strstr(r.reason,"QUARANTINE_LATE_ACCEPTED_ID_DRAINING")&&s.id==88);
     r=s.step(absent(88),1620);CK(r.stage==cs::Stage::Removed);}
    // status for a DIFFERENT id never drives/transitions; v3: status WORLD must match too
    {cs::Service s{};cs::Snap snap{};snap.world=WA;
     CK(s.acquire(0xBAD,WA,1000));(void)s.step(snap,1000);
     snap.acceptedIdValid=true;snap.acceptedId=77;snap.acceptedToken=s.acquireToken;snap.acceptedToken=s.acquireToken;(void)s.step(snap,1020);
     auto r=s.step(present(78),1030);CK(r.stage==cs::Stage::Created&&r.action==cs::Action::None); // wrong id: hold
     cs::Snap sw{};sw.status={cs::Status::Kind::Present,77,WB};sw.world=WA; // status world mismatches ours, snap world ok
     r=s.step(sw,1040);CK(r.stage==cs::Stage::Created&&r.action==cs::Action::None); // right id, wrong status world: hold
     r=s.step(present(77),1050);CK(r.stage==cs::Stage::Present&&r.action==cs::Action::Drive);} // right id+world proceeds
    // driving: unknown status holds WITHOUT a drive action; absent quarantines with retained id
    {cs::Service s{};cs::Snap snap{};snap.world=WA;
     CK(s.acquire(0xBAD,WA,1000));(void)s.step(snap,1000);
     snap.acceptedIdValid=true;snap.acceptedId=77;snap.acceptedToken=s.acquireToken;snap.acceptedToken=s.acquireToken;(void)s.step(snap,1020);
     (void)s.step(present(77),1030);(void)s.step(present(77),1040);CK(s.stage==cs::Stage::Driving);
     auto r=s.step({},1050);CK(r.stage==cs::Stage::Driving&&r.action==cs::Action::None&&std::strstr(r.reason,"DRIVING_STATUS_UNKNOWN_HOLD"));
     r=s.step(absent(77),1060);CK(r.stage==cs::Stage::Quarantined&&std::strstr(r.reason,"CONSTRAINT_LOST_DURING_DRIVE")&&s.id==77);}
    // quarantine: acquire refused; resolve needs the EXACT Absent(id, our world)
    {cs::Service s{};cs::Snap snap{};snap.world=WA;
     CK(s.acquire(0xBAD,WA,1000));(void)s.step(snap,1000);
     snap.acceptedIdValid=true;snap.acceptedId=77;snap.acceptedToken=s.acquireToken;snap.acceptedToken=s.acquireToken;(void)s.step(snap,1020);
     (void)s.step(present(77),1030);(void)s.step(present(77),1040);
     (void)s.step(absent(77),1050);CK(s.stage==cs::Stage::Quarantined);
     CK(!s.acquire(0xBE,WB,1100)); // still owns id 77
     CK(!s.resolveQuarantine(present(77),1110)); // still present
     CK(!s.resolveQuarantine(absent(77,WB),1120)); // absent but the WRONG world: not proof
     cs::Snap repl{};repl.worldReplaced=true;CK(!s.resolveQuarantine(repl,1130)); // world changed alone: not proof
     CK(s.resolveQuarantine(absent(77),1140)&&s.stage==cs::Stage::Fault); // exact proof
     CK(s.acquire(0xBE,WB,1200)&&s.stage==cs::Stage::Requested);}
    // v3: Created-stage status fault QUARANTINES the latched id (never a clean fault leak)
    {cs::Service s{};cs::Snap snap{};snap.world=WA;
     CK(s.acquire(0xBAD,WA,1000));(void)s.step(snap,1000);
     snap.acceptedIdValid=true;snap.acceptedId=77;snap.acceptedToken=s.acquireToken;snap.acceptedToken=s.acquireToken;(void)s.step(snap,1020);
     auto r=s.step(statusFault(77),1030);CK(r.stage==cs::Stage::Quarantined&&std::strstr(r.reason,"CREATE_STATUS_FAULT")&&s.id==77);
     CK(!s.acquire(0xBE,WA,1100));CK(s.resolveQuarantine(absent(77),1110));}
    // remove timeout -> quarantine (retains id)
    {cs::Service s{};cs::Snap snap{};snap.world=WA;
     CK(s.acquire(0xBAD,WA,1000));(void)s.step(snap,1000);
     snap.acceptedIdValid=true;snap.acceptedId=77;snap.acceptedToken=s.acquireToken;snap.acceptedToken=s.acquireToken;(void)s.step(snap,1020);
     (void)s.step(present(77),1030);(void)s.step(present(77),1040);
     CK(s.release(2000));auto r=s.step(present(77),3001);
     CK(r.stage==cs::Stage::Quarantined&&std::strstr(r.reason,"REMOVE_TIMEOUT")&&s.id==77);}
    // gates: bad buddy, world=0, double acquire, release when idle
    {cs::Service s{};CK(!s.release(1000));
     CK(!s.acquire(0,WA,1000));CK(!s.acquire(0xBAD,0,1000)); // v3: world 0 refused
     CK(s.acquire(0xBAD,WA,1000)&&!s.acquire(0xBE,WA,1100));}
    if(fails)return 1;std::puts("constraint60 service v3 OK");return 0;
}
