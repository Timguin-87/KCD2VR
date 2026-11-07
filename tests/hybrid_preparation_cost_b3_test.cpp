#include "../src/vrik/swing_effort59c14.h"
#include <cassert>
#include <cstdio>

namespace se=kcd2vr::swing_effort59c14;
namespace mo=kcd2vr::motion59c4;

static mo::Key key(unsigned gesture) {return {gesture,10,11,12,13,14,15};}
static se::NativeCost54 prepare(const mo::Key& k,std::uint64_t tick) {
    se::NativeCost54 c{};c.key=k;c.actor=20;c.state=21;c.type=-1;c.hand=1;
    c.preparationOnly54=true;c.nativeFields54=true;c.observedTick54=tick;return c;
}
static se::NativeCost54 admitted(const mo::Key& k,std::uint64_t tick) {
    const auto action=std::uintptr_t(100+k.gesture);
    assert(se::actionRegistry54.construct(action,20,true,tick));
    se::Input54 in{k,20,21,tick};se::Action54 binding{};mo::Key credit{};
    assert(se::actionRegistry54.bind(in,action,22,23,tick,binding));
    assert(!se::actionRegistry54.dispatchFinish(binding,true,tick,credit));
    se::NativeCost54 c{};c.key=k;c.actor=20;c.state=21;c.action=action;c.helper=22;c.row=23;
    c.type=1;c.hand=0;c.observedTick54=tick;
    assert(se::actionRegistry54.start(action,tick,c.causal54));return c;
}
static bool fullNativeCost(const mo::Key& k,std::uint64_t tick) {
    se::NativeCostScope54 scope(admitted(k,tick));
    scope.value.wuid=30;scope.value.raised=1;scope.value.struck=1;
    return scope.finish(true,tick+1);
}
int main() {
    se::ledger.clear();se::actionRegistry54.invalidate();
    const auto a=key(1);assert(se::ledger.publish(a,.8,1.6,false,1000));
    {se::NativeCostScope54 p(prepare(a,1000));assert(!p.finish(true,1001));}
    se::Record r{};
    // An unproven prepare is never paid and must not permit a second debit.
    assert(!se::ledger.paidFor54(a,1002));
    assert(se::ledger.reserve(a,1002,r)==se::Claim::Indeterminate);
    // The SAME admitted native action later proves its own full raise+strike.
    // That actual cost may credit this key; no new debit or admission is made.
    assert(fullNativeCost(a,1010));
    assert(se::ledger.paidFor54(a,1011));
    assert(se::ledger.reserve(a,1012,r)==se::Claim::Complete);
    assert(r.tick==1000&&!se::ledger.paidFor54(a,2501)); // no lease renewal

    // A late physical energy publication retains the actual payment clock.
    const auto b=key(2);
    {se::NativeCostScope54 p(prepare(b,2000));assert(!p.finish(true,2001));}
    assert(fullNativeCost(b,2010));
    assert(se::ledger.publish(b,.8,1.6,false,2020));
    assert(se::ledger.paidFor54(b,2021));
    assert(!se::ledger.paidFor54(b,3512));

    // Incomplete/ambiguous irreversible action work remains terminal.
    const auto c=key(3);assert(se::ledger.publish(c,.8,1.6,false,4000));
    {se::NativeCostScope54 p(prepare(c,4000));assert(!p.finish(true,4001));}
    auto native=admitted(c,4010);
    {se::NativeCostScope54 s(native);s.value.wuid=30;s.value.raised=1;
     assert(!s.finish(true,4011));s.value.struck=1;assert(!s.finish(true,4012));}
    assert(!se::ledger.paidFor54(c,4012));
    assert(!se::ledger.nativeComplete54(c,4012));

    // Reserved own-debit uncertainty is never converted into native credit.
    const auto d=key(4);assert(se::ledger.publish(d,.8,1.6,false,5000));
    assert(se::ledger.reserve(d,5000,r)==se::Claim::Ready);
    assert(se::ledger.finish(d,se::Status::Indeterminate));
    {se::NativeCostScope54 p(prepare(d,5001));assert(!p.finish(true,5002));}
    assert(!fullNativeCost(d,5010));
    assert(!se::ledger.paidFor54(d,5011));
    assert(se::ledger.reserve(d,5011,r)==se::Claim::Indeterminate);
    const auto e=key(5);assert(se::ledger.publish(e,.8,1.6,false,6000));
    {se::NativeCostScope54 p(prepare(e,6000));assert(!p.finish(true,6001));}
    se::ledger.cancel(e);assert(!fullNativeCost(e,6010));
    assert(se::ledger.reserve(e,6011,r)==se::Claim::Cancelled);

    // An unwound preparation remains uncredited until independent full proof.
    const auto f=key(6);assert(se::ledger.publish(f,.8,1.6,false,7000));
    {se::NativeCostScope54 p(prepare(f,7000));}
    assert(!se::ledger.paidFor54(f,7001));
    assert(se::ledger.contactProof54(f,7001)==se::ContactProof54::Refuse);
    assert(fullNativeCost(f,7010)&&se::ledger.paidFor54(f,7011));
    auto wrong=f;++wrong.weapon;
    assert(!se::ledger.paidFor54(wrong,7011));
    std::puts("hybrid_preparation_cost_b3: exact subsequent native proof; no debit/revival PASS");
}
