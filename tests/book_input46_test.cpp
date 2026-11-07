#include "vrik/book_input46.h"
#include <cassert>
#include <cstdio>
namespace bi=kcd2vr::book_input46;
namespace bp=kcd2vr::book_pages45;
namespace ai=kcd2vr::activity_input163;
namespace as=kcd2vr::alchemy_semi46;
struct Pad {bool lb=false,rb=false,dpad_left=false,dpad_right=false;float lx=0,rx=0;};
int main(){
    unsigned checks=0;
    auto expect=[&](bool b){++checks;assert(b);};
    auto& w=ai::witness175;
    w.ready=true;w.transitionDepth175=0;w.Clear();bp::mode=bp::ThumbrestFlick;
    w.Begin(ai::Kind175::Reading,0x1200);
    bi::Identity id{bp::Reading,w.lifetime175.load(),0x1200,{}};
    bp::Result pulse{};pulse.page.left=true;
    Pad pad{};expect(bi::apply(id,bp::ThumbrestFlick,pulse,1000,pad)&&pad.lb&&!pad.rb);
    // A recognized stroke has no entitlement after native End/Begin, even when
    // the allocator reuses the same reading instance address.
    w.End(0x1200);w.Begin(ai::Kind175::Reading,0x1200);pad={};
    expect(!bi::apply(id,bp::ThumbrestFlick,pulse,1001,pad)&&!pad.lb);
    id.lifetime=w.lifetime175.load();expect(bi::current(id,1001));
    w.transitionDepth175=1;expect(!bi::current(id,1001));w.transitionDepth175=0;
    w.ready=false;expect(!bi::current(id,1001));w.ready=true;
    w.active[unsigned(ai::Kind175::Reading)]=0x1300;expect(!bi::current(id,1001));
    w.active[unsigned(ai::Kind175::Reading)]=0x1200;
    bp::mode=bp::Native;pad={};expect(!bi::apply(id,bp::ThumbrestFlick,pulse,1001,pad)&&!pad.lb);
    // Native shoulders remain the user's buttons if an old automatic packet
    // is revoked; there is no synthetic release of an unrelated physical LB.
    pad.lb=true;expect(!bi::apply(id,bp::ThumbrestFlick,pulse,1001,pad)&&pad.lb);
    bp::mode=bp::ThumbrestFlick;w.Clear();w.Begin(ai::Kind175::Alchemy,0x2400);
    const auto life=w.lifetime175.load();
    ai::AlchemyBook45 a{0x2400,0x3500,life,1000,7,42};
    as::hooksReady=true;as::bookGeneration=7;
    as::BookEnable e{};e.instance=a.instance;e.action=a.action;e.lifetime=life;e.generation=7;e.enabled=true;
    expect(as::bookEnable.publish(e));expect(ai::alchemyBook45.publish(a));
    id={bp::Alchemy,life,0,a};pad={};
    expect(bi::apply(id,bp::ThumbrestFlick,pulse,1001,pad)&&pad.lb);
    e.enabled=false;expect(as::bookEnable.publish(e));pad={};
    expect(!bi::apply(id,bp::ThumbrestFlick,pulse,1002,pad)&&!pad.lb);
    e.enabled=true;e.generation=8;as::bookGeneration=8;expect(as::bookEnable.publish(e));
    a.generation=8;expect(ai::alchemyBook45.publish(a));
    expect(!bi::apply(id,bp::ThumbrestFlick,pulse,1003,pad)&&!pad.lb);
    id.alchemy=a;expect(bi::current(id,1003));
    a.entity=43;expect(ai::alchemyBook45.publish(a));expect(!bi::current(id,1003));
    id.alchemy=a;expect(bi::current(id,1003));
    expect(!bi::current(id,1121));expect(!bi::current(id,999));
    as::hooksReady=false;expect(!bi::current(id,1003));as::hooksReady=true;
    w.End(0x2400);expect(!bi::current(id,1003));
    expect(!bi::current({},1003));
    std::printf("book_input46: %u current native identity/read-enable/mode publication checks PASS\n",checks);
}
