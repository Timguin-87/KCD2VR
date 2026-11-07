// The existing complete committed-contact fixture, without its historical main.
// Its native AddImpact/capsule leaves count calls; all selection/consumer code is
// current production code. The same test runs against exact D50 as a negative.
#include "native_touch_fixture_d51.inc"
int main(){
    for(int rung=0;rung<4;++rung)for(int selected=0;selected<4;++selected){
        fixture();lab::rung=rung;lab::livingHitImpact=bool(selected&1);lab::livingTouch=bool(selected&2);
        lab::impactLeaseOk=true;fp::touch43::g_on=true;publish(0);publish(1);fp::Dispatch();
        check(impactCalls==0&&capsuleCalls==0,"no added living skeletal actuation from a valid mapped contact at any rung");
        check(characterCalls==0&&statusCalls==0,"paused living path performs no character/joint work");
        fixture();lab::rung=rung;lab::livingHitImpact=bool(selected&1);lab::livingTouch=bool(selected&2);
        lab::impactLeaseOk=true;fp::touch43::g_on=true;capsule(0);capsule(1);fp::Dispatch();
        check(impactCalls==0&&capsuleCalls==0,"no added living capsule impulse from either free hand");
    }
    fixture();fp::touch43::g_on=true;lab::livingTouch=lab::livingHitImpact=true;lab::impactLeaseOk=true;
    const float v[3]{0,0,1};
    check(fp::touch43::CallAddImpact(reinterpret_cast<void*>(character+0x698),base+0x37C66A4,5,v,v)==0&&impactCalls==0,
        "direct or stale native skeletal leaf caller cannot bypass suspension");
    fp::touch43::CapsuleImpulseH11 packet{};
    check(fp::touch43::CallCapsuleH11(base,0xB000,packet)==0&&capsuleCalls==0,
        "direct or stale capsule leaf caller cannot bypass suspension");
    QueryState query{false,true,77};
    {tr::ScopedNpcQueryH9 scope(query,false);check(!query.requested&&query.prepared,
        "paused F9 living preference does not widen a world-only free-hand query");}
    check(!query.requested&&query.prepared&&query.preparedOwner==77,"original outer query owner is preserved");
    std::printf("D51 actual living-contact suspension %u/%u; native IO mocked, no headset run\n",checks-failed,checks);
    return failed?1:0;
}
