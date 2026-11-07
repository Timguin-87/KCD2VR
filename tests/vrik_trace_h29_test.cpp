#include "diagnostics/vrik_trace_h29.h"
#include <cassert>
#include <cstdio>
#include <string>
namespace v=kcd2vr::diagnostics::vrik_h29;namespace c=kcd2vr::diagnostics::capture168;
static void hands28Checks(){
 namespace h=kcd2vr::diagnostics::hands28;namespace c=kcd2vr::diagnostics::capture168;
 c::gate.mask=0;h::Record r{};r.tick=500;r.session=1;assert(!h::push(r,r.tick));
 h::Gate g;assert(g.due(1,1000,7)==1);assert(g.due(1,1099,7)==0);assert(g.due(1,1100,7)==1);
 assert(g.due(1,1101,8)==2);assert(g.due(1,1102,9)==0);assert(g.due(1,1126,10)==2);
 c::gate.select(c::Domain::HandReference,true,500);r.session=c::gate.session[unsigned(c::Domain::HandReference)];r.hand[0].count=2;r.hand[0].joints[0].nativeKnown=true;
 r.hand[0].joints[0].id=15;r.hand[0].joints[0].native.p={.1,.2,.3};r.hand[0].joints[1].id=16;
 r.hand[0].objectWriter=true;r.hand[0].objectReady=true;r.hand[0].objectSince=320;r.hand[0].objectSequence=44;
 h::dropped=0;assert(!h::lock.test_and_set());assert(!h::push(r,r.tick));h::lock.clear();assert(h::dropped==1);
 assert(h::push(r,r.tick));std::string out;h::drain(501,[&](const char* s){out+=s;out+='\n';});
 assert(out.find("nativeLocal=(0.1,0.2,0.3;")!=out.npos&&out.find("NATIVE_NO_VR_FINAL_UNKNOWN")!=out.npos);
 assert(out.find("actualSource=CACHED_OBJECT_LOCALS")!=out.npos&&out.find("objectSince=320")!=out.npos);
 assert(out.find("finalKnown=0")!=out.npos&&out.find("surfaceGap=")==out.npos);
 assert(out.find("HAND_F10 POSE queueDroppedTotal=1 tick=500")!=out.npos);
 r.kind=h::Record::Kind::Item;assert(h::push(r,r.tick));out.clear();h::drain(501,[&](const char* line){out+=line;});assert(out.find("HAND_F10 ITEM_MEASURED queueDroppedTotal=1 tick=500")!=out.npos);r.kind=h::Record::Kind::Pose;
 assert(h::push(r,r.tick));c::gate.select(c::Domain::HandReference,false,502);out.clear();h::drain(503,[&](const char* s){out+=s;});assert(out.empty());
 c::gate.select(c::Domain::HandReference,true,504);r.tick=504;r.session=c::gate.session[unsigned(c::Domain::HandReference)];r.committed=r.restored=r.published=r.originalReturned=true;r.hand[0].joints[0].finalKnown=true;
 assert(h::push(r,r.tick));h::drain(505,[&](const char* s){out+=s;});assert(out.find("COMMITTED_ALLOWED_LOCALS")!=out.npos);
 c::gate.select(c::Domain::HandReference,false,506);
}
int main(){hands28Checks();v::Record r{};r.tick=100;r.sample=1;std::strcpy(r.stage,"FINGER_LOCALS");assert(!v::push(r));c::gate.select(c::Domain::Vrik,true,100);r.poseData=true;r.arms[0].context.curveTrialRequestedH7=true;r.arms[0].fingers=1;r.arms[0].finger[0].valid=true;r.arms[0].finger[0].id=15;r.arms[0].finger[0].position={1,2,3};r.locateTargetXrNs=9876543210123ll;r.heldInputTargetXrNs[1]=r.locateTargetXrNs;r.heldMotorPath=1;assert(v::push(r));std::string text;v::drain(101,[&](const char* s){text+=s;text+='\n';});assert(text.find("nativeCurveTrial=REFERENCE_UNAVAILABLE_DEFAULT trialReference=UNAVAILABLE_BIN_NOT_ACCEPTED")!=text.npos);assert(text.find("bone=15")!=text.npos&&text.find("1.00000,2.00000,3.00000")!=text.npos);assert(text.find("locateTargetXrNs=9876543210123 heldInputTargetXrNsRL=0/9876543210123 heldMotorPath=1")!=text.npos);assert(c::accept("VRIK_F7 FINGER",101));assert(!c::active(c::Domain::Combat,101));r.tick=126;r.sample=2;r.arms[0].context.curveTrialAvailableH7=true;r.arms[0].context.fingerCall=true;assert(v::push(r));v::drain(126,[&](const char* s){text+=s;text+='\n';});assert(text.find("nativeCurveTrial=FREE_CURVE_APPLIED trialReference=CAPTURED_NATIVE_PUNCH_H45")!=text.npos);c::gate.select(c::Domain::Vrik,false,127);assert(!c::accept("VRIK_F7 FINGER",103));assert(!v::push(r));std::puts("vrik_trace_h29: OFF isolation, F7 domain, exact POD, finger coordinates and target clocks PASS");}
