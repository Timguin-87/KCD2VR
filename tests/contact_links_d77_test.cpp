// Production contact POD -> production F7 queue -> production worker formatter.
// No duplicate contact algorithm and no mocked engine contact query.
#include "diagnostics/vrik_trace_h29.h"
#include "vrik/npc_contact172.h"
#include "vrik/contact_budget.h"
struct V3 {float x=0,y=0,z=0;};
struct Q4 {float x=0,y=0,z=0,w=1;};
namespace npc172=kcd2vr::npc_contact172;
#include "vrik/hand_contact_types.inl"
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <vector>
namespace d=kcd2vr::diagnostics::contact_d77;
namespace v=kcd2vr::diagnostics::vrik_h29;
namespace c=kcd2vr::diagnostics::capture168;
static unsigned checks=0;
#define CHECK(x) do {++checks;if(!(x)){std::printf("FAIL line %d: %s\n",__LINE__,#x);std::exit(1);}} while(0)
static d::Evidence copied(bool selected=true){
    HandContact6695 h{};kcd2vr::contact::Budget b{};b.begin_work();b.used=11;b.remaining=7;b.spent=654;
    h.have=h.fresh169r1=h.blocked=true;h.character=reinterpret_cast<void*>(0x1234);h.epoch=17;h.lastCompleteTickH18=888;
    h.worldAllowed={.1f,.2f,.3f};h.presentationWorldD53={.4f,.5f,.6f};h.presentationTickD53=777;h.presentationTrackingD53=3;
    h.rawFeatureReady55=true;h.rawFeatureWorld55[0]={.7f,.8f,.9f};h.fingerContact[2]=true;h.curl[2]=.4;h.hardCurlH31[2]=.2;h.springH31.digit[2].gapWorld=.002;
    h.releaseActiveD45=h.releaseBodySurfaceD71=true;h.releaseCorrectionWorldD45={.01f,0,0};h.bodyPartRequeryMaskD71=4;
    h.planeCount=1;auto& p=h.planes[0];p.feature=3;p.point={1,2,3};p.normal={0,0,1};p.witness169r1=55;
    p.native172.collider=0x9876;p.native172.owner=0x5432;p.native172.entityId=321;p.native172.part=7;
    p.native172.surfaceIdx=11;p.native172.bodyMaterial=12;p.native172.body=npc172::Body::Articulated;
    p.native172.mask=npc172::BodiesAndWorld;p.native172.ownerRead=true;p.native172.bodyCurrentSampleD71=42;
    const auto before=h;const auto beforeBudget=b;d::Evidence e{};
    d::copy(e,selected,h,true,true,true,false,true,42,b);
    CHECK(std::memcmp(&before,&h,sizeof(h))==0);CHECK(std::memcmp(&beforeBudget,&b,sizeof(b))==0);
    return e;
}
static void copyAndEdges(){
    auto e=copied(false);CHECK(!e.observed);CHECK(std::string(d::state(e))=="NOT_REACHED");
    e=copied();CHECK(e.observed&&e.character==0x1234&&e.contactTick==888&&e.epoch==17);
    CHECK(e.budgetUsed==11&&e.budgetRemaining==7&&e.budgetSpentUs==654&&e.budgetSample==42);
    CHECK(e.planes[0].collider==0x9876&&e.planes[0].entity==321&&e.planes[0].part==7&&e.planes[0].currentBodySample==42);
    CHECK(e.fingerMask==4&&e.curl[2]==.4&&e.hardCurl[2]==.2&&e.springGap[2]==.002);
    CHECK(e.release&&e.releaseSources==4&&e.partRequery==4&&e.rawHistory);
    CHECK(std::string(d::state(e))=="CURRENT_BLOCKED");CHECK(d::edges(e,e)==0);
    auto p=e;p.planes[0].witness++;p.contactTick++;CHECK(d::edges(e,p)==0); // sample/witness advance is not an edge.
    p=e;p.planes[0].normal={.01,0,.99995};CHECK(d::edges(e,p)==0);
    p=e;p.planes[0].normal={1,0,0};CHECK(d::edges(e,p)==d::Normal);
    p=e;p.planes[0].part++;CHECK(d::edges(e,p)==d::Target);
    p=e;p.fingerMask=0;CHECK(d::edges(e,p)==d::Finger);
    p=e;p.fresh=false;CHECK(d::edges(e,p)==d::Coverage);CHECK(std::string(d::state(p))=="CURRENT_QUERY_UNPROVEN");
    p=e;p.blocked=false;p.count=0;CHECK(std::string(d::state(p))=="CURRENT_CLEAR");
    p.freeHand=false;CHECK(std::string(d::state(p))=="NOT_FREE_HAND_OWNER");
    p.freeHand=true;p.skipWorld=true;CHECK(std::string(d::state(p))=="WORLD_QUERY_SKIPPED");
}
static void actualQueue(){
    v::Record r{};r.kind=v::Kind::Cycle;r.tick=100;r.sample=42;r.sourceTick=888;r.poseData=true;
    std::snprintf(r.stage,sizeof(r.stage),"%s","D77_TEST_EXISTING_CONSUMER");r.arms[0].context.contactD77=copied();
    std::vector<std::string> lines;const auto writer=[&](const char* s){lines.emplace_back(s);};
    CHECK(!v::push(r));v::drain(100,writer);CHECK(lines.empty());
    c::gate.select(c::Domain::Combat,true,100);CHECK(!v::push(r)); // F3 cannot turn these rows on.
    c::gate.select(c::Domain::Vrik,true,100);CHECK(v::push(r));v::drain(100,writer);
    std::string joined;for(const auto& l:lines){joined+=l;joined+='\n';CHECK(l.size()<2048);}
    CHECK(joined.find("CONTACT_LINK_D77 session=1 sample=42 side=0 observerOrdinal=1 kind=0 state=CURRENT_BLOCKED")!=joined.npos);
    CHECK(joined.find("CONTACT_PLANES_D77 session=1 sample=42 side=0")!=joined.npos);
    CHECK(joined.find("9876,5432,321,7,3,11F,11,12,1,0,42")!=joined.npos);
    CHECK(joined.find("meshSubmission=UNOBSERVED")!=joined.npos);
    CHECK(c::accept("VRIK_F7 CONTACT_LINK_D77",100));
    // Existing 25ms cap cannot be bypassed even by a target change.
    r.tick=110;r.sample++;r.arms[0].context.contactD77.planes[0].part=8;CHECK(!v::push(r));
    r.tick=126;r.sample++;r.arms[0].context.contactD77.planes[0].part=9;CHECK(v::push(r));
    lines.clear();v::drain(126,writer);bool targetEdge=false;
    for(const auto& l:lines)if(l.find("CONTACT_LINK_D77")!=l.npos)targetEdge|=l.find("edges=4 ")!=l.npos;
    CHECK(targetEdge);r.tick=152;r.sample++;CHECK(!v::push(r));
    r.tick=178;r.sample++;r.arms[0].context.contactD77.planes[0].normal={1,0,0};CHECK(v::push(r));
    lines.clear();v::drain(178,writer);bool normalEdge=false;
    for(const auto& l:lines)if(l.find("CONTACT_LINK_D77")!=l.npos)normalEdge|=l.find("edges=8 ")!=l.npos;
    CHECK(normalEdge);r.tick=428;r.sample++;CHECK(v::push(r));
    c::gate.select(c::Domain::Vrik,false,429);lines.clear();v::drain(429,writer);CHECK(lines.empty());
    CHECK(!v::push(r));CHECK(!c::accept("VRIK_F7 CONTACT_LINK_D77",429));
}
static void bounds(){
    auto e=copied();e.count=99;const double huge=std::numeric_limits<double>::max();
    e.epoch=e.contactTick=e.budgetSample=e.budgetSpentUs=e.presentationTick=e.presentationTracking=UINT64_MAX;
    e.character=UINTPTR_MAX;e.budgetUsed=e.budgetRemaining=e.partRequery=e.fingerMask=UINT32_MAX;
    e.worldAllowed=e.presentationWorld=e.storedCompleteRawPalmWorld=e.releaseCorrectionWorld=e.clearCorrectionWorld={huge,-huge,huge};
    for(auto& p:e.planes){p.witness=p.currentRigidSample=p.currentBodySample=UINT64_MAX;p.collider=p.owner=UINTPTR_MAX;
        p.entity=p.mask=p.body=UINT32_MAX;p.feature=p.part=p.surface=p.material=INT32_MIN;p.normal=p.point={huge,-huge,huge};}
    for(unsigned i=0;i<5;++i)e.curl[i]=e.hardCurl[i]=e.springGap[i]=huge;
    unsigned rows=0;d::format(e,UINT32_MAX,UINT64_MAX,UINT64_MAX,UINT32_MAX,UINT64_MAX,UINT32_MAX,[&](const char* line){
        ++rows;CHECK(std::strlen(line)<1899);CHECK(std::strlen(line)<2048);
        if(std::strstr(line,"CONTACT_LINK_D77"))CHECK(std::strstr(line,"meshSubmission=UNOBSERVED"));
        if(std::strstr(line,"CONTACT_PLANES_D77"))CHECK(line[std::strlen(line)-1]==']');
    });CHECK(rows==5); // Two summary rows + at most three groups covering seven planes.
    e.count=-1;rows=0;d::format(e,0,1,1,0,1,0,[&](const char*){++rows;});CHECK(rows==2);
}
int main(){copyAndEdges();actualQueue();bounds();std::printf("contact Evidence POD bytes=%zu; two contact payloads per pose=%zu\n",sizeof(d::Evidence),2*sizeof(d::Evidence));std::printf("contact_links_d77: %u checks PASS (copied POD, actual F7 queue/drain, OFF isolation, edge cap, bounds)\n",checks);}
