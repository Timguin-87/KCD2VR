// Production publication API; callbacks model a reader at each native-registration
// boundary while a new pose is being published. No engine or rendering is mocked
// into successful reload behavior here: this fixture tests source continuity only.
#include "subsystems/ranged_reload/crossbow_native_xr34.h"
#include <iostream>
namespace x=kcd2vr::crossbow_native_xr34;
static int checks=0,failures=0;
#define CHECK(c) do {++checks;if(!(c)){++failures;std::cerr<<"FAIL_R93 line="<<__LINE__<<" " #c "\n";}}while(0)
static bool ready=true,inside=false,observe=false,withdraw=false;
static int registrationCalls=0,readObservations=0;
static std::uint64_t observedSample=0;
static bool registered() noexcept {
    if(inside)return ready;
    inside=true;++registrationCalls;
    if(observe){x::Source s{};CHECK(x::ReadSource(s));CHECK(s.sample==100||s.sample==101);
        observedSample=s.sample;++readObservations;}
    if(withdraw&&registrationCalls==2){x::Source absent{};x::PublishSource(absent);}
    inside=false;return ready;
}
static x::Source valid(int kind=14){x::Source s{};
    s.allowed=true;s.player=1;s.equipment=2;s.manager=3;s.character=4;s.weaponItem=5;s.ammoItem=6;
    s.key=7;s.ammoKey=8;s.generation=9;s.body=10;s.tracking=11;s.request=12;s.sample=100;s.tick=1000;
    s.actor=13;s.primary=1;s.classId=kind;s.physicalD81=true;return s;
}
static void reset(){observe=withdraw=inside=false;ready=true;registrationCalls=readObservations=0;
    x::registrationReader=&registered;x::integrated=true;x::sourceWriterR93.clear();
    x::Source absent{};x::PublishSource(absent);x::completedNativeLease=0;x::activeNativeLease=0;}
int main(){
    for(int kind:{10,14,15}){
        reset();auto s=valid(kind);x::PublishSource(s);x::Source first{};CHECK(x::ReadSource(first));
        CHECK(x::fresh(first,1080)&&!x::fresh(first,1081));
        // A current positive source remains readable throughout the next
        // positive refresh, including after its mailbox copy before enable.
        observe=true;registrationCalls=0;++s.sample;++s.tick;x::PublishSource(s);observe=false;
        CHECK(readObservations>=2&&observedSample==101);x::Source next{};
        CHECK(x::ReadSource(next)&&next.sample==101&&next.tick==1001&&next.publication>first.publication);
        // Busy producer retains the prior packet with its ORIGINAL tick.
        x::sourceWriterR93.test_and_set();s.sample=102;s.tick=1002;x::PublishSource(s);x::sourceWriterR93.clear();
        CHECK(x::ReadSource(next)&&next.sample==101&&next.tick==1001);
        // Explicit withdrawal is immediate even while that producer is busy.
        x::sourceWriterR93.test_and_set();x::Source absent{};x::PublishSource(absent);
        CHECK(!x::ReadSource(next));x::sourceWriterR93.clear();
        x::PublishSource(s);CHECK(x::ReadSource(next)&&next.sample==102);
        // Withdrawal after mailbox copy cannot be undone by the older publish.
        registrationCalls=0;withdraw=true;++s.sample;x::PublishSource(s);withdraw=false;
        CHECK(!x::ReadSource(next));x::PublishSource(s);CHECK(x::ReadSource(next)&&next.sample==103);
        ready=false;x::PublishSource(s);CHECK(!x::ReadSource(next));ready=true;
        x::PublishSource(s);CHECK(x::ReadSource(next));x::WithdrawRegistration();CHECK(!x::ReadSource(next));
    }
    std::cout<<"R93 source publication: "<<(checks-failures)<<"/"<<checks<<" PASS\n";
    return failures?1:0;
}
