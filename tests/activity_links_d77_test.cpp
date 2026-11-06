#include "../src/diagnostics/activity_links_d77.h"
#include "../src/subsystems/ranged_reload/crossbow_native_xr34.h"
#include <cassert>
#include <string>
#include <vector>
#include <iostream>
namespace d=kcd2vr::diagnostics::activity_links_d77;
namespace c=kcd2vr::diagnostics::capture168;
namespace xr=kcd2vr::crossbow_native_xr34;
int main(){
    xr::Status s{};s.owner.serial=1;s.owner.parent=2;s.owner.child=3;s.owner.source.player=4;
    s.owner.source.request=5;s.owner.source.generation=6;s.owner.source.key=7;s.owner.source.classId=15;s.owner.source.primary=0;
    s.clockOwned=true;s.toolTakenD72=true;s.phase=xr::Phase::Span;s.reason=xr::Reason::Working;
    xr::Passenger p{};p.valid=true;p.owner=s.owner;p.childStatus=2;p.publication=9;
    p.binding.qualified=true;p.binding.serial=1;p.binding.tick=100;p.binding.sample=99;
    p.binding.toolEntity=101;p.binding.toolKey=102;p.binding.toolAttachment=103;p.binding.toolBound=true;
    p.binding.nativeCocked=true;p.binding.boltEntity=104;p.binding.boltKey=105;p.binding.boltAttachment=106;p.binding.nativeLoaded=true;
    std::vector<std::string> rows;auto sink=[&](c::Domain domain,const char* text){assert(domain==c::Domain::Reload);rows.emplace_back(text);};
    // OFF is a true observer no-op, including before a caller's identity is valid.
    d::observeReload(s,p,0,110);d::drain(110,sink);assert(rows.empty()&&d::recorder.dropped==0&&d::recorder.coalesced==0);
    c::gate.select(c::Domain::Combat,true,110);d::observeReload(s,p,0,111);d::drain(111,sink);assert(rows.empty());
    c::gate.select(c::Domain::Reload,true,120);d::observeReload(s,p,0,130);d::drain(130,sink);
    assert(rows.size()==1&&rows[0].find("toolEntity=65")!=std::string::npos&&rows[0].find("sourceAgeMs=30")!=std::string::npos);
    assert(rows[0].find("nativeLoaded=1")!=std::string::npos&&rows[0].find("actualRenderedPose=NOT_OBSERVED")!=std::string::npos);
    // Per-frame publication/sample changes alone don't spam. Native loaded edge does.
    p.publication++;p.binding.sample++;d::observeReload(s,p,0,131);d::drain(131,sink);assert(rows.size()==1);
    p.binding.nativeLoaded=false;d::observeReload(s,p,0,132);d::drain(132,sink);assert(rows.size()==2);
    // Mismatched owner's payload is explicitly unavailable, not a false native failure.
    p.owner.child=999;d::observeReload(s,p,0,133);d::drain(133,sink);assert(rows.size()==3);
    assert(rows.back().find("join=UNAVAILABLE")!=std::string::npos&&rows.back().find("nativeLoaded=-1")!=std::string::npos&&rows.back().find("toolEntity=0")!=std::string::npos);
    p.owner=s.owner;
    // Captured records have a finite stop drain; replacement sessions reject old rows.
    d::observeReload(s,p,0,134);c::gate.select(c::Domain::Reload,false,135);d::drain(135,sink);assert(rows.size()==4);
    c::gate.select(c::Domain::Reload,true,140);d::observeReload(s,p,0,141);c::gate.select(c::Domain::Reload,true,142);d::drain(142,sink);assert(rows.size()==4);
    d::observeReload(s,p,0,143);d::drain(143,sink);assert(rows.size()==5);
    // A reused serial/key with wrong player or hand cannot manufacture an exact join.
    p.owner.source.player=999;d::observeReload(s,p,0,144);d::drain(144,sink);
    assert(rows.back().find("join=UNAVAILABLE")!=std::string::npos&&rows.back().find("toolEntity=0")!=std::string::npos);
    p.owner=s.owner;d::observeReload(s,p,0,145);d::drain(145,sink);
    p.owner.source.primary=1;d::observeReload(s,p,0,146);d::drain(146,sink);
    assert(rows.back().find("join=UNAVAILABLE")!=std::string::npos&&rows.back().find("nativeLoaded=-1")!=std::string::npos);
    p.owner=s.owner;d::observeReload(s,p,0,147);d::drain(147,sink);
    // Admission uses the same production adapter but never pretends to have a live pose.
    xr::Admission a{};a.owner=s.owner;a.valid=true;a.binding=p.binding;a.childStatus=2;
    d::observeReload(s,a,1,144);d::drain(144,sink);assert(rows.back().find("FIRST_POSE_ADMISSION")!=std::string::npos);
    // Progress-only transitions rate-limit; actual identity/binding transitions don't.
    const auto before=rows.size();s.progress=.2f;d::observeReload(s,p,0,145);d::drain(145,sink);assert(rows.size()==before);
    d::observeReload(s,p,0,400);d::drain(400,sink);assert(rows.size()==before+1);
    // Queue capacity and per-session budget are bounded, with visible loss counts.
    for(unsigned i=0;i<200;++i){s.owner.serial=1000+i;p.owner=s.owner;d::observeReload(s,p,0,500+i);}
    const auto lost=d::recorder.dropped.load();assert(lost>0);d::drain(800,sink);
    c::gate.select(c::Domain::Reload,true,1000);
    for(unsigned i=0;i<2100;++i){s.owner.serial=10000+i;p.owner=s.owner;d::observeReload(s,p,0,1001+i);d::drain(1001+i,sink);}
    assert(d::recorder.budgetLost.load()>0);
    const auto count=rows.size();c::gate.select(c::Domain::Reload,false,4000);d::observeReload(s,p,0,4001);d::drain(4001,sink);assert(rows.size()==count);
    assert(c::gate.active(c::Domain::Combat,4001)); // F11 never manipulates another domain.
    std::cout<<"D77 activity links: real XR34 packet fixture PASS; bounded copied diagnostics only\n";
}
