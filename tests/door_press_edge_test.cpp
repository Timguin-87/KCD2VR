// DS-F25/F34: connected press-edge door token chain (pure, host-replayable).
// Mirrors the PRODUCTION order of pickup_use_producer19c3.inl lines 88-122:
// update -> transport pulse -> complete(sent). The native Use transport is a
// mock leaf; the Controller, publication and Intent validity are the real code.
#include "../src/core/door_use46.h"
#include "../src/core/door_pre_press66.h"
#include <cstdio>
#include <cstring>
namespace d46=kcd2vr::door_use46;
namespace dpp=kcd2vr::door_pre_press66;
static int fails=0;
#define CK(c) do{if(!(c)){std::printf("FAIL %s:%d %s\n",__FILE__,__LINE__,#c);++fails;}}while(0)
static bool g_transportSent=false;static int g_transportCalls=0;
static bool MockRequestPhysicalInteractionUse() noexcept {++g_transportCalls;return g_transportSent;}
static d46::Input pressInput(int side,std::uint64_t now,bool held=true) noexcept {
    d46::Input in{};in.available=true;in.held=held;in.side=side;in.now=now;in.press=now-20;
    in.poseTick=now-20;in.poseEpoch=2;in.sourceGeneration=1;in.trackingEpoch=3;
    in.player=0xAAA;in.motion={0,0,0};in.scoped54=true;in.experimentEpoch64=1;in.interactionMode64=2;
    return in;
}
static void warmCache(int side,std::uint32_t target,std::uint64_t tick) noexcept {
    dpp::Entry e{};e.target=target;e.side=side;e.entity=0xBBB;e.tick=tick;e.sourceGeneration=1;e.poseEpoch=2;
    e.point[0]=.5f;e.point[1]=1.0f;e.point[2]=.5f;std::memcpy(e.cls,"AnimDoor",8);dpp::store(side,e);
}
int main(){
    dpp::clear(0);
    // 1) production order happy path: update -> transport(true) -> complete -> valid Intent
    {warmCache(0,123,900);d46::Controller c{};d46::Selection sel{};auto in=pressInput(0,1000);
     auto plan=c.update(in,sel);
     CK(plan.owns&&plan.target==123&&plan.pulse);            // token built BEFORE the transport
     g_transportSent=true;
     const bool sent=plan.pulse?MockRequestPhysicalInteractionUse():false; // the production pulse
     c.complete(plan,sent);
     CK(sent&&g_transportCalls==1);
     d46::Intent i{};CK(d46::read(i,1000)&&i.target==123&&i.sent&&i.press==in.press);
     // same squeeze: the next frame never produces a second pulse
     auto plan2=c.update(in,sel);CK(plan2.owns&&plan2.target==123&&!plan2.pulse);
     c.cancel();}
    // 2) transport FAILS: complete(false) cancels, no Intent survives
    {warmCache(0,124,1900);d46::Controller c{};d46::Selection sel{};auto in=pressInput(0,2000);
     auto plan=c.update(in,sel);CK(plan.pulse);
     g_transportSent=false;
     const bool sent=plan.pulse?MockRequestPhysicalInteractionUse():false;
     c.complete(plan,sent);
     d46::Intent i{};CK(!d46::read(i,2000)); // cancelled, nothing sent
     c.cancel();}
    // 3) re-entrant update during the transport: still exactly ONE pulse
    {warmCache(0,125,2900);d46::Controller c{};d46::Selection sel{};auto in=pressInput(0,3000);
     auto plan=c.update(in,sel);CK(plan.pulse);
     g_transportSent=true;
     const bool sent=(plan.pulse)?MockRequestPhysicalInteractionUse():false;
     auto reentrant=c.update(in,sel);CK(reentrant.owns&&!reentrant.pulse); // re-entrant call inside transport
     c.complete(plan,sent);
     CK(g_transportCalls==3); // 1 + 1 (failure case) + 1
     c.cancel();}
    // 4) repeat squeeze on the SAME press (held stays true, new now): one token
    {warmCache(0,126,3900);d46::Controller c{};d46::Selection sel{};auto in=pressInput(0,4000);
     auto plan=c.update(in,sel);g_transportSent=true;
     const bool sent=plan.pulse?MockRequestPhysicalInteractionUse():false;
     c.complete(plan,sent);
     auto in2=pressInput(0,4020);in2.press=in.press; // same physical squeeze, later frame
     auto plan2=c.update(in2,sel);CK(plan2.owns&&plan2.target==126&&!plan2.pulse);
     c.cancel();}
    // 5) release before surface: held=false -> no pulse, no token
    {warmCache(0,127,4900);d46::Controller c{};d46::Selection sel{};auto in=pressInput(0,5000,false);
     auto plan=c.update(in,sel);
     CK(!plan.pulse);
     d46::Intent i{};CK(!d46::read(i,5000));
     c.cancel();}
    // 6) stale cache (>500 ms) never nominates
    {warmCache(0,128,100);d46::Controller c{};d46::Selection sel{};auto in=pressInput(0,6000);
     auto plan=c.update(in,sel);CK(plan.owns&&plan.target==0&&!plan.pulse);
     c.cancel();}
    // DS-F50/F53 (Codex F43/F45): nearest-eligibility from the SAME current palm
    {dpp::Entry a{};a.target=1;a.side=0;a.point[0]=.1f;a.point[1]=0;a.point[2]=0;a.distance=.01f;a.entity=0xAAA;a.poseEpoch=2;
     CK(dpp::nearerNow(a,1.1f,0,0,.04f));  // hand moved: A now 1.0 m, B 0.2 m - the NEW door replaces (old .01 never compared)
     CK(!dpp::nearerNow(a,1.1f,0,0,1.21f)); // new door 1.1 m vs A 1.0 m: A stays nearer
     CK(!dpp::nearerNow(a,.05f,0,0,.04f));  // new door .2 m vs A .05 m: A stays nearer
     CK(dpp::qualified(a,0xAAA,2));        // valid owner joins
     CK(!dpp::qualified(a,0xBBB,2));       // wrong player: invalidated
     CK(!dpp::qualified(a,0xAAA,3));       // stale pose epoch: invalidated
     dpp::Entry nan{};nan.target=2;nan.side=0;nan.point[0]=std::nanf("");nan.entity=0xAAA;nan.poseEpoch=2; // valid owner, nonfinite point (DS-F56)
     CK(!dpp::nearerNow(nan,0,0,0,.01f)); // a NaN nominee never blocks
     CK(!dpp::qualified(nan,0xAAA,2));    // and is invalidated by the producer
     CK(!dpp::nearerNow(a,0,0,0,std::nanf(""))); // nonfinite new d2: no replace
    }
    if(fails)return 1;std::puts("door production-order token chain OK");return 0;
}
