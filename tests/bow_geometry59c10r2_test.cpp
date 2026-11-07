#include "../src/vrik/bow_geometry59c10r2.h"
#include <cassert>
#include <cstdio>
#include <limits>
namespace bg=kcd2vr::bow_geometry59c10r2;
int main() {
    bg::Identity id{};id.key=11;id.generation=3;id.request=4;id.epoch=5;
    id.entity=reinterpret_cast<void*>(1);id.character=reinterpret_cast<void*>(2);
    id.pose=reinterpret_cast<void*>(3);id.owner=reinterpret_cast<void*>(4);id.poseType=5;id.scale=1;
    bg::Lifetimes lifetimes{};
    auto first=lifetimes.observe(id,1000,995,20);
    assert(bg::fresh(first,1000));
    bg::Geometry measured{};static_cast<bg::Identity&>(measured)=id;
    measured.lifetime=first.lifetime;measured.tick=1000;measured.nock={0,-.15,.08};
    measured.axis={0,-1,0};measured.drawLength=.548;
    assert(bg::usable(measured,first,1000));
    // A long draw or a missing SKINNING geometry refresh does not cancel the
    // draw while the exact bow still has genuinely fresh successful placements.
    for(std::uint64_t tick=1008;tick<250000;tick+=8) {
        auto p=lifetimes.observe(id,tick,tick-3,tick/8);
        assert(p.lifetime==first.lifetime&&bg::usable(measured,p,tick));
        assert(measured.tick==1000); // never rejuvenate the geometry receipt
    }
    auto current=lifetimes.observe(id,250000,249995,40000);
    assert(bg::usable(measured,current,250075));
    assert(!bg::usable(measured,current,250076)); // source age, not republish age
    assert(!bg::usable(measured,current,249999)); // future publication
    auto oldSource=lifetimes.observe(id,250100,250019,40001);
    assert(!bg::fresh(oldSource,250100));
    auto resumed=lifetimes.observe(id,250110,250108,40002);
    assert(resumed.lifetime!=first.lifetime&&!bg::usable(measured,resumed,250110));
    // Every native identity boundary retires the earlier local calibration.
    for(unsigned field=0;field<10;++field) {
        auto changed=id;
        switch(field) {
        case 0:++changed.key;break;case 1:++changed.generation;break;
        case 2:++changed.request;break;case 3:++changed.epoch;break;
        case 4:changed.entity=reinterpret_cast<void*>(20);break;
        case 5:changed.character=reinterpret_cast<void*>(21);break;
        case 6:changed.pose=reinterpret_cast<void*>(22);break;
        case 7:changed.owner=reinterpret_cast<void*>(23);break;
        case 8:++changed.poseType;break;case 9:changed.scale=1.1;break;
        }
        bg::Lifetimes l{};auto original=l.observe(id,1000,1000,1);
        measured.lifetime=original.lifetime;
        auto other=l.observe(changed,1010,1010,2);
        assert(other.lifetime!=original.lifetime&&!bg::usable(measured,other,1010));
        auto returned=l.observe(id,1020,1020,3);
        assert(returned.lifetime!=original.lifetime&&!bg::usable(measured,returned,1020));
    }
    bg::Lifetimes invalid{};auto valid=invalid.observe(id,1000,1000,1);
    measured.lifetime=valid.lifetime;
    assert(!bg::fresh(invalid.observe({},1001,1001,2),1001));
    auto recovery=invalid.observe(id,1002,1002,3);
    assert(recovery.lifetime!=valid.lifetime&&!bg::usable(measured,recovery,1002));
    measured.lifetime=recovery.lifetime;
    assert(bg::usable(measured,recovery,1002));
    measured.drawLength=0;measured.restStructural175=true;
    assert(bg::usable(measured,recovery,1002)); // no invented full-draw length
    measured.restStructural175=false;assert(!bg::usable(measured,recovery,1002));
    measured.drawLength=.548;measured.axis={};assert(!bg::usable(measured,recovery,1002));
    measured.axis={0,-1,0};measured.nock.x=std::numeric_limits<double>::quiet_NaN();
    assert(!bg::usable(measured,recovery,1002));
    std::puts("PASS bow local lifetime, fresh placement, identity retirement, recenter, malformed geometry");
}
